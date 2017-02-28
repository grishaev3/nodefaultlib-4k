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
	double *ttan;

	__asm {
		fld qword ptr angle
		fptan
		fstp qword ptr ttan
	}

	return (float)*ttan;
}

float lsqrtf(double value)
{
	double *result;

	__asm {
		fld qword ptr value
		fsqrt
		fstp qword ptr result
	}

	return (float)*result;
}

float labsf(double value)
{
	double *result;

	__asm {
		fld qword ptr value
		fistp qword ptr result
	}

	return (float)*result;
}

