#pragma once

#include <windows.h>
#include <GL/gl.h>
#include <gl/glext.h>
#include <gl/wglext.h>

#include <stdbool.h>

#include "vhlsl.h"
#include "fhlsl.h"

#ifdef _DEBUG
#define NUMFUNCIONES 29
#else
#define NUMFUNCIONES 25
#endif
#include "io.h"

void *glf[NUMFUNCIONES];

#define glCreateProgram						((PFNGLCREATEPROGRAMPROC)glf[0])
#define glCreateShader						((PFNGLCREATESHADERPROC)glf[1])
#define glShaderSource						((PFNGLSHADERSOURCEPROC)glf[2])
#define glCompileShader						((PFNGLCOMPILESHADERPROC)glf[3])
#define glAttachShader						((PFNGLATTACHSHADERPROC)glf[4])
#define glLinkProgram						((PFNGLLINKPROGRAMPROC)glf[5])
#define glUseProgram						((PFNGLUSEPROGRAMPROC)glf[6])
#define glUniform4fv						((PFNGLUNIFORM4FVPROC)glf[7])
#define glUniformMatrix4fv					((PFNGLUNIFORMMATRIX4FVPROC)glf[8])
#define glGetUniformLocation				((PFNGLGETUNIFORMLOCATIONARBPROC)glf[9])

#define glGenBuffers						((PFNGLGENBUFFERSPROC)glf[10])
#define glBindBuffer						((PFNGLBINDBUFFERPROC)glf[11])
#define glBufferData						((PFNGLBUFFERDATAPROC)glf[12])

#define glGenFramebuffers					((PFNGLGENFRAMEBUFFERSPROC)glf[13])
#define glBindFramebuffer					((PFNGLBINDFRAMEBUFFERPROC)glf[14])
#define glFramebufferTexture2D				((PFNGLFRAMEBUFFERTEXTURE2DPROC)glf[15])
#define glRenderbufferStorageMultisample	((PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)glf[16])
#define glFramebufferRenderbuffer			((PFNGLFRAMEBUFFERRENDERBUFFERPROC)glf[17])
#define glCheckFramebufferStatus			((PFNGLCHECKFRAMEBUFFERSTATUSPROC)glf[18])

#define glGenRenderbuffers					((PFNGLGENRENDERBUFFERSPROC)glf[19])
#define glBindRenderbuffer					((PFNGLBINDRENDERBUFFERPROC)glf[20])
#define glBlitFramebuffer					((PFNGLBLITFRAMEBUFFERPROC)glf[21])
#define glTexImage2DMultisample				((PFNGLTEXIMAGE2DMULTISAMPLEPROC)glf[22])

#define glDeleteFramebuffers				((PFNGLDELETEFRAMEBUFFERSPROC)glf[23])
#define glDeleteTextures					((PFNGLDELETETEXTURESEXTPROC)glf[24])

#ifdef _DEBUG
#define oglGetObjectParameteriv				((PFNGLGETOBJECTPARAMETERIVARBPROC)glf[25])
#define oglGetInfoLog						((PFNGLGETINFOLOGARBPROC)glf[26])
#define glGetShaderiv						((PFNGLGETSHADERIVPROC)glf[27])
#define glGetShaderInfoLog					((PFNGLGETSHADERINFOLOGPROC)glf[28])
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
	"glUniformMatrix4fv",
	"glGetUniformLocation",

	"glGenBuffers",
	"glBindBuffer",
	"glBufferData",

	"glGenFramebuffers",
	"glBindFramebuffer",
	"glFramebufferTexture2D",
	"glRenderbufferStorageMultisample",
	"glFramebufferRenderbuffer",
	"glCheckFramebufferStatus",
	"glGenRenderbuffers",
	"glBindRenderbuffer",
	"glBlitFramebuffer",
	"glTexImage2DMultisample",

	"glDeleteFramebuffers",
	"glDeleteTextures",

	//--
#ifdef _DEBUG
	"glGetObjectParameterivARB",
	"glGetInfoLogARB",
	"glGetShaderiv",
	"glGetShaderInfoLog"
#endif
};



extern HWND  g_hWnd;
extern uint32_t g_maxX;
extern uint32_t g_maxY;
GLenum g_shadingProgram[2];
GLuint g_framebuffer;
GLuint g_textureBuffer;
GLboolean g_isExtensionInit = 0;

#ifdef _DEBUG
bool gl_is_extension_supported(const char * extension)
{
	size_t extlen = strlen(extension);
	const char *supported = (const char *)glGetString(GL_EXTENSIONS);
	while (supported) {
		if (!strncmp(supported, extension, extlen) &&
			(*(supported + extlen) == 0 || *(supported + extlen) == ' '))
			return true;
		supported = strchr(supported, ' ');
		if (supported)
			supported++;
	}

	return false;
}

