#pragma once

#include "lmath.h"

extern mat4_t g_transform;

typedef struct { 
	int numberOfBlock, windingCount, segCount;
	float *jitArray;
} geometry_info;


void fill_jitArray(geometry_info *info)
{
	info->jitArray = (float*)LocalAlloc(LPTR,
		info->segCount * info->windingCount * sizeof(float));

	for (int i = 0; i < info->segCount; i += info->windingCount) {
		for (int j = 0; j < info->windingCount; j++) {
			info->jitArray[i + j] = lrandf(10.0f*j, 10.0f*j);
		}
	}
}

void free_jitArray(geometry_info *info)
{
	LocalFree(info->jitArray);
}

void draw_ring(float z, float radius, float jit, const geometry_info *info)
{
	vec3_t scale = (vec3_t) { 0.65f, 0.05f, 1.0f };

	float step = 360 / info->numberOfBlock;

	for (float angle = 0.0 + jit; angle < 360 - jit; angle += step)
	{
		const float angle_in_rad = angle*Deg2Rad;

		float s, c;
		lsincosf(angle_in_rad, &s, &c);
		vec3_t offset = (vec3_t) { c * radius, s * radius, z };

		g_transform = get_transform(
			m4_translation(offset),
			m4_rotation_z((angle + 90.0f) * Deg2Rad),
			m4_scaling(scale));

		glPushMatrix();
		glMultMatrixf(&g_transform.m); // current * transform
		glutSolidCube(0.5f);
		glPopMatrix();
	}
}

void draw_geom(geometry_info *info)
{
	for (int depth = 0; depth < info->segCount; depth += info->windingCount) {
		float z = -(float)depth / info->windingCount;
		float currentRadius = 1.0f;
		for (int w = 0; w < info->windingCount; w++) {
			draw_ring(z, currentRadius, info->jitArray[depth + w], info);
			currentRadius += 0.1f;
		}
	}
}