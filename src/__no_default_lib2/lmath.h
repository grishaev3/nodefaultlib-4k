#pragma once

inline void lsincosf(double angle, float* psin, float* pcos)
{
	double tsin, tcos;
	__asm {
		fninit;
		fld qword ptr angle;
		fsincos;
		fstp qword ptr tcos;
		fst qword ptr tsin;
	}

	*psin = (float)tsin;
	*pcos = (float)tcos;
}