#version 330 core

layout (location = 0) in vec3 in_vertex;
layout (location = 1) in vec3 in_normal;

out vec3 frag_vertex;
out vec3 frag_normal;

uniform mat4 in_prj_view;
uniform mat4 in_world;

void main()
{
    // Pass some variables to the fragment shader
    frag_vertex = in_vertex;
    frag_normal = in_normal;
    
    // Apply all matrix transformations to vert
	gl_Position = in_prj_view * in_world * vec4(in_vertex, 1.0);
}
