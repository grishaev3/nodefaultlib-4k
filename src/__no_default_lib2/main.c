#pragma comment(linker, "/MERGE:.data=.text")
#pragma comment(linker, "/MERGE:.rdata=.text")
#pragma comment(linker, "/SECTION:.text,EWR")

#ifdef _DEBUG
#include <stdio.h>
#endif // DEBUG

#include <windows.h>
#include <gl\glut.h>

#define MATH_3D_IMPLEMENTATION
#include "math_3d.h"
#include "lmath.h"
#include "geom.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
#include "lshader.h"
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

/* C Coding Standard https://users.ece.cmu.edu/~eno/coding/CCodingStandard.html */

vec3_t g_from, g_to, g_up;
mat4_t g_transform, g_view, g_projection, g_prj_view;

float g_deltaTime;
float g_cameraAngle;

const float g_speed = 5;
geometry_info g_geometry_info;

void init()
{
	g_from = vec3(0.0, 0.0, 1.0f);
	g_up = vec3(0.0f, 1.0f, 0.0f);

	lsrand(GetTickCount());

	g_geometry_info.numberOfBlock = 10;
	g_geometry_info.windingCount = 5;
	g_geometry_info.segCount = 50;
	fill_jitArray(&g_geometry_info);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glPolygonMode(GL_FRONT, GL_LINE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);


	glColor4f(0, 1, 0, 0.35f);
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void display()
{
	float startFrame = glutGet(GLUT_ELAPSED_TIME);

	// camera fly calculate
	g_cameraAngle += g_deltaTime * g_speed; // ~0.15
	float s, c, radius = 0.35f;
	lsincosf(g_cameraAngle * Deg2Rad, &s, &c);
	g_to.x = c*radius;	g_to.y = s*radius;
	g_view = m4_look_at(g_from, g_to, g_up);
	g_prj_view = m4_mul(g_projection, g_view);

	// move camera
#ifdef IS_MOVE
	float deltaCameraZ = g_deltaTime*1/g_speed;
	g_from.z -= deltaCameraZ;
	g_to.z -= deltaCameraZ;
#endif // IS_MOVE

	// setup view
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(&g_prj_view.m);	// current

	/* main draw */
	draw_geom(&g_geometry_info);

	glutSwapBuffers();

	g_deltaTime = (float)(glutGet(GLUT_ELAPSED_TIME) - startFrame) / 1000.0f;
}

void resize(int maxX, int maxY)
{
	glViewport(0, 0, maxX, maxY);
	g_projection = m4_perspective(90.0f, (float)maxX / maxY, 0.2f, 100.0f);
}

int main(int* argc, char **argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 600);
	glutCreateWindow("__4K_demo");

	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutIdleFunc(display);

	init();

	glutMainLoop();
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

void _cdecl mainCRTStartup()
{
	args args;
	get_comand_line(&args);
	main(&args.argc, &args.argv);
}