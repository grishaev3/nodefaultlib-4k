varying vec4 diffuse;
void main()
{
    gl_Position  =  ftransform();
    vec3 normal = gl_Normal;
    vec3 lightVector = normalize(gl_LightSource[0].position.xyz);
    float nxDir = max(0.0, dot(normal, lightVector));
    diffuse = gl_LightSource[0].diffuse * nxDir;
}