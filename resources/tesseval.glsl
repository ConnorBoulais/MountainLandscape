#version 410 core

layout (triangles, fractional_even_spacing, cw) in;	//equal_spacing
in vec2 TE_vertex_tex[];
in vec3 TE_vertex_normal[];
in vec4 TE_viewpos[];
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform sampler2D tex;
uniform vec3 camoff;

uniform float h_freq;
uniform float b_freq;
uniform float b_pow;
uniform float amplitude;
uniform int h_oct;

out vec2 frag_tex;
out vec3 position;
out vec3 normal;

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
    vec4 pos = (gl_TessCoord.x * gl_in[0].gl_Position +
                   gl_TessCoord.y * gl_in[1].gl_Position +
                   gl_TessCoord.z * gl_in[2].gl_Position);

	vec2 Tex = (gl_TessCoord.x * TE_vertex_tex[0] +
                   gl_TessCoord.y * TE_vertex_tex[1] +
                   gl_TessCoord.z * TE_vertex_tex[2]);

	pos.y += (getHeight(vec3(pos))/1.f);

	position = vec3(pos);

	normal = normalize(gl_TessCoord.x * TE_vertex_normal[0] + 
						gl_TessCoord.y * TE_vertex_normal[1] + 
						gl_TessCoord.z * TE_vertex_normal[2]); 

	frag_tex = Tex;
	gl_Position = P * V * pos;

}