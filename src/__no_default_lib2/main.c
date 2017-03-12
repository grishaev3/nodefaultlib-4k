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
#include "tests.h"
#include "lshader.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
	int _fltused = 0x9875;
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#define IS_MOVE
//#define IS_SHADING

/* C Coding Standard https://users.ece.cmu.edu/~eno/coding/CCodingStandard.html */
HWND  g_hWnd = NULL;
HDC   g_hDC = NULL;
HGLRC g_hRC = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

vec3_t g_from, g_to, g_up;
mat4_t g_view, g_projection, g_prj_view;

float g_deltaTime;
float g_cameraAngle;

const float g_speed = 1;
size_t g_segCount = 1;
geometry_ring* g_rings;

extern GLenum g_ShadingProgram;

void on_init()
{
	g_from = vec3(0.0, 0.0, 1.0f);
	g_up = vec3(0.0f, 1.0f, 0.0f);

	lsrand(GetTickCount());
	init_gl_arb();

#ifdef IS_SHADING
	if (!load_shaders()) return;
#endif // IS_SHADING

	//g_geometry_info.numberOfBlock = 3;	// кол-во блоков в окружности
	//g_geometry_info.windingCount = 1;		// кол-во слоёв (мотков)
	//g_geometry_info.segCount = 10;		// длинна тунеля
	//fill_jitArray(&g_geometry_info);

	size_t g_ringsSizeInBytes = g_segCount * sizeof(geometry_ring);
	g_rings = (geometry_ring*)malloc(g_ringsSizeInBytes);
	ZeroMemory(g_rings, g_ringsSizeInBytes);

	size_t  structOffset = g_rings;
	for (int i = 0; i < g_segCount; i++, structOffset += sizeof(geometry_ring)) {
		geometry_ring* pRing = (geometry_ring*)(&g_rings[i]);
		pRing->numberOfBlock = 12;
		pRing->windingCount = 1;
		alloc_ring(pRing);

		float currentRadius = 1.0f;
		for (int windingIndex = 0; windingIndex < pRing->windingCount; windingIndex++) {
			float jit = lrandf(-10.0f*windingIndex, 10.0f*windingIndex);
			generate_ring(-(float)i, currentRadius, jit, windingIndex, pRing);
			currentRadius += 0.1f;
		}
	}

	//for (int depth = 0; depth < info->segCount; depth += info->windingCount) {
	//	float z = -(float)depth / info->windingCount;
	//	float currentRadius = 1.0f;
	//	for (int w = 0; w < info->windingCount; w++) {
	//		draw_ring(z, currentRadius, info->jitArray[depth + w], info);
	//		currentRadius += 0.1f;
	//	}
	//}
	//


	glPolygonMode(GL_FRONT, GL_LINE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glColor4f(0.0f, 1.0f, 0.0f, 0.35f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void on_resize(int maxX, int maxY)
{
	glViewport(0, 0, maxX, maxY);
	g_projection = m4_perspective(90.0f, (float)maxX / maxY, 0.2f, 100.0f);
}

void on_render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//float startFrame = glutGet(GLUT_ELAPSED_TIME);
	float startFrame = 0;

	// camera fly calculate
	g_cameraAngle += g_deltaTime * g_speed; // ~0.15
	float s, c, radius = 0.15f;
	lsincosf(g_cameraAngle * Deg2Rad, &s, &c);
	g_to.x = c*radius;	g_to.y = s*radius;
	g_view = m4_look_at(g_from, g_to, g_up);
	g_prj_view = m4_mul(g_projection, g_view);

	// move camera
#ifdef IS_MOVE
	/*
	float deltaCameraZ = 0.0001f;
	for (int i = 0; i < g_segCount; i++) {
		geometry_ring *pRing = &rings[i];
		if (move_ring(deltaCameraZ, pRing) > 1.0f) {
			//free_ring(pRing);
			//alloc_ring(pRing);
			//for (int j = 0; j < pRing->windingCount; j++) {
			//	generate_ring(-g_segCount, 1.0f, lrandf(10.0f*j, 10.0f*j), pRing);
			//}
		}
	}
	*/
#endif // IS_MOVE

	// setup view
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf((const GLfloat*)&g_prj_view.m);	// current

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* main draw */
#ifdef IS_SHADING
	glUseProgramObjectARB(g_ShadingProgram);
#endif // IS_SHADING
	for (int i = 0; i < g_segCount; i++)
		draw_ring(&g_rings[i]);

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

#ifdef _DEBUG
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
#else
int WINAPI WinMainCRTStartup(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif // DEBUG
{
	MSG msg;
	WNDCLASS wc;
	ZeroMemory(&msg, sizeof(MSG));
	ZeroMemory(&wc, sizeof(WNDCLASS));
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = L"minwindowsapp";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	if (!RegisterClass(&wc))
		return 1;

	g_hWnd = CreateWindow(wc.lpszClassName,
		L"__4Kb",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, 720, 480, 0, 0, hInstance, NULL);

	if (g_hWnd == NULL) return E_FAIL;

	init_opengl();

	on_init();

	while (TRUE) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
			break;
		on_render();
	}

	return on_exit();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_DESTROY:
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