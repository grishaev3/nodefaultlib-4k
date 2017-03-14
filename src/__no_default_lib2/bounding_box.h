#pragma once

#include "math_3d.h"

#ifdef _DEBUG

typedef struct bounding_box {
	vec3_t far_left;
	vec3_t near_left;
	vec3_t near_right;
	vec3_t far_right;
} bounding_box;

float get_near_dist(bounding_box* bb)
{
	return 0.2;
}
float get_far_dist(bounding_box* bb)
{
	return 200.0f;
}

#endif // _DEBUG