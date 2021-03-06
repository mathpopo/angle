//
// Copyright 2016 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// RenderTargetVk:
//   Wrapper around a Vulkan renderable resource, using an ImageView.
//

#include "libANGLE/renderer/vulkan/RenderTargetVk.h"

#include "libANGLE/renderer/vulkan/ContextVk.h"
#include "libANGLE/renderer/vulkan/ResourceVk.h"
#include "libANGLE/renderer/vulkan/TextureVk.h"
#include "libANGLE/renderer/vulkan/vk_format_utils.h"
#include "libANGLE/renderer/vulkan/vk_helpers.h"

namespace rx
{

RenderTargetVk::RenderTargetVk()
{
    reset();
}

RenderTargetVk::~RenderTargetVk() {}

RenderTargetVk::RenderTargetVk(RenderTargetVk &&other)
    : mImage(other.mImage),
      mImageViews(other.mImageViews),
      mResolveImage(other.mResolveImage),
      mResolveImageViews(other.mResolveImageViews),
      mLevelIndexGL(other.mLevelIndexGL),
      mLayerIndex(other.mLayerIndex),
      mContentDefined(other.mContentDefined)
{
    other.reset();
}

void RenderTargetVk::init(vk::ImageHelper *image,
                          vk::ImageViewHelper *imageViews,
                          vk::ImageHelper *resolveImage,
                          vk::ImageViewHelper *resolveImageViews,
                          uint32_t levelIndexGL,
                          uint32_t layerIndex,
                          bool isImageTransient)
{
    mImage             = image;
    mImageViews        = imageViews;
    mResolveImage      = resolveImage;
    mResolveImageViews = resolveImageViews;
    mLevelIndexGL      = levelIndexGL;
    mLayerIndex        = layerIndex;

    // Conservatively assume the content is defined.
    mContentDefined = true;

    mIsImageTransient = isImageTransient;
}

void RenderTargetVk::reset()
{
    mImage             = nullptr;
    mImageViews        = nullptr;
    mResolveImage      = nullptr;
    mResolveImageViews = nullptr;
    mLevelIndexGL      = 0;
    mLayerIndex        = 0;
    mContentDefined    = false;
}

vk::ImageViewSerial RenderTargetVk::getAssignViewSerialImpl(ContextVk *contextVk,
                                                            vk::ImageViewHelper *imageViews) const
{
    ASSERT(imageViews);
    ASSERT(mLayerIndex < std::numeric_limits<uint16_t>::max());
    ASSERT(mLevelIndexGL < std::numeric_limits<uint16_t>::max());

    vk::ImageViewSerial imageViewSerial =
        imageViews->getAssignSerial(contextVk, mLevelIndexGL, mLayerIndex);
    ASSERT(imageViewSerial.getValue() < std::numeric_limits<uint32_t>::max());
    return imageViewSerial;
}

vk::ImageViewSerial RenderTargetVk::getAssignImageViewSerial(ContextVk *contextVk) const
{
    return getAssignViewSerialImpl(contextVk, mImageViews);
}

vk::ImageViewSerial RenderTargetVk::getAssignResolveImageViewSerial(ContextVk *contextVk) const
{
    return getAssignViewSerialImpl(contextVk, mResolveImageViews);
}

angle::Result RenderTargetVk::onColorDraw(ContextVk *contextVk)
{
    ASSERT(!mImage->getFormat().actualImageFormat().hasDepthOrStencilBits());

    contextVk->onRenderPassImageWrite(VK_IMAGE_ASPECT_COLOR_BIT, vk::ImageLayout::ColorAttachment,
                                      mImage);
    if (mResolveImage)
    {
        contextVk->onRenderPassImageWrite(VK_IMAGE_ASPECT_COLOR_BIT,
                                          vk::ImageLayout::ColorAttachment, mResolveImage);
    }
    retainImageViews(contextVk);

    mContentDefined = true;

    return angle::Result::Continue;
}

angle::Result RenderTargetVk::onDepthStencilDraw(ContextVk *contextVk)
{
    ASSERT(mImage->getFormat().actualImageFormat().hasDepthOrStencilBits());

    const angle::Format &format    = mImage->getFormat().actualImageFormat();
    VkImageAspectFlags aspectFlags = vk::GetDepthStencilAspectFlags(format);

    contextVk->onRenderPassImageWrite(aspectFlags, vk::ImageLayout::DepthStencilAttachment, mImage);
    if (mResolveImage)
    {
        contextVk->onRenderPassImageWrite(aspectFlags, vk::ImageLayout::DepthStencilAttachment,
                                          mResolveImage);
    }
    retainImageViews(contextVk);

    mContentDefined = true;

    return angle::Result::Continue;
}

vk::ImageHelper &RenderTargetVk::getImageForRenderPass()
{
    ASSERT(mImage && mImage->valid());
    return *mImage;
}

const vk::ImageHelper &RenderTargetVk::getImageForRenderPass() const
{
    ASSERT(mImage && mImage->valid());
    return *mImage;
}

vk::ImageHelper &RenderTargetVk::getResolveImageForRenderPass()
{
    ASSERT(mResolveImage && mResolveImage->valid());
    return *mResolveImage;
}

const vk::ImageHelper &RenderTargetVk::getResolveImageForRenderPass() const
{
    ASSERT(mResolveImage && mResolveImage->valid());
    return *mResolveImage;
}

angle::Result RenderTargetVk::getImageViewImpl(ContextVk *contextVk,
                                               const vk::ImageHelper &image,
                                               vk::ImageViewHelper *imageViews,
                                               const vk::ImageView **imageViewOut) const
{
    ASSERT(image.valid() && imageViews);
    int32_t levelVK = mLevelIndexGL - mImage->getBaseLevel();
    return imageViews->getLevelLayerDrawImageView(contextVk, image, levelVK, mLayerIndex,
                                                  imageViewOut);
}

angle::Result RenderTargetVk::getImageView(ContextVk *contextVk,
                                           const vk::ImageView **imageViewOut) const
{
    ASSERT(mImage);
    return getImageViewImpl(contextVk, *mImage, mImageViews, imageViewOut);
}

angle::Result RenderTargetVk::getResolveImageView(ContextVk *contextVk,
                                                  const vk::ImageView **imageViewOut) const
{
    ASSERT(mResolveImage);
    return getImageViewImpl(contextVk, *mResolveImage, mResolveImageViews, imageViewOut);
}

bool RenderTargetVk::isResolveImageOwnerOfData() const
{
    // If there's a resolve attachment and the image itself is transient, it's the resolve
    // attachment that owns the data, so all non-render-pass accesses to the render target data
    // should go through the resolve attachment.
    return hasResolveAttachment() && isImageTransient();
}

angle::Result RenderTargetVk::getAndRetainCopyImageView(ContextVk *contextVk,
                                                        const vk::ImageView **imageViewOut) const
{
    retainImageViews(contextVk);

    const vk::ImageViewHelper *imageViews =
        isResolveImageOwnerOfData() ? mResolveImageViews : mImageViews;

    // If the source of render target is a texture or renderbuffer, this will always be valid.  This
    // is also where 3D or 2DArray images could be the source of the render target.
    if (imageViews->hasCopyImageView())
    {
        *imageViewOut = &imageViews->getCopyImageView();
        return angle::Result::Continue;
    }

    // Otherwise, this must come from the surface, in which case the image is 2D, so the image view
    // used to draw is just as good for fetching.  If resolve attachment is present, fetching is
    // done from that.
    return isResolveImageOwnerOfData() ? getResolveImageView(contextVk, imageViewOut)
                                       : getImageView(contextVk, imageViewOut);
}

const vk::Format &RenderTargetVk::getImageFormat() const
{
    ASSERT(mImage && mImage->valid());
    return mImage->getFormat();
}

gl::Extents RenderTargetVk::getExtents() const
{
    ASSERT(mImage && mImage->valid());
    uint32_t levelVK = mLevelIndexGL - mImage->getBaseLevel();
    return mImage->getLevelExtents2D(levelVK);
}

void RenderTargetVk::updateSwapchainImage(vk::ImageHelper *image,
                                          vk::ImageViewHelper *imageViews,
                                          vk::ImageHelper *resolveImage,
                                          vk::ImageViewHelper *resolveImageViews)
{
    ASSERT(image && image->valid() && imageViews);
    mImage             = image;
    mImageViews        = imageViews;
    mResolveImage      = resolveImage;
    mResolveImageViews = resolveImageViews;
}

vk::ImageHelper &RenderTargetVk::getImageForCopy() const
{
    ASSERT(mImage && mImage->valid() && (mResolveImage == nullptr || mResolveImage->valid()));
    return isResolveImageOwnerOfData() ? *mResolveImage : *mImage;
}

vk::ImageHelper &RenderTargetVk::getImageForWrite() const
{
    ASSERT(mImage && mImage->valid() && (mResolveImage == nullptr || mResolveImage->valid()));
    return isResolveImageOwnerOfData() ? *mResolveImage : *mImage;
}

angle::Result RenderTargetVk::flushStagedUpdates(ContextVk *contextVk,
                                                 vk::ClearValuesArray *deferredClears,
                                                 uint32_t deferredClearIndex)
{
    // This function is called when the framebuffer is notified of an update to the attachment's
    // contents.  Therefore, set mContentDefined so that the next render pass will have loadOp=LOAD.
    mContentDefined = true;

    ASSERT(mImage->valid() && (!isResolveImageOwnerOfData() || mResolveImage->valid()));

    // Note that the layer index for 3D textures is always zero according to Vulkan.
    uint32_t layerIndex = mLayerIndex;
    if (mImage->getType() == VK_IMAGE_TYPE_3D)
    {
        layerIndex = 0;
    }

    vk::ImageHelper *image = isResolveImageOwnerOfData() ? mResolveImage : mImage;

    // All updates should be staged on the image that owns the data as the source of truth.  With
    // multisampled-render-to-texture framebuffers, that is the resolve image.  In that case, even
    // though deferred clears set the loadOp of the transient multisampled image, the clears
    // themselves are staged on the resolve image.  The |flushSingleSubresourceStagedUpdates| call
    // below will either flush all staged updates to the resolve image, or if the only staged update
    // is a clear, it will accumulate it in the |deferredClears| array.  Later, when the render pass
    // is started, the deferred clears are applied to the transient multisampled image.
    ASSERT(!isResolveImageOwnerOfData() || !mImage->isUpdateStaged(mLevelIndexGL, layerIndex));
    ASSERT(isResolveImageOwnerOfData() || mResolveImage == nullptr ||
           !mResolveImage->isUpdateStaged(mLevelIndexGL, layerIndex));

    if (!image->isUpdateStaged(mLevelIndexGL, layerIndex))
    {
        return angle::Result::Continue;
    }

    vk::CommandBuffer *commandBuffer;
    ANGLE_TRY(contextVk->endRenderPassAndGetCommandBuffer(&commandBuffer));
    return image->flushSingleSubresourceStagedUpdates(
        contextVk, mLevelIndexGL, layerIndex, commandBuffer, deferredClears, deferredClearIndex);
}

void RenderTargetVk::retainImageViews(ContextVk *contextVk) const
{
    mImageViews->retain(&contextVk->getResourceUseList());
    if (mResolveImageViews)
    {
        mResolveImageViews->retain(&contextVk->getResourceUseList());
    }
}

gl::ImageIndex RenderTargetVk::getImageIndex() const
{
    // Determine the GL type from the Vk Image properties.
    if (mImage->getType() == VK_IMAGE_TYPE_3D)
    {
        return gl::ImageIndex::Make3D(mLevelIndexGL, mLayerIndex);
    }

    // We don't need to distinguish 2D array and cube.
    if (mImage->getLayerCount() > 1)
    {
        return gl::ImageIndex::Make2DArray(mLevelIndexGL, mLayerIndex);
    }

    ASSERT(mLayerIndex == 0);
    return gl::ImageIndex::Make2D(mLevelIndexGL);
}
}  // namespace rx
