#version 330 core

uniform vec3 u_LightPos;       // The position of the light in eye space.
varying vec3 v_Position;       // Interpolated position for this fragment triangle per fragment.
varying vec3 v_Normal;         // Interpolated normal for this fragment.

void main()
{
	// Will be used for attenuation.
	float distance = length(u_LightPos - v_Position);

	// Get a lighting direction vector from the light to the vertex.
	vec3 lightVector = normalize(u_LightPos - v_Position);

	// Calculate the dot product of the light vector and vertex normal. If the normal and light vector are
	// pointing in the same direction then it will get max illumination.
	float diffuse = max(dot(v_Normal, lightVector), 0.4);

	// Add attenuation.
	diffuse = diffuse * (1.0 / (1.0 + (0.25 * distance * distance)));

	// Multiply the color by the diffuse illumination level to get final output color.
	gl_FragColor = vec4(0.0, 1.0, 0.25, 1.0) * diffuse;
}