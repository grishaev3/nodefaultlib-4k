#pragma once

#include <GL/gl.h>
#include <gl/glext.h>
#include "io.h"


#ifdef DEBUG
#define NUMFUNCIONES 15
#else
#define NUMFUNCIONES 13
#endif

extern void *myglfunc[NUMFUNCIONES];

#define oglCreateProgram	            ((PFNGLCREATEPROGRAMPROC)glf[0])
#define oglCreateShader		            ((PFNGLCREATESHADERPROC)glf[1])
#define oglShaderSource                 ((PFNGLSHADERSOURCEPROC)glf[2])
#define oglCompileShader                ((PFNGLCOMPILESHADERPROC)glf[3])
#define oglAttachShader                 ((PFNGLATTACHSHADERPROC)glf[4])
#define oglLinkProgram                  ((PFNGLLINKPROGRAMPROC)glf[5])
#define oglUseProgram                   ((PFNGLUSEPROGRAMPROC)glf[6])
#define oglUniform4fv                   ((PFNGLUNIFORM4FVPROC)glf[7])
#define oglUniform1i                    ((PFNGLUNIFORM1IPROC)glf[8])
#define oglGetUniformLocation           ((PFNGLGETUNIFORMLOCATIONARBPROC)glf[9])

#define glGenBuffers           ((PFNGLGETUNIFORMLOCATIONARBPROC)glf[10])
#define glBindBuffer           ((PFNGLGETUNIFORMLOCATIONARBPROC)glf[11])
#define glBufferData           ((PFNGLGETUNIFORMLOCATIONARBPROC)glf[12])


#ifdef DEBUG
#define oglGetObjectParameteriv         ((PFNGLGETOBJECTPARAMETERIVARBPROC)myglfunc[13])
#define oglGetInfoLog                   ((PFNGLGETINFOLOGARBPROC)myglfunc[14])
#endif


static char *strs[] = {
	"glCreateProgram",
	"glCreateShader",
	"glShaderSource",
	"glCompileShader",
	"glAttachShader",
	"glLinkProgram",
	"glUseProgram",
	"glUniform4fv",
	"glUniform1i",
	"glGetUniformLocation",

	"glGenBuffers",
	"glBindBuffer",
	"glBufferData",
	//--
#ifdef DEBUG
	"glGetObjectParameterivARB",
	"glGetInfoLogARB",
#endif
};

#define NUMFUNCIONES	13
void *glf[NUMFUNCIONES];

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
	for (int i = 0; i < NUMFUNCIONES; i++) {
		glf[i] = wglGetProcAddress(strs[i]);
		if (!glf[i])
			return 0;
	}
	return 1;
}

#ifdef IS_SHADING
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