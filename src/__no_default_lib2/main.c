#pragma comment(linker, "/MERGE:.data=.text")
#pragma comment(linker, "/MERGE:.rdata=.text")
#pragma comment(linker, "/SECTION:.text,EWR")

#ifdef _DEBUG
#include <stdio.h>
#endif // DEBUG

#include <windows.h>
#include <gl/GL.h>

#define MATH_3D_IMPLEMENTATION
#include "math_3d.h"
#include "lmath.h"
#include "geom.h"
#include "io.h"
#include "lshader.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
	int _fltused = 0x9875;
#ifdef __cplusplus
}
#endif  /* __cplusplus */


#ifdef _DEBUG
void write(char * fmtstr, ...)
{
	DWORD dwRet;
	CHAR buffer[256];
	va_list v1;
	va_start(v1, fmtstr);
	wvsprintf(buffer, fmtstr, v1);
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), buffer, lstrlen(buffer), &dwRet, 0);
	va_end(v1);
}

void test_trigonometry()
{
	float angle = 42.0f;
	float s, c;
	float deginrad = angle * Deg2Rad;
	lsincosf(deginrad, &s, &c);

	printf("sin %d: %f{%f}\n", (int)angle, s, sinf(deginrad));
	printf("cos %d: %f{%f}\n", (int)angle, c, cosf(deginrad));
	printf("tan %d: %f{%f}\n", (int)angle, ltanf(deginrad), tanf(deginrad));
	printf("sqrt %d: %f{%f}\n", (int)angle, lsqrtf(deginrad), sqrtf(deginrad));
	printf("fabs %f: %f{%f}\n", -angle * M_PI, labsf(-angle * M_PI), fabsf(-angle * M_PI));
}
#endif // DEBUG#

#define IS_MOVE
//#define IS_SHADING

/* C Coding Standard https://users.ece.cmu.edu/~eno/coding/CCodingStandard.html */

HWND  g_hWnd = NULL;
HDC   g_hDC = NULL;
HGLRC g_hRC = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

vec3_t g_from, g_to, g_up;
mat4_t g_transform, g_view, g_projection, g_prj_view;

float g_deltaTime;
float g_cameraAngle;

const float g_speed = 5;
geometry_info g_geometry_info;

extern GLenum g_ShadingProgram;

void on_resize(int maxX, int maxY)
{
	glViewport(0, 0, maxX, maxY);
	g_projection = m4_perspective(90.0f, (float)maxX / maxY, 0.2f, 100.0f);
}

typedef struct args
{
	int argc;
	char **argv;
} args;

void get_comand_line(args *args)
{
	LPWSTR *szArglist;
	int nArgs;

	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (NULL == szArglist) ExitProcess(1);

	args->argc = nArgs;
	args->argv = szArglist;
	LocalFree(szArglist);
}

//void _cdecl mainCRTStartup()
//{
//	args args;
//	get_comand_line(&args);
//	///main(&args.argc, &args.argv);
//}

void init_opengl(void)
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;

	g_hDC = GetDC(g_hWnd);
	GLuint iPixelFormat = ChoosePixelFormat(g_hDC, &pfd);

	if (iPixelFormat != 0) {
		PIXELFORMATDESCRIPTOR bestMatch_pfd;
		DescribePixelFormat(g_hDC, iPixelFormat, sizeof(pfd), &bestMatch_pfd);

		if (bestMatch_pfd.cDepthBits < pfd.cDepthBits) {
			return;
		}

		if (SetPixelFormat(g_hDC, iPixelFormat, &pfd) == FALSE) {
			DWORD dwErrorCode = GetLastError();
			return;
		}
	}
	else {
		DWORD dwErrorCode = GetLastError();
		return;
	}

	g_hRC = wglCreateContext(g_hDC);
	wglMakeCurrent(g_hDC, g_hRC);
}

void on_init()
{
	g_from = vec3(0.0, 0.0, 1.0f);
	g_up = vec3(0.0f, 1.0f, 0.0f);

	lsrand(GetTickCount());
	init_gl_arb();

#ifdef IS_SHADING
	if (!load_shaders()) return;
#endif // IS_SHADING

	g_geometry_info.numberOfBlock = 12;
	g_geometry_info.windingCount = 5;
	g_geometry_info.segCount = 50;
	fill_jitArray(&g_geometry_info);

	glPolygonMode(GL_FRONT, GL_LINE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glColor4f(0.0f, 1.0f, 0.0f, 0.35f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int on_exit(void)
{
	if (g_hRC != NULL) {
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(g_hRC);
		g_hRC = NULL;
	}

	if (g_hDC != NULL) {
		ReleaseDC(g_hWnd, g_hDC);
		g_hDC = NULL;
	}

	return 0;
}

void on_render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//float startFrame = glutGet(GLUT_ELAPSED_TIME);
	float startFrame = 0;

	// camera fly calculate
	g_cameraAngle += g_deltaTime * g_speed; // ~0.15
	float s, c, radius = 0.35f;
	lsincosf(g_cameraAngle * Deg2Rad, &s, &c);
	g_to.x = c*radius;	g_to.y = s*radius;
	g_view = m4_look_at(g_from, g_to, g_up);
	g_prj_view = m4_mul(g_projection, g_view);

	// move camera
#ifdef IS_MOVE
	float deltaCameraZ = g_deltaTime * 1 / g_speed;
	g_from.z -= deltaCameraZ;
	g_to.z -= deltaCameraZ;
#endif // IS_MOVE

	// setup view
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&g_prj_view.m);	// current

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* main draw */
#ifdef IS_SHADING
	glUseProgramObjectARB(g_ShadingProgram);
#endif // IS_SHADING
	draw_geom(&g_geometry_info);

	/*glBegin(GL_TRIANGLES);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();*/

	//glutSwapBuffers();
	//g_deltaTime = (float)(glutGet(GLUT_ELAPSED_TIME) - startFrame) / 1000.0f;
	g_deltaTime = (float)(16 - startFrame) / 1000.0f;
	SwapBuffers(g_hDC);
}

#ifdef _DEBUG
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
#else
int WINAPI WinMainCRTStartup(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif // DEBUG
{
	MSG msg = { 0 };
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = L"minwindowsapp";
	if (!RegisterClass(&wc))
		return 1;

	g_hWnd = CreateWindow(wc.lpszClassName,
		L"__4Kb",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, 720, 480, 0, 0, hInstance, NULL);

	if (g_hWnd == NULL)
		return E_FAIL;

	init_opengl();

	on_init();

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
			on_render();
	}

	return on_exit();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CLOSE:
		on_exit();
		PostQuitMessage(0);
		ExitProcess(0);
		break;
	case WM_SIZE:
		on_resize(LOWORD(lParam), HIWORD(lParam));
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}