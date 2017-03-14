#pragma once

#include <GL/gl.h>
#include <gl/glext.h>
#include "io.h"

/*
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
*/

PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
/*
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;

PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
*/

#ifdef IS_SHADING
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
PFNGLUSEPROGRAMOBJECTARBPROC  glUseProgramObjectARB;
PFNGLGETATTRIBLOCATIONARBPROC glGetAttribLocationARB;

PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
#endif

GLenum g_ShadingProgram;

#ifdef _DEBUG
int IsExtEnable(const char * pName)
{
	// Получить строку со списком доступных расширений
	const char * ch = (const char *)glGetString(GL_EXTENSIONS);

	size_t size = strlen(pName);
	while (ch)
	{
		if (!strncmp(ch, pName, size) && (*(ch + size) == 0 || *(ch + size) == ' '))
			return 1;
		ch = strchr(ch, ' ');
		if (ch)
			ch++;
	}

	return 0;
}

int check_shader(GLuint shader)
{
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		GLint logSize = 0;
		int maxLength = MAX_PATH;
		char *errorLog = (char*)LocalAlloc(LPTR, maxLength);
		glGetShaderiv(shader, maxLength, &logSize);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
		LocalFree(errorLog);
	}
	return success;
}
#endif

void init_gl_vbo_arb()
{
	glGenBuffers = (PFNGLGENBUFFERSPROC)
		wglGetProcAddress("glGenBuffers");

	glBindBuffer = (PFNGLBINDBUFFERPROC)
		wglGetProcAddress("glBindBuffer");

	glBufferData = (PFNGLBUFFERDATAPROC)
		wglGetProcAddress("glBufferData");

	/*
	glGenVertexArrays=(PFNGLGENVERTEXARRAYSPROC)
		wglGetProcAddress("glGenVertexArrays");

	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)
		wglGetProcAddress("glBindVertexArray");

	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)
		wglGetProcAddress("glEnableVertexAttribArray");

	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)
		wglGetProcAddress("glVertexAttribPointer");

	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)
		wglGetProcAddress("glDisableVertexAttribArray");
	*/
}

#ifdef IS_SHADING
void init_gl_shader_arb()
{
#ifdef _DEBUG
	if (!IsExtEnable("GL_ARB_multitexture"))
		return 0;
#endif // DEBUG

	/*
	glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)
		wglGetProcAddress("glActiveTextureARB");
	glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)
		wglGetProcAddress("glClientActiveTextureARB");
	glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)
		wglGetProcAddress("glMultiTexCoord2fARB");
	*/

	glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)
		wglGetProcAddress("glCreateProgramObjectARB");
	glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)
		wglGetProcAddress("glCreateShaderObjectARB");
	glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC) 
		wglGetProcAddress("glShaderSourceARB");
	glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC) 
		wglGetProcAddress("glCompileShaderARB");
	glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)
		wglGetProcAddress("glAttachObjectARB");
	glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC) 
		wglGetProcAddress("glLinkProgramARB");
	glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)  
		wglGetProcAddress("glUseProgramObjectARB");
	glGetAttribLocationARB = (PFNGLGETATTRIBLOCATIONARBPROC)
		wglGetProcAddress("glGetAttribLocationARB");

	glGetShaderiv = (PFNGLGETSHADERIVPROC)
		wglGetProcAddress("glGetShaderiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)
		wglGetProcAddress("glGetShaderInfoLog");
}

BOOL load_shaders()
{
	GLenum my_vertex_shader;
	GLenum my_fragment_shader;
	char *my_vertex_shader_source;
	char *my_fragment_shader_source;

#ifdef _DEBUG
	// Get Vertex And Fragment Shader Sources
	my_vertex_shader_source = read_file("v.hlsl");
	my_fragment_shader_source = read_file("f.hlsl");
#endif

	// Create Shader And Program Objects
	g_ShadingProgram = glCreateProgramObjectARB();
	my_vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	my_fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	// Load Shader Sources
	glShaderSourceARB(my_vertex_shader, 1, &my_vertex_shader_source, NULL);
	glShaderSourceARB(my_fragment_shader, 1, &my_fragment_shader_source, NULL);

	// Compile The Shaders

	glCompileShaderARB(my_vertex_shader);
#ifdef _DEBUG
	if (!check_shader(my_vertex_shader))
		return 0;
#endif
	
	glCompileShaderARB(my_fragment_shader);
#ifdef _DEBUG
	if (!check_shader(my_fragment_shader))
		return 0;
#endif

	// Attach The Shader Objects To The Program Object
	glAttachObjectARB(g_ShadingProgram, my_vertex_shader);
	glAttachObjectARB(g_ShadingProgram, my_fragment_shader);

	// Link The Program Object
	glLinkProgramARB(g_ShadingProgram);

	return TRUE;
}
#endif