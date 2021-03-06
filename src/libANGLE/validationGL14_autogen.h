// GENERATED FILE - DO NOT EDIT.
// Generated by generate_entry_points.py using data from gl.xml and wgl.xml.
//
// Copyright 2020 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// validationGL14_autogen.h:
//   Validation functions for the OpenGL 1.4 entry points.

#ifndef LIBANGLE_VALIDATION_GL14_AUTOGEN_H_
#define LIBANGLE_VALIDATION_GL14_AUTOGEN_H_

#include "common/PackedEnums.h"

namespace gl
{
class Context;

bool ValidateFogCoordPointer(const Context *context,
                             GLenum type,
                             GLsizei stride,
                             const void *pointer);
bool ValidateFogCoordd(const Context *context, GLdouble coord);
bool ValidateFogCoorddv(const Context *context, const GLdouble *coord);
bool ValidateFogCoordf(const Context *context, GLfloat coord);
bool ValidateFogCoordfv(const Context *context, const GLfloat *coord);
bool ValidateMultiDrawArrays(const Context *context,
                             PrimitiveMode modePacked,
                             const GLint *first,
                             const GLsizei *count,
                             GLsizei drawcount);
bool ValidateMultiDrawElements(const Context *context,
                               PrimitiveMode modePacked,
                               const GLsizei *count,
                               DrawElementsType typePacked,
                               const void *const *indices,
                               GLsizei drawcount);
bool ValidatePointParameteri(const Context *context, GLenum pname, GLint param);
bool ValidatePointParameteriv(const Context *context, GLenum pname, const GLint *params);
bool ValidateSecondaryColor3b(const Context *context, GLbyte red, GLbyte green, GLbyte blue);
bool ValidateSecondaryColor3bv(const Context *context, const GLbyte *v);
bool ValidateSecondaryColor3d(const Context *context, GLdouble red, GLdouble green, GLdouble blue);
bool ValidateSecondaryColor3dv(const Context *context, const GLdouble *v);
bool ValidateSecondaryColor3f(const Context *context, GLfloat red, GLfloat green, GLfloat blue);
bool ValidateSecondaryColor3fv(const Context *context, const GLfloat *v);
bool ValidateSecondaryColor3i(const Context *context, GLint red, GLint green, GLint blue);
bool ValidateSecondaryColor3iv(const Context *context, const GLint *v);
bool ValidateSecondaryColor3s(const Context *context, GLshort red, GLshort green, GLshort blue);
bool ValidateSecondaryColor3sv(const Context *context, const GLshort *v);
bool ValidateSecondaryColor3ub(const Context *context, GLubyte red, GLubyte green, GLubyte blue);
bool ValidateSecondaryColor3ubv(const Context *context, const GLubyte *v);
bool ValidateSecondaryColor3ui(const Context *context, GLuint red, GLuint green, GLuint blue);
bool ValidateSecondaryColor3uiv(const Context *context, const GLuint *v);
bool ValidateSecondaryColor3us(const Context *context, GLushort red, GLushort green, GLushort blue);
bool ValidateSecondaryColor3usv(const Context *context, const GLushort *v);
bool ValidateSecondaryColorPointer(const Context *context,
                                   GLint size,
                                   GLenum type,
                                   GLsizei stride,
                                   const void *pointer);
bool ValidateWindowPos2d(const Context *context, GLdouble x, GLdouble y);
bool ValidateWindowPos2dv(const Context *context, const GLdouble *v);
bool ValidateWindowPos2f(const Context *context, GLfloat x, GLfloat y);
bool ValidateWindowPos2fv(const Context *context, const GLfloat *v);
bool ValidateWindowPos2i(const Context *context, GLint x, GLint y);
bool ValidateWindowPos2iv(const Context *context, const GLint *v);
bool ValidateWindowPos2s(const Context *context, GLshort x, GLshort y);
bool ValidateWindowPos2sv(const Context *context, const GLshort *v);
bool ValidateWindowPos3d(const Context *context, GLdouble x, GLdouble y, GLdouble z);
bool ValidateWindowPos3dv(const Context *context, const GLdouble *v);
bool ValidateWindowPos3f(const Context *context, GLfloat x, GLfloat y, GLfloat z);
bool ValidateWindowPos3fv(const Context *context, const GLfloat *v);
bool ValidateWindowPos3i(const Context *context, GLint x, GLint y, GLint z);
bool ValidateWindowPos3iv(const Context *context, const GLint *v);
bool ValidateWindowPos3s(const Context *context, GLshort x, GLshort y, GLshort z);
bool ValidateWindowPos3sv(const Context *context, const GLshort *v);
}  // namespace gl

#endif  // LIBANGLE_VALIDATION_GL14_AUTOGEN_H_
