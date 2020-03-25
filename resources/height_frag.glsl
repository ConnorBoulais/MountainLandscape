#version 330 core
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 viewpos;

in vec2 frag_tex;
in vec3 position;
in vec3 normal;

uniform vec3 campos;
uniform vec3 sun_pos;

uniform float h_freq;
uniform float b_freq;
uniform float b_pow;
uniform float amplitude;
uniform int h_oct;

uniform mat4 V;
uniform mat4 lightSpace;
uniform vec3 lightpos;
uniform vec3 lightdir;

uniform sampler2D tex; //grass
uniform sampler2D tex2; //sand
uniform sampler2D shadowMapTex; //depthmap


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

float calcShadowFactor(vec4 lightSpacePosition) {
    vec3 shifted = (lightSpacePosition.xyz / lightSpacePosition.w + 1.0) * 0.5;

    float shadowFactor = 0;
    float bias = 0.001;
    float fragDepth = shifted.z - bias;

    if (fragDepth > 1.0) {
        return 0.0;
    }

    const int numSamples = 5;
    const ivec2 offsets[numSamples] = ivec2[](
        ivec2(0, 0), ivec2(1, 0), ivec2(0, 1), ivec2(-1, 0), ivec2(0, -1)
    );

    for (int i = 0; i < numSamples; i++) {
        if (fragDepth > textureOffset(shadowMapTex, shifted.xy, offsets[i]).r) {
            shadowFactor += 1;
        }
    }
    shadowFactor /= numSamples;

    return shadowFactor;
}

//float get_shadow_fact(vec4 lightSpacePosition)
//{
//	for();
//	return 0;
//}

vec3 getNewNormal()
{
	vec3 a,b,c;
	float h= 0.4;
	a =  position;
	b =  a + vec3(h,0,0);
	c =  a + vec3(0,0,h);
	a.y += getHeight(a.xyz);
	b.y += getHeight(b.xyz);
	c.y += getHeight(c.xyz);

	vec3 ac = a-c;
	vec3 bc = b-c;
	vec3 nor = normalize(cross(ac.xyz,bc.xyz));

	vec3 n = normalize(-(normal + nor*0.8));
	return n;
}

void main()
{

vec4 fragLightSpacePos = lightSpace * vec4(position,1);
float shadowFactor = 1.0 - calcShadowFactor(fragLightSpacePos);

//vec3 n = getNewNormal();

vec3 n = normalize(normal);

vec3 lp = vec3(0,1000, sun_pos.z*1000);//vec3(-10,10,1000);//sun_pos * 1000;
vec3 ld = normalize(lp-position);
float diffuse = dot(n, ld);

vec4 sandcolor = texture(tex2, frag_tex*100);
vec4 grasscolor = texture(tex, frag_tex*100);
float slope = pow(1-abs(normal.y), 0.3); //0.6 for grass2
vec4 comb_color;

comb_color = (sandcolor - grasscolor) * slope + grasscolor;

float len = length(position.xz+campos.xz);
len-=41;
len/=8.;
len=clamp(len,0,1);
color.rgb = vec3(comb_color)*0.75 + diffuse*vec3(comb_color)*0.25;
color.a=1-len;

color.rgb*=(0.6 + 0.4*shadowFactor);

viewpos = V*vec4(position,1);
viewpos.z *= -1;
//color = texture(shadowMapTex, frag_tex);
}
