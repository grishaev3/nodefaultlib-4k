#pragma once


int lrandi(unsigned int *seed)
{
	unsigned int next = *seed;
	int result;

	next *= 1103515245;
	next += 12345;
	result = (unsigned int)(next / 65536) % 2048;

	next *= 1103515245;
	next += 12345;
	result <<= 10;
	result ^= (unsigned int)(next / 65536) % 1024;

	next *= 1103515245;
	next += 12345;
	result <<= 10;
	result ^= (unsigned int)(next / 65536) % 1024;

	*seed = next;

	return result;
}

float lrandf(float a, float b) {
	unsigned seed = 42;
	float random = ((float)lrandi(&seed)) / (float)RAND_MAX;
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

