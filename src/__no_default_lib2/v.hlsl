attribute vec4 at_vertex;

uniform mat4 in_prj_view;
uniform mat4 in_world;

void main()
{
	gl_Position = in_prj_view * in_world * at_vertex;
}
