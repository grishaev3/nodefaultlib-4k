#version 330 core

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_normal;

uniform mat4 u_world;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform vec3 u_LightPos;

varying vec3 v_Position;       // This will be passed into the fragment shader.
varying vec4 v_Color;          // This will be passed into the fragment shader.
varying vec3 v_Normal;         // This will be passed into the fragment shader.


void main()
{
	mat4 u_MVMatrix = u_view * u_world;

	// Transform the vertex into eye space.
	v_Position = vec3(u_MVMatrix * vec4(a_vertex, 1.0));

	// Transform the normal's orientation into eye space.
	v_Normal = vec3(u_MVMatrix * vec4(a_normal, 0.0));

    // Apply all matrix transformations to vert
	gl_Position = u_proj * u_view * u_world * vec4(a_vertex, 1.0);
}
