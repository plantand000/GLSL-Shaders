#version 120

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalTransform;

struct lightStruct
{
	vec3 position;
	vec3 color;
};

#define NUM_LIGHTS 2

uniform lightStruct lights[NUM_LIGHTS];

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

uniform vec3 e; // already in world space

varying vec3 u_N;
varying vec3 p;
vec3 color;

void main()
{
	vec3 N = normalize(u_N);
    vec3 E = normalize(e - p);
	color = ka;
	for (int i = 0; i < NUM_LIGHTS; i++) {
		// light position already in world coordinate
		vec3 L_i = normalize(lights[i].position - p);
		vec3 R_i = normalize((2 * dot(L_i,N)) * N - L_i);
		vec3 i_diffuse = kd * max(0, dot(L_i, N));
		vec3 i_spec = ks * pow(max(0, dot(R_i,E)), s);
		color += (lights[i].color * (i_diffuse + i_spec));
	}
	gl_FragColor = vec4(color, 1.0f);
}