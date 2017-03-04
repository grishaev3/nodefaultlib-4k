//#pragma optimize("gsy",on)
//#pragma comment(linker,"/MERGE:.rdata=.text /MERGE:.data=.text")
//#pragma comment(linker, "/SECTION:.text,EWR")
//#pragma comment(linker,"/FILEALIGN:512")

#ifdef _DEBUG
#include <stdio.h>
#endif // DEBUG

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
#else
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
#endif // DEBUG


vec3_t from, to, up;
mat4_t transform, view, projection, prj_view;

float deltaTime = 0.0f;
float cameraAngle = 0.0f;

void init()
{
	transform = m4_identity();
	view = m4_identity();
	projection = m4_identity();

	from = vec3(0.0, 0.0, 2.0f);
	to = vec3(0.0f, 0.0f, 0.0f);
	up = vec3(0.0f, 1.0f, 0.0f);
	view = m4_look_at(from, to, up);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glPolygonMode(GL_FRONT, GL_LINE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
}

mat4_t get_transform(mat4_t translate, mat4_t rotate, mat4_t scale)
{
	return m4_mul(m4_mul(translate, rotate), scale);
}

void display()
{
	const float speed = 5;
	float startFrame = glutGet(GLUT_ELAPSED_TIME);
	
	// camera fly calculate
	cameraAngle += deltaTime * speed; // ~0.15
	float s, c, radius = 0.5f;
	lsincosf(cameraAngle * Deg2Rad, &s, &c);
	to.x = c*radius;	to.y = s*radius;
	view = m4_look_at(from, to, up);
	prj_view = m4_mul(projection, view);

	// setup view
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(&prj_view.m);	// current

	vec3_t scale = (vec3_t) { 0.4, 0.05, 1 };
	const float one_deg = M_PI / 180;
	for (float angle = 0.0; angle < 360; angle += 360 / 12)
	{
		const float angle_in_rad = angle*one_deg;

		float s, c;
		lsincosf(angle_in_rad, &s, &c);
		vec3_t offset = (vec3_t) { c * 1.0f , s * 1.0f, 0.0f};

		transform = get_transform(
			m4_translation(offset),
			m4_rotation_z((angle + 90) * one_deg),
			m4_scaling(scale));

		glPushMatrix();
		glMultMatrixf(&transform.m); // current * transform
		glutSolidCube(0.5f);
		glPopMatrix();
	}

	glutSwapBuffers();

	deltaTime = (float)(glutGet(GLUT_ELAPSED_TIME) - startFrame) / 1000.0f;
}

void resize(int maxX, int maxY)
{
	glViewport(0, 0, maxX, maxY);
	projection = m4_perspective(73.74f, 1.0f*maxX / maxY, 0.2f, 200.0f);
}

int main(int* argc, char **argv)
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("__4K_demo");

	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutIdleFunc(display);

	init();

	glutMainLoop();
	return 0;
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
		write(L"%s", L"get_comand_line failed\n");
		ExitProcess(1);
	}

	args->argc = nArgs;
	args->argv = szArglist;
	LocalFree(szArglist);
}

void _cdecl mainCRTStartup()
{
	write(L"Running mainCRTStartup...v%.2f\n", 2.1);
	args args;
	get_comand_line(&args);
	main(&args.argc, &args.argv);
}