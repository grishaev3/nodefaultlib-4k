#pragma comment(linker, "/MERGE:.data=.text")
#pragma comment(linker, "/MERGE:.rdata=.text")
#pragma comment(linker, "/SECTION:.text,EWR")

#define IS_MOVE
#define IS_SHADING

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#ifdef _DEBUG
#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "tests.h"
#else
#endif // DEBUG

#include <windows.h>
#include <gl/GL.h>
#include <stdint.h>
#include <stdbool.h>

#define MATH_3D_IMPLEMENTATION
#include "math_3d.h"
#include "lmath.h"
#include "geom.h"
#include "lext.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
	int _fltused = 0x9875;
#ifdef __cplusplus
}
#endif  /* __cplusplus */


extern GLuint g_framebuffer;
extern GLenum g_shadingProgram;

/* C Coding Standard https://users.ece.cmu.edu/~eno/coding/CCodingStandard.html */
HWND  g_hWnd = NULL;
HDC   g_hDC = NULL;
HGLRC g_hRC = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

uint32_t g_frames_per_second = 60;
uint32_t g_maxX = 1024 / 3, g_maxY = 640 / 3;

const float g_speed = 1;
float g_deltaTime;
float g_cameraAngle;

vec3_t g_from, g_to, g_up;
mat4_t g_view, g_projection, g_prj_view, g_world_view;

float g_radius = 2.6f;
float g_radiusCoeff = 0.05f;
uint32_t g_segCount = 7;
uint32_t g_maxBlockCount = 18;
uint32_t g_maxWindingCount = 8;
geometry_ring *g_rings;

bool on_init()
{
	lsrand(GetTickCount());
	g_from = vec3(0.0, 0.0, 1.0f);
	g_up = vec3(0.0f, 1.0f, 0.0f);

	if (!init_gl_ext())
		return false;

	gl_framebuffer();

#ifdef IS_SHADING
	l_calculate_normals();
	if (!load_shaders())
		return false;
#endif // IS_SHADING

	l_create_vertex_array();

	//g_geometry_info.numberOfBlock = 3;	// кол-во блоков в окружности
	//g_geometry_info.windingCount = 1;		// кол-во слоёв (мотков)
	//g_geometry_info.segCount = 10;		// длинна тунеля

	// memory usage
	size_t g_ringsSizeInBytes = g_segCount * sizeof(geometry_ring);
	size_t geomSizeInBytes = (g_maxBlockCount * g_maxWindingCount * sizeof(mat4_t)) * g_segCount;

	/* TODO: dynamic array https://habrahabr.ru/post/324210/ */
	l_heap_create(geomSizeInBytes, geomSizeInBytes * 2);
	g_rings = (geometry_ring*)l_malloc(g_ringsSizeInBytes);

	for (uint32_t i = 0; i < g_segCount; i++) {
		geometry_ring *pRing = &g_rings[i];
		pRing->blockCount = g_maxBlockCount;
		pRing->windingCount = g_maxWindingCount;
		alloc_ring(pRing);
		generate_ring_winding((float)i, g_radius, g_radiusCoeff, pRing);
	}

#ifdef IS_SHADING
	glPolygonMode(GL_FRONT, GL_FILL);
#else
	glPolygonMode(GL_FRONT, GL_LINE);
#endif// IS_SHADING

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	/* 34..bytes
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	*/

	return true;
}

void on_resize(int maxX, int maxY)
{
	g_maxX = maxX;
	g_maxY = maxY;
	glViewport(0, 0, maxX, maxY);
	g_projection = m4_perspective(90.0f, (float)maxX
		/ maxY, 0.2f, 20.0f);

	/* FrameBuffer gen */
	if (g_isExtensionInit) gl_framebuffer();
}

void on_render(uint32_t frameTime)
{
	//float startFrame = glutGet(GLUT_ELAPSED_TIME);
	float startFrame = 0;

	// camera fly calculate
	g_cameraAngle += g_deltaTime * 10; // ~0.15
	float s, c, radius = 0.5f;
	lsincosf(g_cameraAngle * Deg2Rad, &s, &c);
	g_to.x = c*radius;	g_to.y = s*radius;
	g_view = m4_look_at(g_from, g_to, g_up);


#ifdef IS_MOVE	// move camera
	float deltaCameraZ = 0.05f;
	for (uint32_t i = 0; i < g_segCount; i++) {
		geometry_ring *pRing = &g_rings[i];
		if (move_ring(deltaCameraZ, pRing) > 1.2f) {

			free_ring(pRing);
			pRing->blockCount = g_maxBlockCount;
			pRing->windingCount = g_maxWindingCount;
			alloc_ring(pRing);

			generate_ring_winding((float)g_segCount, g_radius, g_radiusCoeff, pRing);
		}
	}
#endif // IS_MOVE

	// 1. Draw scene as normal in multisampled buffers
	glBindFramebuffer(GL_FRAMEBUFFER, g_framebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* main draw */
#ifdef IS_SHADING

	glUseProgram(g_shadingProgram);
	lUniformMatrix4fv("u_view", &g_view.m);
	lUniformMatrix4fv("u_proj", &g_projection.m);
	float lightPos[4] = { 0, 1, -1, 0 };
	lUniform4fv("u_LightPos", lightPos);

#endif // IS_SHADING
	for (uint32_t i = 0; i < g_segCount; i++)
		draw_ring(&g_rings[i]);


	// 2. Now blit multisampled buffer(s) to default framebuffers
	glBindFramebuffer(GL_READ_FRAMEBUFFER, g_framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, g_maxX, g_maxY, 0, 0,
		g_maxX, g_maxY, GL_COLOR_BUFFER_BIT, GL_NEAREST);

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
	int pixelFormat;

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;

	g_hDC = GetDC(g_hWnd);
	pixelFormat = ChoosePixelFormat(g_hDC, &pfd);
	SetPixelFormat(g_hDC, pixelFormat, &pfd);
	g_hRC = wglCreateContext(g_hDC);
	wglMakeCurrent(g_hDC, g_hRC);
}

int on_exit(void)
{
	l_heap_destroy();

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(g_hRC);
	ReleaseDC(g_hWnd, g_hDC);
	g_hDC = NULL;
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
		0, 0, g_maxX, g_maxY, 0, 0, hInstance, NULL);

	if (g_hWnd == NULL) return E_FAIL;

	init_opengl();

	if (!on_init()) return on_exit();

	int32_t sleep_time = 0;
	int32_t next_game_tick = GetTickCount();
	const uint32_t SKIP_TICKS = (1000 / g_frames_per_second);

	while (1) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
			break;

		on_render(SKIP_TICKS);

		next_game_tick += SKIP_TICKS;
		sleep_time = next_game_tick - GetTickCount();
		if (sleep_time >= 0) {
			Sleep(sleep_time);
		}
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