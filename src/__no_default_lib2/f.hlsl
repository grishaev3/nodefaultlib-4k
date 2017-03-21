#version 330 core

uniform vec4 in_Color;

uniform mat4 in_prj_view;
uniform mat4 in_world;

in vec3 frag_vertex;
in vec3 frag_normal;

out vec4 final_color;

void main()
{
    //calculate normal in world coordinates
    mat3 normalMatrix = transpose(inverse(mat3(in_world)));
    vec3 normal = normalize(normalMatrix * frag_normal);
    
    //calculate the location of this fragment (pixel) in world coordinates
    vec3 fragPosition = vec3(in_world * vec4(frag_vertex, 1));
    
    //calculate the vector from this pixels surface to the light source
    vec3 surfaceToLight = vec3(0.0) - fragPosition;
    
    //calculate the cosine of the angle of incidence
    float brightness = dot(normal, surfaceToLight) / (length(surfaceToLight) * length(normal));
    brightness = clamp(brightness, 0.2, 1);

	final_color = vec4(brightness);
	//final_color = vec4(1);
}