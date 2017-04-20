//#pragma comment(linker, "/MERGE:.data=.text")
//#pragma comment(linker, "/MERGE:.rdata=.text")
//#pragma comment(linker, "/SECTION:.text,EWR")

//#define IS_MOVE
//#define IS_AA
//#define IS_SHOW_SIZE_CAPTION	//+~250b
//#define IS_STATIC_NORMALS
//#define IS_CAMERA_VIEW_CHANGE

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

uint32_t g_frames_per_second = 60;
uint32_t g_maxX = 1024 / 3, g_maxY = 640 / 3;

const float g_speed = 1;
float g_deltaTime;
float g_cameraAngle;

vec3_t g_from, g_to, g_up;
mat4_t g_view, g_projection, g_prj_view, g_world_view;

float g_radius = 2.6f;
float g_radiusCoeff = 0.04f;
const uint32_t g_segCount = 7;		// кол-во блоков в окружности
uint32_t g_maxBlockCount = 18;		// длинна тунеля
uint32_t g_maxWindingCount = 8;		// кол-во слоёв (мотков)
geometry_ring *g_rings;

bool on_init()
{
	l_srand(GetTickCount());
	// TODO: -(right) +(left)
	g_up = vec3(0.0f, 1.0f, 0.0f);

	if (!init_gl_ext())
		return false;

#ifdef IS_AA
	gl_framebuffer();
#endif // IS_AA

#ifdef IS_STATIC_NORMALS
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

	// memory usage
	size_t g_ringsSizeInBytes = g_segCount * sizeof(geometry_ring);
	size_t geomSizeInBytes = (g_maxBlockCount * g_maxWindingCount * sizeof(mat4_t)) * g_segCount;

	/* TODO: dynamic array https://habrahabr.ru/post/324210/ */
	g_rings = (geometry_ring*)l_malloc(g_ringsSizeInBytes);

	for (uint32_t i = 0; i < g_segCount; i++) {
		geometry_ring *pRing = &g_rings[i];
		pRing->blockCount = g_maxBlockCount;
		pRing->windingCount = g_maxWindingCount;
		alloc_ring(pRing);
		generate_ring_winding(-(float)i, g_radius, g_radiusCoeff, pRing);
	}

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

#ifdef IS_AA
	glEnable(GL_MULTISAMPLE);
#endif // IS_AA

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
#endif // IS_AA
}

void on_set_camera(uint32_t frameTime)
{
	const int numOfParams = 6;
	static uint32_t cameraIndex = 0, cameraTimer = 0;

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

	if ((cameraTimer += frameTime) > 3750) {
		cameraIndex = l_randi(0, numOfViews);
		cameraTimer = 0;
	}
#else
	const int numOfViews = 1;
	static float g_cvp[] = {
		-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -3.0f
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

void on_render(uint32_t frameTime)
{
	//float startFrame = glutGet(GLUT_ELAPSED_TIME);
	float startFrame = 0;

	// camera fly calculate
	on_set_camera(frameTime);

#ifdef IS_MOVE	// move camera
	const float deltaCameraZ = 0.05f;
	const float limitDestrioyZ = g_maxBlockCount / 3;

	for (uint32_t i = 0; i < g_segCount; i++) {
		geometry_ring *pRing = &g_rings[i];
		if (move_ring(deltaCameraZ, pRing) > limitDestrioyZ) {

			free_ring(pRing);
			pRing->blockCount = g_maxBlockCount;
			pRing->windingCount = g_maxWindingCount;
			alloc_ring(pRing);

			generate_ring_winding(-(float)g_segCount, g_radius, g_radiusCoeff, pRing);
		}
	}
#endif // IS_MOVE

#ifdef IS_AA
	// 1. Draw scene as normal in multisampled buffers
	glBindFramebuffer(GL_FRAMEBUFFER, g_framebuffer);
#endif // IS_AA

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* main draw 
	glPolygonMode(GL_FRONT, GL_LINE);
	glUseProgram(g_shadingProgram[WRF_SHD]);
	l_UniformMatrix4fv("u_view", &g_view.m, WRF_SHD);
	l_UniformMatrix4fv("u_proj", &g_projection.m, WRF_SHD);
	for (uint32_t i = 0; i < g_segCount; i++)
		draw_ring(&g_rings[i], WRF_SHD);

	// ~34b
	glPolygonMode(GL_FRONT, GL_FILL);
	glUseProgram(g_shadingProgram[LIT_SHD]);
	l_UniformMatrix4fv("u_view", &g_view.m, LIT_SHD);
	l_UniformMatrix4fv("u_proj", &g_projection.m, LIT_SHD);
	float lightPos[4] = { 0.0f, 1, -1, 0 };
	l_Uniform4fv("u_LightPos", lightPos, LIT_SHD);
	for (uint32_t i = 0; i < g_segCount; i++)
		draw_ring(&g_rings[i], LIT_SHD);
		*/


	glPolygonMode(GL_FRONT, GL_FILL);
	glUseProgram(g_shadingProgram[LIT_SHD]);
	l_UniformMatrix4fv("u_view", &g_view.m, LIT_SHD);
	l_UniformMatrix4fv("u_proj", &g_projection.m, LIT_SHD);
	float lightPos[4] = { 0.0f, 1, -1, 0 };
	l_Uniform4fv("u_LightPos", lightPos, LIT_SHD);
	mat4_t result;
	float angle = 0.0f;
	float step = 360.0f / 12;
	float radius = 2.6f;
	float s, c;
	const vec3_t scale = (vec3_t) { 0.65f, 0.125f, 1.0f };
	for (uint32_t i = 0; i < 12; i++) {
		float angle_in_rad = angle*Deg2Rad;
		l_sincosf(angle_in_rad, &s, &c);

		mat4_t translation = m4_translation((vec3_t) { c * radius, s * radius, -2.0f });
		mat4_t rotation = m4_rotation_y(-90* Deg2Rad);
		mat4_t scalem = m4_scaling(scale);
		//mat4_t rotation = m4_identity();

		get_transform(&result, &translation, &rotation, &scalem);
		l_UniformMatrix4fv("u_world", &result, LIT_SHD);
		l_draw_plane();

		angle += step;
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

	//glutSwapBuffers();
	//g_deltaTime = (float)(glutGet(GLUT_ELAPSED_TIME) - startFrame) / 1000.0f;
	g_deltaTime = (float)(16 - startFrame) / 1000.0f;
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

	int32_t sleep_time = 0;
	int32_t next_game_tick = GetTickCount();
	const uint32_t SKIP_TICKS = (1000 / g_frames_per_second);

	while (1) {
		while (PeekMessage(&msg, NULL, 0.0f, 0.0f, PM_REMOVE)) {
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