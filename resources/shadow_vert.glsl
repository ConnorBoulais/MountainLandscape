#version 450 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform float h_freq;
uniform float b_freq;
uniform float b_pow;
uniform float amplitude;
uniform int h_oct;

uniform int render_terrain;
uniform vec3 camoff;

uniform vec3 centerpos;

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

void main()
{

	if(render_terrain == 0){
		vec4 tpos = M * vec4(vertPos,1);
		float tree_height = getHeight(centerpos);
		float transform_fact = int(tree_height*1000) % 10;
		vec3 shift = vec3(transform_fact, 0, -transform_fact);
		tpos += vec4(shift,0);
		vec3 center = centerpos + shift;

		tpos.y += getHeight(center);	

		gl_Position = P * V * tpos;
		return;
	}
	else{
		vec4 tpos = vec4(vertPos,1);
		tpos.z -=camoff.z;
		tpos.x -=camoff.x;
		tpos = M * tpos;

		tpos.y += getHeight(vec3(tpos.x, tpos.y, tpos.z));

		gl_Position = P * V * tpos;
		return;
	}
}
