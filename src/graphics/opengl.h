#ifndef __cherry_opengl_h
#define __cherry_opengl_h

	#ifdef BUILD_OPENGL

		#include "types/id.h"

		#if OS == IOS
			#define GL_GLEXT_PROTOTYPES
			#include <OpenGLES/ES3/gl.h>
			#include <OpenGLES/ES3/glext.h>
			//#define GL_MAX_SAMPLES GL_MAX_SAMPLES_APPLE
		#elif OS == OSX
			#define __gl_h_
			#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
			#include <OpenGL/gl3.h>
			#include <OpenGL/glext.h>
		#elif OS == DROID
			#define GL_GLEXT_PROTOTYPES
			#include <EGL/egl.h>
			#include <GLES3/gl3.h>
			#include <GLES3/gl3ext.h>
		#elif OS == WEB
			#define GL_GLEXT_PROTOTYPES
			#include <GL/gl.h>
			#include <GL/glext.h>
		#elif OS == WINDOWS
			#include <GL/glew.h>
		#else
			#define GL_GLEXT_PROTOTYPES
			#include <GL/gl.h>
			#include <GL/glext.h>
		#endif

	#endif

#endif