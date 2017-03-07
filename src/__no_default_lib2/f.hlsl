varying vec4 diffuse; 

void main()
{
    gl_FragColor = (gl_LightSource[0].ambient + diffuse); 
}