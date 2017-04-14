#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef Deg2Rad
#define Deg2Rad 0.0174532925199433
#endif

#ifndef RAND_MAX
#define RAND_MAX	0x7fff
#endif

static unsigned long int next = 1;

int l_rand(void) // RAND_MAX assumed to be 32767
{
	next = next * 1103515245 + 12345;
	return (unsigned int)(next / 65536) % 32768;
}

void l_srand(unsigned int seed)
{
	next = seed;
}

int l_randi(int min, int max)
{
	return (l_rand() % (max - min)) + min;
}

float l_randf(float a, float b) {
	float random = ((float)l_rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

void l_sincosf(double angle, float* psin, float* pcos)
{
	double tsin, tcos;

	__asm {
		fninit
		fld qword ptr angle
		fsincos
		fstp qword ptr tcos
		fst qword ptr tsin
	}

	*psin = (float)tsin;
	*pcos = (float)tcos;
}

float l_tanf(double angle)
{
	double ttan, one;

	__asm {
		fninit
		fld qword ptr angle
		fptan
		fstp qword ptr one
		fst qword ptr ttan
	}

	return (float)ttan;
}

float l_sqrtf(double value)
{
	double result;

	__asm {
		fninit
		fld qword ptr value
		fsqrt
		fstp qword ptr result
	}

	return (float)result;
}

float l_absf(double value)
{
	return (value < 0) ? -value : value;
}

