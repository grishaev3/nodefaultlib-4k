/* File generated with Shader Minifier 1.1.4
 * http://www.ctrl-alt-test.fr
 */
#ifndef SHADER_CODE_H_
# define SHADER_CODE_H_

const char *f_hlsl =
 "#version 330 core\n"
 "uniform vec3 u_LightPos;"
 "varying vec3 v_Position,v_Normal;"
 "void main()"
 "{"
   "float v=length(u_LightPos-v_Position);"
   "vec3 n=normalize(u_LightPos-v_Position);"
   "float l=max(dot(v_Normal,n),.4);"
   "l=l*(1./(1.+.25*v*v));"
   "gl_FragColor=vec4(0.,1.,.25,1.)*l;"
 "}";

#endif // SHADER_CODE_H_
