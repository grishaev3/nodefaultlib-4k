//#pragma comment(linker, "/MERGE:.data=.text")
//#pragma comment(linker, "/MERGE:.rdata=.text")
//#pragma comment(linker, "/SECTION:.text,EWR")

#define IS_MOVE
//#define IS_AA
//#define IS_SHOW_SIZE_CAPTION	//+~250b
//#define IS_DYNAMIC_NORMALS
#define IS_CAMERA_VIEW_CHANGE

// TODO:
#define Q_LOW
#define Q_HI

#define LIT_SHD	0
#define WRF_SHD	1

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
#include "s_geom.h"
#include "lext.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
#include "wire_vhlsl.h"
#include "wire_fhlsl.h"
	int _fltused = 0x9875;
#ifdef __cplusplus
}
#endif  /* __cplusplus */

extern GLenum g_shadingProgram[2];
extern GLuint g_framebuffer;

/* C Coding Standard https://users.ece.cmu.edu/~eno/coding/CCodingStandard.html */
HWND  g_hWnd = NULL;
HDC   g_hDC = NULL;
HGLRC g_hRC = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

uint32_t g_maxX = 1024 / 3, g_maxY = 640 / 3;

vec3_t g_from, g_to, g_up;
mat4_t g_view, g_projection, g_prj_view, g_world_view;

const uint32_t g_frames_per_second = 40;
const float g_scene_duration_in_sec = 1.5f;
const float g_camera_chg_view_in_sec = 1.5f;

const float g_radius = 2.6f;
const float g_radiusCoeff = 0.04f;
const uint32_t g_segCount = 10;
const uint32_t g_sqSegCount = 16;
const uint32_t g_maxBlockCount = 16;
const uint32_t g_maxWindingCount = 6;

geometry_ring *g_rings;
geometry_square *g_squares;

bool on_init()
{
	l_srand(GetTickCount());
	// TODO: -(right) +(left)
	g_up = vec3(0.0f, 1.0f, 0.0f);

	if (!init_gl_ext())
		return false;

#ifdef IS_AA
	gl_framebuffer();
#endif

#ifdef IS_DYNAMIC_NORMALS
	l_calculate_normals();
#endif
	l_create_vertex_array();

#ifdef _DEBUG
	char *vs_source0 = (char*)l_malloc(1024);
	char *fs_source0 = (char*)l_malloc(1024);
	char *vs_source1 = (char*)l_malloc(1024);
	char *fs_source1 = (char*)l_malloc(1024);

	if (load_shaders("\\shd\\v.hlsl", "\\shd\\f.hlsl", vs_source0, fs_source0))
		create_shaders(vs_source0, fs_source0, LIT_SHD);
	if (load_shaders("\\shd\\wire_v.hlsl", "\\shd\\wire_f.hlsl", vs_source1, fs_source1))
		create_shaders(vs_source1, fs_source1, WRF_SHD);

	int isFree = 0;
	isFree = l_free(vs_source0);
	isFree = l_free(fs_source0);
	isFree = l_free(vs_source1);
	isFree = l_free(fs_source1);
#else
	/* GLSL type checker and minifier http://www.ctrl-alt-test.fr/?p=287 */
	create_shaders(v_hlsl, f_hlsl, LIT_SHD);
	create_shaders(wire_v_hlsl, wire_f_hlsl, WRF_SHD);
#endif

	/* TODO: dynamic array https://habrahabr.ru/post/324210/ */
	size_t ringsSizeInBytes = g_segCount * sizeof(geometry_ring);
	g_rings = (geometry_ring*)l_malloc(ringsSizeInBytes);
	size_t squareSizeInButes = g_segCount * sizeof(geometry_square);
	g_squares = (geometry_square*)l_malloc(squareSizeInButes);

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

#ifdef IS_AA
	glEnable(GL_MULTISAMPLE);
#endif

	return true;
}

void on_resize(int maxX, int maxY)
{
	g_maxX = maxX;
	g_maxY = maxY;
	glViewport(0.0f, 0.0f, maxX, maxY);
	g_projection = m4_perspective(90.0f, (float)maxX
		/ maxY, 0.2f, 20.0f);

#ifdef IS_AA
	/* FrameBuffer gen */
	if (g_isExtensionInit) gl_framebuffer();
#endif
}

void on_set_camera(int32_t frameTime)
{
	const int numOfParams = 6;
	static int32_t cameraIndex = 0, cameraTimer = 0;

	// ~108b
#ifdef	IS_CAMERA_VIEW_CHANGE
	const int numOfViews = 16;
	static float g_cvp[] = {
		-0.0f, -1.25f, -0.0f, -0.0f, -1.25f, -1.0f,
		-0.0f, -1.25f, -0.0f, -0.0f, -1.25f, -1.0f,

		-0.0f, +4.2f, -0.0f, -0.0f, +4.2f, -1.0f,
		-0.0f, -4.2f, -0.0f, -0.0f, -4.2f, +1.0f,

		-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -3.0f,
		-0.0f, -0.0f, -7.0f, -0.0f, -0.0f, +1.0f,

		-0.0f, +1.0f, -0.0f, -0.0f, -1.0f, -3.0f,
		-0.0f, -1.55f, -0.0f, -0.0f, +1.0f, -1.25f,

		-0.1f, -0.0f, -0.0f, -1.0f, -0.0f, -1.0f,
		+0.1f, -0.0f, -0.0f, +1.0f, -0.0f, +1.0f,

		+4.2f, -0.0f, -0.0f, +0.0f, -0.0f, -1.0f,
		-4.2f, -0.0f, -0.0f, +0.0f, -0.0f, -1.0f,

		-4.2f, -0.0f, -0.0f, -4.2f, -0.0f, -1.0f,
		+4.2f, -0.0f, -0.0f, +4.2f, -0.0f, -1.0f,

		-0.0f, -4.2f, -0.0f, +0.0f, -0.0f, -1.0f,
		-0.0f, +4.2f, -0.0f, +0.0f, -0.0f, -1.0f,
	};

	if ((cameraTimer += frameTime) > g_camera_chg_view_in_sec * 1000) {
		cameraIndex = l_randi(0, numOfViews);
		cameraTimer = 0;
	}
#else
	const int numOfViews = 1;
	static float g_cvp[] = {
		-0.0f, +0.0f, -0.0f, -0.0f, -0.0f, -4.5f
	};
#endif
	//g_cameraAngle += g_deltaTime * 10; // ~0.15
	//float s, c, radius = 0.5f;
	//l_sincosf(g_cameraAngle * Deg2Rad, &s, &c);
	//g_to.x = c*radius;	g_to.y = s*radius;

	g_from = vec3fv(&g_cvp[cameraIndex * numOfParams]);
	g_to = vec3fv(&g_cvp[cameraIndex * numOfParams] + numOfParams / 2);
	g_view = m4_look_at(g_from, g_to, g_up);
}

