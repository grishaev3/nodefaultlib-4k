#pragma once

static unsigned long int next = 1;

int lrand(void) // RAND_MAX assumed to be 32767
{
	next = next * 1103515245 + 12345;
	return (unsigned int)(next / 65536) % 32768;
}

void lsrand(unsigned int seed)
{
	next = seed;
}

int lrandi(int min, int max)
{
	return (lrand() % (max - min)) + min;
}

float lrandf(float a, float b) {
	float random = ((float)lrand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

void lsincosf(double angle, float* psin, float* pcos)
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

float ltanf(double angle)
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

float lsqrtf(double value)
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

float labsf(double value)
{
	return (value < 0) ? -value : value;
}

