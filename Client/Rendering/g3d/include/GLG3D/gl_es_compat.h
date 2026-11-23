#ifndef G3D_GL_ES_COMPAT_H
#define G3D_GL_ES_COMPAT_H

#if defined(G3D_GLES_COMPAT) && !defined(G3D_GLES_COMPAT_IMPL)

#ifdef __cplusplus
extern "C" {
#endif

void rbx_gl_es_reset_state(void);
void rbx_gl_es_set_visual_style(int mode);

/* Core 1.x / 2.0 compatibility wrappers */
void rbx_glAlphaFunc(GLenum func, GLclampf ref);
void rbx_glBegin(GLenum mode);
void rbx_glEnd(void);
void rbx_glBindTexture(GLenum target, GLuint texture);
void rbx_glBlendFunc(GLenum sfactor, GLenum dfactor);
void rbx_glClear(GLbitfield mask);
void rbx_glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
void rbx_glClearDepth(GLclampd depth);
void rbx_glClearStencil(GLint s);
void rbx_glClipPlane(GLenum plane, const GLdouble* equation);
void rbx_glColor3f(GLfloat red, GLfloat green, GLfloat blue);
void rbx_glColor3fv(const GLfloat* v);
void rbx_glColor3d(GLdouble red, GLdouble green, GLdouble blue);
void rbx_glColor3ub(GLubyte red, GLubyte green, GLubyte blue);
void rbx_glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void rbx_glColor4fv(const GLfloat* v);
void rbx_glColor4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
void rbx_glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
void rbx_glColor4ubv(const GLubyte* v);
void rbx_glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
void rbx_glColorMaterial(GLenum face, GLenum mode);
void rbx_glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
void rbx_glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
void rbx_glCullFace(GLenum mode);
void rbx_glDeleteTextures(GLsizei n, const GLuint* textures);
void rbx_glDepthFunc(GLenum func);
void rbx_glDepthMask(GLboolean flag);
void rbx_glDepthRange(GLclampd near_val, GLclampd far_val);
void rbx_glDisable(GLenum cap);
void rbx_glDisableClientState(GLenum cap);
void rbx_glDrawArrays(GLenum mode, GLint first, GLsizei count);
void rbx_glDrawBuffer(GLenum mode);
void rbx_glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
void rbx_glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels);
void rbx_glEnable(GLenum cap);
void rbx_glEnableClientState(GLenum cap);
void rbx_glEvalCoord1f(GLfloat u);
void rbx_glEvalCoord2f(GLfloat u, GLfloat v);
void rbx_glEvalMesh1(GLenum mode, GLint i1, GLint i2);
void rbx_glEvalMesh2(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
void rbx_glEvalPoint1(GLint i);
void rbx_glEvalPoint2(GLint i, GLint j);
void rbx_glFinish(void);
void rbx_glFlush(void);
void rbx_glFogf(GLenum pname, GLfloat param);
void rbx_glFogfv(GLenum pname, const GLfloat* params);
void rbx_glFogi(GLenum pname, GLint param);
void rbx_glFrontFace(GLenum mode);
void rbx_glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
void rbx_glGenTextures(GLsizei n, GLuint* textures);
void rbx_glGetBooleanv(GLenum pname, GLboolean* params);
void rbx_glGetClipPlane(GLenum plane, GLdouble* equation);
void rbx_glGetDoublev(GLenum pname, GLdouble* params);
GLenum rbx_glGetError(void);
void rbx_glGetFloatv(GLenum pname, GLfloat* params);
void rbx_glGetIntegerv(GLenum pname, GLint* params);
void rbx_glGetLightfv(GLenum light, GLenum pname, GLfloat* params);
const GLubyte* rbx_glGetString(GLenum name);
void rbx_glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, GLvoid* pixels);
void rbx_glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params);
void rbx_glGetTexParameteriv(GLenum target, GLenum pname, GLint* params);
void rbx_glHint(GLenum target, GLenum mode);
GLboolean rbx_glIsEnabled(GLenum cap);
GLboolean rbx_glIsTexture(GLuint texture);
void rbx_glLightModelf(GLenum pname, GLfloat param);
void rbx_glLightModelfv(GLenum pname, const GLfloat* params);
void rbx_glLightModeli(GLenum pname, GLint param);
void rbx_glLightf(GLenum light, GLenum pname, GLfloat param);
void rbx_glLightfv(GLenum light, GLenum pname, const GLfloat* params);
void rbx_glLineWidth(GLfloat width);
void rbx_glLoadIdentity(void);
void rbx_glLoadMatrixd(const GLdouble* m);
void rbx_glLoadMatrixf(const GLfloat* m);
void rbx_glMap1f(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat* points);
void rbx_glMap2f(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat* points);
void rbx_glMapGrid1f(GLint un, GLfloat u1, GLfloat u2);
void rbx_glMapGrid2d(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
void rbx_glMapGrid2f(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
void rbx_glMaterialf(GLenum face, GLenum pname, GLfloat param);
void rbx_glMaterialfv(GLenum face, GLenum pname, const GLfloat* params);
void rbx_glMatrixMode(GLenum mode);
void rbx_glMultMatrixd(const GLdouble* m);
void rbx_glMultMatrixf(const GLfloat* m);
void rbx_glNormal3d(GLdouble nx, GLdouble ny, GLdouble nz);
void rbx_glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz);
void rbx_glNormal3fv(const GLfloat* v);
void rbx_glNormalPointer(GLenum type, GLsizei stride, const GLvoid* pointer);
void rbx_glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
void rbx_glPixelStorei(GLenum pname, GLint param);
void rbx_glPixelZoom(GLfloat xfactor, GLfloat yfactor);
void rbx_glPointSize(GLfloat size);
void rbx_glPolygonMode(GLenum face, GLenum mode);
void rbx_glPolygonOffset(GLfloat factor, GLfloat units);
void rbx_glPopAttrib(void);
void rbx_glPopClientAttrib(void);
void rbx_glPopMatrix(void);
void rbx_glPushAttrib(GLbitfield mask);
void rbx_glPushClientAttrib(GLbitfield mask);
void rbx_glPushMatrix(void);
void rbx_glRasterPos4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
void rbx_glReadBuffer(GLenum mode);
void rbx_glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels);
void rbx_glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void rbx_glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
void rbx_glScalef(GLfloat x, GLfloat y, GLfloat z);
void rbx_glScaled(GLdouble x, GLdouble y, GLdouble z);
void rbx_glScissor(GLint x, GLint y, GLsizei width, GLsizei height);
void rbx_glShadeModel(GLenum mode);
void rbx_glStencilFunc(GLenum func, GLint ref, GLuint mask);
void rbx_glStencilMask(GLuint mask);
void rbx_glStencilOp(GLenum fail, GLenum zfail, GLenum zpass);
void rbx_glTexCoord2f(GLfloat s, GLfloat t);
void rbx_glTexCoord2fv(const GLfloat* v);
void rbx_glTexCoord3f(GLfloat s, GLfloat t, GLfloat r);
void rbx_glTexCoord3fv(const GLfloat* v);
void rbx_glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
void rbx_glTexCoord4fv(const GLfloat* v);
void rbx_glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
void rbx_glTexEnvf(GLenum target, GLenum pname, GLfloat param);
void rbx_glTexEnvfv(GLenum target, GLenum pname, const GLfloat* params);
void rbx_glTexEnvi(GLenum target, GLenum pname, GLint param);
void rbx_glTexGenfv(GLenum coord, GLenum pname, const GLfloat* params);
void rbx_glTexGeni(GLenum coord, GLenum pname, GLint param);
void rbx_glTexImage1D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
void rbx_glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
void rbx_glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
void rbx_glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels);
void rbx_glTexParameterf(GLenum target, GLenum pname, GLfloat param);
void rbx_glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params);
void rbx_glTexParameteri(GLenum target, GLenum pname, GLint param);
void rbx_glTranslated(GLdouble x, GLdouble y, GLdouble z);
void rbx_glTranslatef(GLfloat x, GLfloat y, GLfloat z);
void rbx_glVertex2f(GLfloat x, GLfloat y);
void rbx_glVertex2fv(const GLfloat* v);
void rbx_glVertex2d(GLdouble x, GLdouble y);
void rbx_glVertex3f(GLfloat x, GLfloat y, GLfloat z);
void rbx_glVertex3fv(const GLfloat* v);
void rbx_glVertex3d(GLdouble x, GLdouble y, GLdouble z);
void rbx_glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void rbx_glVertex4fv(const GLfloat* v);
void rbx_glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
void rbx_glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

