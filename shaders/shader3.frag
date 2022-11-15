#version 120

varying vec3 N;
varying vec3 E;
varying vec3 colorb;
varying vec3 colorw;
void main()
{
	if (dot(N, E) < 0.3  && dot(N, E) > -0.3) {
		gl_FragColor = vec4(colorw, 1.0f);
	} else {
		gl_FragColor = vec4(colorb, 1.0f);
	}
}