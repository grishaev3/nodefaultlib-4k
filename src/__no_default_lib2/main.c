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


vec3_t from, to, up;
mat4_t transform, view, projection, prj_view;

float deltaTime;
float cameraAngle;

const float numberOfBlock = 10;
const float speed = 5;
const int windingCount = 5;
const int segCount = 50;

float *jitArray;

void init()
{
	from = vec3(0.0, 0.0, 1.0f);
	up = vec3(0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glPolygonMode(GL_FRONT, GL_LINE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);


	glColor3f(1, 1, 1, 0.5f);
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

mat4_t get_transform(mat4_t translate, mat4_t rotate, mat4_t scale)
{
	return m4_mul(m4_mul(translate, rotate), scale);
}


void draw_ring(float z, float radius, float jit)
{
	vec3_t scale = (vec3_t) { 0.65f, 0.05f, 1.0f };

	float step = 360 / numberOfBlock;

	for (float angle = 0.0 + jit; angle < 360 - jit; angle += step)
	{
		const float angle_in_rad = angle*Deg2Rad;

		float s, c;
		lsincosf(angle_in_rad, &s, &c);
		vec3_t offset = (vec3_t) { c * radius, s * radius, z };

		transform = get_transform(
			m4_translation(offset),
			m4_rotation_z((angle + 90.0f) * Deg2Rad),
			m4_scaling(scale));

		glPushMatrix();
		glMultMatrixf(&transform.m); // current * transform
		glutSolidCube(0.5f);
		glPopMatrix();
	}
}


void display()
{
	float startFrame = glutGet(GLUT_ELAPSED_TIME);

	// camera fly calculate
	cameraAngle += deltaTime * speed; // ~0.15
	float s, c, radius = 0.35f;
	lsincosf(cameraAngle * Deg2Rad, &s, &c);
	to.x = c*radius;	to.y = s*radius;
	view = m4_look_at(from, to, up);
	prj_view = m4_mul(projection, view);

	// move camera
#ifdef IS_MOVE
	float deltaCameraZ = deltaTime*1/speed;
	from.z -= deltaCameraZ;
	to.z -= deltaCameraZ;
#endif // IS_MOVE



	// setup view
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(&prj_view.m);	// current

	for (int depth = 0; depth < segCount; depth+= windingCount) {
		float z = -(float)depth/ windingCount;
		float currentRadius = 1.0f;
		for (int w = 0; w < windingCount; w++) {
			draw_ring(z, currentRadius, jitArray[depth + w]);
			currentRadius += 0.1f;
		}
	}

	glutSwapBuffers();

	deltaTime = (float)(glutGet(GLUT_ELAPSED_TIME) - startFrame) / 1000.0f;
}

void resize(int maxX, int maxY)
{
	glViewport(0, 0, maxX, maxY);
	projection = m4_perspective(73.74f, (float)maxX / maxY, 0.2f, 100.0f);
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
	if (NULL == szArglist)
	{
		ExitProcess(1);
	}

	args->argc = nArgs;
	args->argv = szArglist;
	LocalFree(szArglist);
}

void _cdecl mainCRTStartup()
{
	srand(GetTickCount());

	jitArray = (float*)LocalAlloc(LPTR, segCount * windingCount * sizeof(float));
	for (int i = 0; i < segCount; i+= windingCount)
	{
		for (int j = 0; j < windingCount; j++) {
			jitArray[i + j] = lrandf(10.0f*j, 10.0f*j);
	
		}
	}

	args args;
	get_comand_line(&args);
	main(&args.argc, &args.argv);
}