/* Extension entry points bound in GLCaps::loadExtensions */
void rbx_glActiveTextureARB(GLenum texture);
void rbx_glClientActiveTextureARB(GLenum texture);
void rbx_glMultiTexCoord1fARB(GLenum target, GLfloat s);
void rbx_glMultiTexCoord1dARB(GLenum target, GLdouble s);
void rbx_glMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t);
void rbx_glMultiTexCoord2fvARB(GLenum target, const GLfloat* v);
void rbx_glMultiTexCoord2dvARB(GLenum target, const GLdouble* v);
void rbx_glMultiTexCoord3fvARB(GLenum target, const GLfloat* v);
void rbx_glMultiTexCoord3dvARB(GLenum target, const GLdouble* v);
void rbx_glMultiTexCoord4fvARB(GLenum target, const GLfloat* v);
void rbx_glMultiTexCoord4dvARB(GLenum target, const GLdouble* v);
void rbx_glBlendEquationEXT(GLenum mode);
void rbx_glBindBufferARB(GLenum target, GLuint buffer);
void rbx_glDeleteBuffersARB(GLsizei n, const GLuint* buffers);
void rbx_glGenBuffersARB(GLsizei n, GLuint* buffers);
GLboolean rbx_glIsBufferARB(GLuint buffer);
void rbx_glBufferDataARB(GLenum target, GLsizeiptrARB size, const GLvoid* data, GLenum usage);
void rbx_glBufferSubDataARB(GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid* data);
void rbx_glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid* indices);
void rbx_glGenFramebuffersEXT(GLsizei n, GLuint* framebuffers);
void rbx_glDeleteFramebuffersEXT(GLsizei n, const GLuint* framebuffers);
void rbx_glBindFramebufferEXT(GLenum target, GLuint framebuffer);
GLenum rbx_glCheckFramebufferStatusEXT(GLenum target);
void rbx_glFramebufferTexture2DEXT(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
void rbx_glFramebufferRenderbufferEXT(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
void rbx_glGenRenderbuffersEXT(GLsizei n, GLuint* renderbuffers);
void rbx_glDeleteRenderbuffersEXT(GLsizei n, const GLuint* renderbuffers);
void rbx_glBindRenderbufferEXT(GLenum target, GLuint renderbuffer);
void rbx_glRenderbufferStorageEXT(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
void rbx_glGenerateMipmapEXT(GLenum target);

#ifdef __cplusplus
}
#endif

#define glAlphaFunc rbx_glAlphaFunc
#define glBegin rbx_glBegin
#define glEnd rbx_glEnd
#define glBindTexture rbx_glBindTexture
#define glBlendFunc rbx_glBlendFunc
#define glClear rbx_glClear
#define glClearColor rbx_glClearColor
#define glClearDepth rbx_glClearDepth
#define glClearStencil rbx_glClearStencil
#define glClipPlane rbx_glClipPlane
#define glColor3f rbx_glColor3f
#define glColor3fv rbx_glColor3fv
#define glColor3d rbx_glColor3d
#define glColor3ub rbx_glColor3ub
#define glColor4f rbx_glColor4f
#define glColor4fv rbx_glColor4fv
#define glColor4d rbx_glColor4d
#define glColor4ub rbx_glColor4ub
#define glColor4ubv rbx_glColor4ubv
#define glColorMask rbx_glColorMask
#define glColorMaterial rbx_glColorMaterial
#define glColorPointer rbx_glColorPointer
#define glCopyTexImage2D rbx_glCopyTexImage2D
#define glCullFace rbx_glCullFace
#define glDeleteTextures rbx_glDeleteTextures
#define glDepthFunc rbx_glDepthFunc
#define glDepthMask rbx_glDepthMask
#define glDepthRange rbx_glDepthRange
#define glDisable rbx_glDisable
#define glDisableClientState rbx_glDisableClientState
#define glDrawArrays rbx_glDrawArrays
#define glDrawBuffer rbx_glDrawBuffer
#define glDrawElements rbx_glDrawElements
#define glDrawPixels rbx_glDrawPixels
#define glEnable rbx_glEnable
#define glEnableClientState rbx_glEnableClientState
#define glEvalCoord1f rbx_glEvalCoord1f
#define glEvalCoord2f rbx_glEvalCoord2f
#define glEvalMesh1 rbx_glEvalMesh1
#define glEvalMesh2 rbx_glEvalMesh2
#define glEvalPoint1 rbx_glEvalPoint1
#define glEvalPoint2 rbx_glEvalPoint2
#define glFinish rbx_glFinish
#define glFlush rbx_glFlush
#define glFogf rbx_glFogf
#define glFogfv rbx_glFogfv
#define glFogi rbx_glFogi
#define glFrontFace rbx_glFrontFace
#define glFrustum rbx_glFrustum
#define glGenTextures rbx_glGenTextures
#define glGetBooleanv rbx_glGetBooleanv
#define glGetClipPlane rbx_glGetClipPlane
#define glGetDoublev rbx_glGetDoublev
#define glGetError rbx_glGetError
#define glGetFloatv rbx_glGetFloatv
#define glGetIntegerv rbx_glGetIntegerv
#define glGetLightfv rbx_glGetLightfv
#define glGetString rbx_glGetString
#define glGetTexImage rbx_glGetTexImage
#define glGetTexLevelParameteriv rbx_glGetTexLevelParameteriv
#define glGetTexParameteriv rbx_glGetTexParameteriv
#define glHint rbx_glHint
#define glIsEnabled rbx_glIsEnabled
#define glIsTexture rbx_glIsTexture
#define glLightModelf rbx_glLightModelf
#define glLightModelfv rbx_glLightModelfv
#define glLightModeli rbx_glLightModeli
#define glLightf rbx_glLightf
#define glLightfv rbx_glLightfv
#define glLineWidth rbx_glLineWidth
#define glLoadIdentity rbx_glLoadIdentity
#define glLoadMatrixd rbx_glLoadMatrixd
#define glLoadMatrixf rbx_glLoadMatrixf
#define glMap1f rbx_glMap1f
#define glMap2f rbx_glMap2f
#define glMapGrid1f rbx_glMapGrid1f
#define glMapGrid2d rbx_glMapGrid2d
#define glMapGrid2f rbx_glMapGrid2f
#define glMaterialf rbx_glMaterialf
#define glMaterialfv rbx_glMaterialfv
#define glMatrixMode rbx_glMatrixMode
#define glMultMatrixd rbx_glMultMatrixd
#define glMultMatrixf rbx_glMultMatrixf
#define glNormal3d rbx_glNormal3d
#define glNormal3f rbx_glNormal3f
#define glNormal3fv rbx_glNormal3fv
#define glNormalPointer rbx_glNormalPointer
#define glOrtho rbx_glOrtho
#define glPixelStorei rbx_glPixelStorei
#define glPixelZoom rbx_glPixelZoom
#define glPointSize rbx_glPointSize
#define glPolygonMode rbx_glPolygonMode
#define glPolygonOffset rbx_glPolygonOffset
#define glPopAttrib rbx_glPopAttrib
#define glPopClientAttrib rbx_glPopClientAttrib
#define glPopMatrix rbx_glPopMatrix
#define glPushAttrib rbx_glPushAttrib
#define glPushClientAttrib rbx_glPushClientAttrib
#define glPushMatrix rbx_glPushMatrix
#define glRasterPos4d rbx_glRasterPos4d
#define glReadBuffer rbx_glReadBuffer
#define glReadPixels rbx_glReadPixels
#define glRotatef rbx_glRotatef
#define glRotated rbx_glRotated
#define glScalef rbx_glScalef
#define glScaled rbx_glScaled
#define glScissor rbx_glScissor
#define glShadeModel rbx_glShadeModel
#define glStencilFunc rbx_glStencilFunc
#define glStencilMask rbx_glStencilMask
#define glStencilOp rbx_glStencilOp
#define glTexCoord2f rbx_glTexCoord2f
#define glTexCoord2fv rbx_glTexCoord2fv
#define glTexCoord3f rbx_glTexCoord3f
#define glTexCoord3fv rbx_glTexCoord3fv
#define glTexCoord4f rbx_glTexCoord4f
#define glTexCoord4fv rbx_glTexCoord4fv
#define glTexCoordPointer rbx_glTexCoordPointer
#define glTexEnvf rbx_glTexEnvf
#define glTexEnvfv rbx_glTexEnvfv
#define glTexEnvi rbx_glTexEnvi
#define glTexGenfv rbx_glTexGenfv
#define glTexGeni rbx_glTexGeni
#define glTexImage1D rbx_glTexImage1D
#define glTexImage2D rbx_glTexImage2D
#define glTexImage3D rbx_glTexImage3D
#define glTexSubImage2D rbx_glTexSubImage2D
#define glTexParameterf rbx_glTexParameterf
#define glTexParameterfv rbx_glTexParameterfv
#define glTexParameteri rbx_glTexParameteri
#define glTranslated rbx_glTranslated
#define glTranslatef rbx_glTranslatef
#define glVertex2f rbx_glVertex2f
#define glVertex2fv rbx_glVertex2fv
#define glVertex2d rbx_glVertex2d
#define glVertex3f rbx_glVertex3f
#define glVertex3fv rbx_glVertex3fv
#define glVertex3d rbx_glVertex3d
#define glVertex4f rbx_glVertex4f
#define glVertex4fv rbx_glVertex4fv
#define glVertexPointer rbx_glVertexPointer
#define glViewport rbx_glViewport

#endif /* G3D_GLES_COMPAT && !G3D_GLES_COMPAT_IMPL */

#endif /* G3D_GL_ES_COMPAT_H */
