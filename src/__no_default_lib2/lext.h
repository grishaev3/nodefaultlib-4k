#pragma once

#include <windows.h>
#include <Shlwapi.h>
#include <GL/gl.h>
#include <gl/glext.h>
#include <gl/wglext.h>
#include "io.h"


#ifdef _DEBUG
#define NUMFUNCIONES 17
#else
#define NUMFUNCIONES 13
#endif

extern void *myglfunc[NUMFUNCIONES];

#define oglCreateProgram		((PFNGLCREATEPROGRAMPROC)glf[0])
#define oglCreateShader			((PFNGLCREATESHADERPROC)glf[1])
#define oglShaderSource			((PFNGLSHADERSOURCEPROC)glf[2])
#define oglCompileShader		((PFNGLCOMPILESHADERPROC)glf[3])
#define oglAttachShader			((PFNGLATTACHSHADERPROC)glf[4])
#define oglLinkProgram			((PFNGLLINKPROGRAMPROC)glf[5])
#define oglUseProgram			((PFNGLUSEPROGRAMPROC)glf[6])
#define oglUniform4fv			((PFNGLUNIFORM4FVPROC)glf[7])
#define oglUniform1i			((PFNGLUNIFORM1IPROC)glf[8])
#define oglGetUniformLocation	((PFNGLGETUNIFORMLOCATIONARBPROC)glf[9])

#define glGenBuffers			((PFNGLGETUNIFORMLOCATIONARBPROC)glf[10])
#define glBindBuffer			((PFNGLGETUNIFORMLOCATIONARBPROC)glf[11])
#define glBufferData			((PFNGLGETUNIFORMLOCATIONARBPROC)glf[12])


#ifdef _DEBUG
#define oglGetObjectParameteriv	((PFNGLGETOBJECTPARAMETERIVARBPROC)glf[13])
#define oglGetInfoLog			((PFNGLGETINFOLOGARBPROC)glf[14])
#define glGetShaderiv			((PFNGLGETSHADERIVPROC)glf[15])
#define glGetShaderInfoLog		((PFNGLGETSHADERINFOLOGPROC)glf[16])
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
#ifdef _DEBUG
	"glGetObjectParameterivARB",
	"glGetInfoLogARB",
	"glGetShaderiv",
	"glGetShaderInfoLog"
#endif
};

#define NUMFUNCIONES	13
void *glf[NUMFUNCIONES];

extern HWND  g_hWnd;
GLenum g_ShadingProgram;

#ifdef _DEBUG
int gl_is_extension_supported(const char * pName)
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

void init_gl_ext()
{
	for (int i = 0; i < NUMFUNCIONES; i++) {
		glf[i] = wglGetProcAddress(strs[i]);
		if (!glf[i])
			return 0;
	}
	return 1;
}

BOOL wgl_is_extension_supported(const char *extension)
{
	const size_t extlen = strlen(extension);
	const char *supported = NULL;
	GLenum err;

	// Try To Use wglGetExtensionStringARB On Current DC, If Possible
	PROC wglGetExtString = wglGetProcAddress("wglGetExtensionStringARB");
	err = glGetError();

	if (wglGetExtString)
	{
		HDC wgl_get_current_dc = wglGetCurrentDC();
		supported = ((char*(__stdcall*)(HDC))wglGetExtString)(wgl_get_current_dc);
	}

	// If That Failed, Try Standard Opengl Extensions String
	if (supported == NULL) {
		supported = (const char*)glGetString(GL_EXTENSIONS);
		err = glGetError();
	}

	// If That Failed Too, Must Be No Extensions Supported
	if (supported == NULL)
		return FALSE;

	// Begin Examination At Start Of String, Increment By 1 On False Match
	for (const char* p = supported; ; p++)
	{
		// Advance p Up To The Next Possible Match
		p = StrStrA(p, extension);

		if (p == NULL)
			return FALSE;                       // No Match

												// Make Sure That Match Is At The Start Of The String Or That
												// The Previous Char Is A Space, Or Else We Could Accidentally
												// Match "wglFunkywglExtension" With "wglExtension"

												// Also, Make Sure That The Following Character Is Space Or NULL
												// Or Else "wglExtensionTwo" Might Match "wglExtension"
		if ((p == supported || p[-1] == ' ') && (p[extlen] == '\0' || p[extlen] == ' '))
			return TRUE;                        // Match
	}
}

int get_pixel_format(HDC hDC)
{
	if (!gl_is_extension_supported("GL_ARB_multisample") &&
		!wgl_is_extension_supported("GL_ARB_multisample")) {
		return FALSE;
	}

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB =
		(PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

	if (!wglChoosePixelFormatARB) return FALSE;

	int pixelFormat;
	BOOL valid;
	UINT numFormats;
	float fAttributes[] = { 0,0 };

	// These Attributes Are The Bits We Want To Test For In Our Sample
	// Everything Is Pretty Standard, The Only One We Want To
	// Really Focus On Is The SAMPLE BUFFERS ARB And WGL SAMPLES
	// These Two Are Going To Do The Main Testing For Whether Or Not
	// We Support Multisampling On This Hardware
	int iAttributes[] = { WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
		WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB,24,
		WGL_ALPHA_BITS_ARB,8,
		WGL_DEPTH_BITS_ARB,16,
		WGL_STENCIL_BITS_ARB,0,
		WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
		WGL_SAMPLE_BUFFERS_ARB,GL_TRUE,
		WGL_SAMPLES_ARB, 4 ,                        // Check For 4x Multisampling
		0,0 };

	// First We Check To See If We Can Get A Pixel Format For 4 Samples
	valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);

	// if We Returned True, And Our Format Count Is Greater Than 1
	if (valid && numFormats >= 1)
	{
		return pixelFormat;
	}

	// Our Pixel Format With 4 Samples Failed, Test For 2 Samples
	iAttributes[19] = 2;
	valid = wglChoosePixelFormatARB(hDC, iAttributes, fAttributes, 1, &pixelFormat, &numFormats);
	if (valid && numFormats >= 1)
	{
		return pixelFormat;
	}

	return  0;
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