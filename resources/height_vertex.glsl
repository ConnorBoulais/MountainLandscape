#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform mat4 lightSpace;

uniform float h_freq;
uniform float b_freq;
uniform float b_pow;
uniform float amplitude;
uniform int h_oct;

out vec3 vertex_pos;
out vec3 vertex_normal;
out vec2 vertex_tex;
out vec4 fragLightSpacePos;
uniform sampler2D tex;

uniform vec3 camoff;

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

vec3 getNormal(vec3 pos)
{
	vec3 a,b,c;
	a =  pos;
	b =  a + vec3(1,0,0);
	c =  a + vec3(0,0,1);
	a.y += getHeight(a.xyz);
	b.y += getHeight(b.xyz);
	c.y += getHeight(c.xyz);
	vec3 ac = a-c;
	vec3 bc = b-c;
	return normalize(cross(ac.xyz,bc.xyz));

}

void main()
{

	vec2 texcoords=vertTex;
	float t=1./100.;
	texcoords -= vec2(camoff.x,camoff.z)*t;
	vertex_tex = texcoords;

	vec4 tpos =  vec4(vertPos, 1.0);
	tpos.z -=camoff.z;
	tpos.x -=camoff.x;

	tpos =  M * tpos;

	vertex_pos = tpos.xyz;

	vertex_normal=getNormal(vertex_pos);

	gl_Position = tpos;
}