bool wgl_is_extension_supported(const char *extension)
{
	const size_t extlen = strlen(extension);
	const char *supported = NULL;

	PROC wglGetExtString = wglGetProcAddress("wglGetExtensionStringARB");

	if (wglGetExtString) {
		HDC wgl_get_current_dc = wglGetCurrentDC();
		supported = ((char*(__stdcall*)(HDC))wglGetExtString)(wgl_get_current_dc);
	}


	if (supported == NULL)
		return false;
	return true;
}

int get_pixel_format(HDC hDC)
{
	if (!wgl_is_extension_supported("GL_ARB_multisample")) {
		return false;
	}

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB =
		(PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");

	if (!wglChoosePixelFormatARB) return false;

	int pixelFormat;
	bool valid;
	UINT numFormats;
	float fAttributes[] = { 0,0 };

	// These Attributes Are The Bits We Want To Test For In Our Sample
	// Everything Is Pretty Standard, The Only One We Want To
	// Really Focus On Is The SAMPLE BUFFERS ARB And WGL SAMPLES
	// These Two Are Going To Do The Main Testing For Whether Or Not
	// We Support Multisampling On This Hardware
	int iAttributes[] = { WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
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

bool check_shader(GLuint shader, char * errorLog)
{
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		GLint logSize = 0;
		int maxLength = MAX_PATH;
		glGetShaderiv(shader, maxLength, &logSize);
		glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog);
	}
	return success;
}
#endif

bool init_gl_ext()
{
	for (int i = 0; i < NUMFUNCIONES; i++) {
		glf[i] = wglGetProcAddress(strs[i]);
		if (!glf[i])
			return false;
	}
	return g_isExtensionInit = true;
}


/* https://learnopengl.com/#!Advanced-OpenGL/Anti-Aliasing */
GLuint generate_multisample_texture(GLuint samples)
{
	GLuint texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB,
		g_maxX, g_maxY, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	return texture;
}

bool gl_framebuffer()
{
	if (g_framebuffer)
		glDeleteFramebuffers(1, &g_framebuffer);
	if (g_textureBuffer)
		glDeleteTextures(1, &g_textureBuffer);

	// Framebuffers
	glGenFramebuffers(1, &g_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, g_framebuffer);
	// Create a multisampled color attachment texture
	g_textureBuffer = generate_multisample_texture(4);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, g_textureBuffer, 0);
	// Create a renderbuffer object for depth and stencil attachments
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4,
		GL_DEPTH24_STENCIL8, g_maxX, g_maxY);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		//cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
		return false;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}


void lUniform4fv(const char *param, GLfloat *x, uint32_t prgIndex)
{
	GLint loc = glGetUniformLocation(prgIndex, param);
	glUniform4fv(loc, 1, x);
}
void lUniformMatrix4fv(const char *param, GLfloat *x, uint32_t prgIndex)
{
	GLint loc = glGetUniformLocation(g_shadingProgram[prgIndex], param);
	glUniformMatrix4fv(loc, 1, false, x);
}

bool create_shaders(const char *my_vertex_shader_source, const char *my_fragment_shader_source, uint32_t prgIndex)
{
	char errorLog[MAX_PATH] = { 0 };
	GLenum my_vertex_shader;
	GLenum my_fragment_shader;

	// Create Shader And Program Objects
	g_shadingProgram[prgIndex] = glCreateProgram();
	my_vertex_shader = glCreateShader(GL_VERTEX_SHADER_ARB);
	my_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER_ARB);

	// Load Shader Sources
	glShaderSource(my_vertex_shader, 1, &my_vertex_shader_source, NULL);
	glShaderSource(my_fragment_shader, 1, &my_fragment_shader_source, NULL);

	// Compile The Shaders

	glCompileShader(my_vertex_shader);
#ifdef _DEBUG
	if (!check_shader(my_vertex_shader, errorLog))
		return false;
#endif

	glCompileShader(my_fragment_shader);
#ifdef _DEBUG
	if (!check_shader(my_fragment_shader, errorLog))
		return false;
#endif

	// Attach The Shader Objects To The Program Object
	glAttachShader(g_shadingProgram[prgIndex], my_vertex_shader);
	glAttachShader(g_shadingProgram[prgIndex], my_fragment_shader);

	// Link The Program Object
	glLinkProgram(g_shadingProgram[prgIndex]);

	return true;
}
