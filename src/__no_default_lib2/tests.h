#pragma once

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lmath.h"

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
	l_sincosf(deginrad, &s, &c);

	FILE *fp = fopen("debug.txt", "w");
	if (!fp) return;

	fprintf_s(fp, "sin %d: %f{%f}\n", (int)angle, s, sin(deginrad));
	fprintf_s(fp, "cos %d: %f{%f}\n", (int)angle, c, cos(deginrad));
	fprintf_s(fp, "tan %d: %f{%f}\n", (int)angle, l_tanf(deginrad), tan(deginrad));
	fprintf_s(fp, "sqrt %d: %f{%f}\n", (int)angle, l_sqrtf(deginrad), sqrt(deginrad));
	fprintf_s(fp, "fabs %f: %f{%f}\n", -angle * M_PI, l_absf(-angle * M_PI), fabsf(-angle * M_PI));

	fclose(fp);
}

/*
void test_trigonometry_sse()
{
	v4sf x, s, c;
	float angle = 42.0f;
	float deginrad = angle * Deg2Rad;

	x.m128_f32[0] = angle;
	sincos_ps(x, &s, &c);

	FILE *fp = fopen("debug.txt", "w");
	if (!fp) return;
	fprintf(fp, "sin %d: %f{%f}\n", (int)angle, s.m128_f32[0]);
	fclose(fp);
}
*/

int test_random_numbers()
{
	FILE *fp = fopen("debug.txt", "w");
	if (!fp) return;

	l_srand(GetTickCount());
	fprintf(fp, "lrand()\n");
	for (int i = 0; i < 10; i++)
		fprintf(fp, "%d\n", l_rand());

	fprintf(fp, "\nfrand(0.0f, 10.0f)\n");
	for (int i = 0; i < 10; i++)
		fprintf(fp, "%f\n", l_randf(0, 10));

	fclose(fp);
}