#version 330 core

layout (location = 0) in vec3 a_vertex;

uniform mat4 u_world;
uniform mat4 u_view;
uniform mat4 u_proj;

void main()
{
	mat4 u_MVMatrix = u_view * u_world;
	gl_Position = u_proj * u_view * u_world * vec4(a_vertex, 1.0);
}
