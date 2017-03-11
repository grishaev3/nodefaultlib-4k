#pragma once

#include "lmath.h"

extern mat4_t g_transform;


float ver[8][3] =
{
	{ -0.25,-0.25,0.25 },
	{ -0.25,0.25,0.25 },
	{ 0.25,0.25,0.25 },
	{ 0.25,-0.25,0.25 },
	{ -0.25,-0.25,-0.25 },
	{ -0.25,0.25,-0.25 },
	{ 0.25,0.25,-0.25 },
	{ 0.25,-0.25,-0.25 },
};

void quad(int a, int b, int c, int d)
{
	glBegin(GL_QUADS);
	glColor3f(1.0f,1.0f,1.0f);

	glVertex3fv(ver[a]);
	glVertex3fv(ver[b]);
	glVertex3fv(ver[c]);
	glVertex3fv(ver[d]);
	glEnd();
}

void colorcube()
{
	quad(0, 3, 2, 1);
	quad(2, 3, 7, 6);
	quad(0, 4, 7, 3);
	quad(1, 2, 6, 5);
	quad(4, 5, 6, 7);
	quad(0, 1, 5, 4);
}

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
			m4_rotation_z((angle - 90.0f) * Deg2Rad),
			m4_scaling(scale));

		glPushMatrix();
		glMultMatrixf((const GLfloat*)&g_transform.m); // current * transform
		colorcube();
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