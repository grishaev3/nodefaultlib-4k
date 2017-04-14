#pragma once




#ifdef _DEBUG

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

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

	fprintf(fp, "sin %d: %f{%f}\n", (int)angle, s, sinf(deginrad));
	fprintf(fp, "cos %d: %f{%f}\n", (int)angle, c, cosf(deginrad));
	fprintf(fp, "tan %d: %f{%f}\n", (int)angle, l_tanf(deginrad), tanf(deginrad));
	fprintf(fp, "sqrt %d: %f{%f}\n", (int)angle, l_sqrtf(deginrad), sqrtf(deginrad));
	fprintf(fp, "fabs %f: %f{%f}\n", -angle * M_PI, l_absf(-angle * M_PI), fabsf(-angle * M_PI));

	fclose(fp);
}

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

#endif // DEBUG#
