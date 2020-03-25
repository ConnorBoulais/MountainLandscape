#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
//layout(location = 3) in vec4 InstancePos;


uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 centerpos;
uniform vec3 int_campos;

uniform float h_freq;
uniform float b_freq;
uniform float b_pow;
uniform float amplitude;
uniform int h_oct;

out vec3 vertex_pos;
out vec3 vertex_normal;
out vec2 vertex_tex;
out float tree_height;
out vec4 view_position;

float hash(float n) { return fract(sin(n) * 753.5453123); }
float snoise(vec3 x)
	{
	vec3 p = floor(x);
	vec3 f = fract(x);
	f = f * f * (3.0 - 2.0 * f);

	float n = p.x + p.y * 157.0 + 113.0 * p.z;
	return mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),
		mix(hash(n + 157.0), hash(n + 158.0), f.x), f.y),
		mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),
			mix(hash(n + 270.0), hash(n + 271.0), f.x), f.y), f.z);
	}

float noise(vec3 position, int octaves, float frequency, float persistence) {
	float total = 0.0;
	float maxAmplitude = 0.0;
	float amplitude = 1.0;
	for (int i = 0; i < octaves; i++) {
		total += snoise(position * frequency) * amplitude;
		frequency *= 2.0;
		maxAmplitude += amplitude;
		amplitude *= persistence;
		}
	return total / maxAmplitude;
	}

float getHeight(vec3 pos)
{
	float height = noise(pos.xzy, h_oct, h_freq, 0.6);
	float baseheight = noise(pos.xzy, h_oct, b_freq, 0.3);
	baseheight = pow(baseheight, b_pow);
	height = baseheight*height;
	height*=amplitude;
	return height;
}

mat4 getRy(float angle)
{
	return mat4(
		vec4(cos(angle),0,-sin(angle),0),
		vec4(0,1,0,0),
		vec4(sin(angle),0,cos(angle),0),
		vec4(0,0,0,1));
}

void main()
{
	vertex_normal = vec4(M * vec4(vertNor,0.0)).xyz;
	
	vec4 tpos =  M * vec4(vertPos, 1.0);

	tree_height = getHeight(centerpos);
	float transform_fact = int(tree_height*1000) % 10;
	vec3 shift = vec3(transform_fact, 0, -transform_fact);
	tpos += vec4(shift,0);
	vec3 center = centerpos + shift;

	tree_height = getHeight(center);	
	tpos.y += tree_height;

//	tpos = Ry * tpos;
//
//	float transform_fact = int(tree_height*1000) % 10;
//	tpos.x += transform_fact;
//	tpos.z -= transform_fact;

	vertex_pos = tpos.xyz;
	vertex_tex = vertTex;
	view_position = V*M*vec4(vertPos,1);
	gl_Position = P * V * tpos;
}
