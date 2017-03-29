/* File generated with Shader Minifier 1.1.4
 * http://www.ctrl-alt-test.fr
 */
#ifndef WIRE_VHLSL_H_
# define WIRE_VHLSL_H_

const char *wire_v_hlsl =
 "#version 330 core\n"
 "layout(location=0)in vec3 a_vertex;"
 "uniform mat4 u_world,u_view,u_proj;"
 "void main()"
 "{"
   "mat4 n=u_view*u_world;"
   "gl_Position=u_proj*u_view*u_world*vec4(a_vertex,1.);"
 "}";

#endif // WIRE_VHLSL_H_
