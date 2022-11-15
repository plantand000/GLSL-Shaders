#version 120

attribute vec3 vPositionModel; // in object space
attribute vec3 vNormalModel; // in object space

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

varying vec3 E;
varying vec3 N;
varying vec3 colorb;
varying vec3 colorw;

void main()
{
	colorb = vec3(0.0,0.0,0.0);
	colorw = vec3(1.0,1.0,1.0);
	gl_Position = projection * view * model * vec4(vPositionModel, 1.0);

	// translate position and normal to world space (use inv and transpose for normals)
	vec4 transPos = model * vec4(vPositionModel, 1.0); 
	vec4 transNorm = normalTransform * vec4(vNormalModel, 1.0);

	// homogenize tranformations
	vec3 p = transPos.xyz / transPos.w; 
	N = normalize(transNorm.xyz / transNorm.w); // N vector

	// compute L_i, R_i, E vectors
	E = normalize(e - p);
}