void on_render()
{
	static float currPosZ = 20.0f; // TODO: ??
	const float limitDestrioyZ = g_maxBlockCount / 3; // TODO: ??
	const float deltaCameraZ = g_segCount / (float)(g_frames_per_second * g_scene_duration_in_sec);

	for (uint32_t i = 0; i < g_segCount; i++) {

		geometry_ring *pRing = &g_rings[i];
		geometry_square *pSquare = &g_squares[i];

		if (currPosZ > limitDestrioyZ) {

			free_ring(pRing);
			pRing->blockCount = g_maxBlockCount;
			pRing->windingCount = g_maxWindingCount;
			alloc_ring(pRing);
			generate_ring_winding(-(float)i, g_radius, g_radiusCoeff, pRing);

			free_square(pSquare);
			pSquare->blockCount = g_sqSegCount;
			alloc_square(pSquare);
			generate_square(-(float)i, g_radius * l_randf(1.1375f, 1.275f), pSquare);
		}
	}

#ifdef IS_MOVE	// move camera
	for (size_t i = 0; i < g_segCount; i++) {
		currPosZ = move_ring(deltaCameraZ, &g_rings[i]);
		currPosZ = move_square(deltaCameraZ, &g_squares[i]);
	}
#else
	currPosZ = -1.0f;
#endif

#ifdef IS_AA
	// 1. Draw scene as normal in multisampled buffers
	glBindFramebuffer(GL_FRAMEBUFFER, g_framebuffer);
#endif // IS_AA

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// ~34b
	glPolygonMode(GL_FRONT, GL_FILL);
	glUseProgram(g_shadingProgram[LIT_SHD]);
	l_UniformMatrix4fv("u_view", &g_view.m, LIT_SHD);
	l_UniformMatrix4fv("u_proj", &g_projection.m, LIT_SHD);
	float lightPos[4] = { 0.0f, 1, -1, 0 };
	l_Uniform4fv("u_LightPos", lightPos, LIT_SHD);
	for (uint32_t i = 0; i < g_segCount; i++) {
		draw_ring(&g_rings[i], LIT_SHD);
		draw_square(&g_squares[i], LIT_SHD);
	}

	glPolygonMode(GL_FRONT, GL_LINE);
	glUseProgram(g_shadingProgram[WRF_SHD]);
	l_UniformMatrix4fv("u_view", &g_view.m, WRF_SHD);
	l_UniformMatrix4fv("u_proj", &g_projection.m, WRF_SHD);
	for (uint32_t i = 0; i < g_segCount; i++) {
		draw_ring(&g_rings[i], WRF_SHD);
		draw_square(&g_squares[i], WRF_SHD);
	}

#ifdef IS_AA
	// 2. Now blit multisampled buffer(s) to default framebuffers
	glBindFramebuffer(GL_READ_FRAMEBUFFER, g_framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0.0f, 0.0f, g_maxX, g_maxY, 0.0f, 0.0f,
		g_maxX, g_maxY, GL_COLOR_BUFFER_BIT, GL_NEAREST);
#endif // IS_AA

	/*glBegin(GL_TRIANGLES);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();*/

	SwapBuffers(g_hDC);
}

void init_opengl(void)
{
	int pixelFormat;

	PIXELFORMATDESCRIPTOR pfd = { 0 };
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

	return S_OK;
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
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	if (!RegisterClass(&wc))
		return 1;

	char szCaption[16] = { 0 };
#ifdef IS_SHOW_SIZE_CAPTION
	szCaption[0] = '_'; szCaption[1] = '_';
	l_itoa(get_exe_size(), &szCaption[2], 10);
	strcat(szCaption + strlen(szCaption), "b");
#endif // IS_SHOW_SIZE_CAPTION
	g_hWnd = CreateWindowA(wc.lpszClassName,
		szCaption,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0, g_maxX, g_maxY, 0, 0, hInstance, NULL);


	if (g_hWnd == NULL) return E_FAIL;

	init_opengl();

	if (!on_init()) return on_exit();

	const int32_t max_frame_duration_in_msec = 
		(1000 / g_frames_per_second);

	while (1) {
		while (PeekMessage(&msg, NULL, 0.0f, 0.0f, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
			break;

		int32_t start_frame = GetTickCount();
		on_render();
		int32_t frame_time = GetTickCount() - start_frame;
		on_set_camera(frame_time);
		int32_t diff = max_frame_duration_in_msec - frame_time;

		if (diff > 0) 
			Sleep(diff);
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