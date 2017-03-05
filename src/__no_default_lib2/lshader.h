#pragma once

#include <GL/gl.h>
#include <gl/glext.h>

/*
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
*/

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


GLenum my_program;


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
#endif


BOOL init_gl_arb()
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

	return TRUE;
}


int check_shader(GLuint shader)
{
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		GLint logSize = 0;
		int maxLength = 256;
		char *errorLog = (char*)LocalAlloc(LPTR, maxLength);
		glGetShaderiv(shader, maxLength, &logSize);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
		LocalFree(errorLog);
	}
	return success;
}


int load_shaders()
{
	char * my_fragment_shader_source;
	char * my_vertex_shader_source;
	GLenum my_vertex_shader;
	GLenum my_fragment_shader;

	// Get Vertex And Fragment Shader Sources
	
	my_vertex_shader_source = "varying vec4 diffuse; void main() { gl_Position  =  ftransform(); vec3 normal = gl_Normal; vec3 lightVector = normalize(gl_LightSource[0].position.xyz); float nxDir = max(0.0, dot(normal, lightVector)); diffuse = gl_LightSource[0].diffuse * nxDir; }";
	my_fragment_shader_source = "varying vec4 diffuse; void main() {  gl_FragColor = (gl_LightSource[0].ambient + diffuse); }";

	// Create Shader And Program Objects
	my_program = glCreateProgramObjectARB();
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
	glAttachObjectARB(my_program, my_vertex_shader);
	glAttachObjectARB(my_program, my_fragment_shader);

	// Link The Program Object
	glLinkProgramARB(my_program);

	// Use The Program Object Instead Of Fixed Function OpenGL
	glUseProgramObjectARB(my_program);
}