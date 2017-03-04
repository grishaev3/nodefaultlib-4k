#pragma once

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

