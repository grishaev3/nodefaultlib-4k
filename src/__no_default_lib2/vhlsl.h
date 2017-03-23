/* File generated with Shader Minifier 1.1.4
 * http://www.ctrl-alt-test.fr
 */
#ifndef VHLSL_H_
# define VHLSL_H_

const char *v_hlsl =
 "#version 330 core\n"
 "layout(location=0)in vec3 a_vertex;"
 "layout(location=1)in vec3 a_normal;"
 "uniform mat4 u_world,u_view,u_proj;"
 "uniform vec3 u_LightPos;"
 "varying vec3 v_Position;"
 "varying vec4 v_Color;"
 "varying vec3 v_Normal;"
 "void main()"
 "{"
   "mat4 v=u_view*u_world;"
   "v_Position=vec3(v*vec4(a_vertex,1.));"
   "v_Normal=vec3(v*vec4(a_normal,0.));"
   "gl_Position=u_proj*u_view*u_world*vec4(a_vertex,1.);"
 "}";

#endif // VHLSL_H_
