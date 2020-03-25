#version 330 core
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 viewpos;

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
in float tree_height;
in vec4 view_position;

uniform sampler2D tex; //tree tex
uniform sampler2D shadow_tex; //shadow tex
uniform vec3 campos;

uniform mat4 lightSpace;

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
        if (fragDepth > textureOffset(shadow_tex, shifted.xy, offsets[i]).r) {
            shadowFactor += 1;
        }
    }
    shadowFactor /= numSamples;

    return shadowFactor;
}

void main()
{

vec4 fragLightSpacePos = lightSpace * vec4(vertex_pos,1);
float shadowFactor = 1.0 - calcShadowFactor(fragLightSpacePos);

vec3 n = normalize(-vertex_normal);

//if(tree_height >= 2 || tree_height <= -5){
//	color = vec4(1,1,1,0);
//	return;
//}

//color = vec4(1,1,0,1);
vec4 t_color;
t_color = texture(tex, vertex_tex);

//vec3 camera_pos = -campos;
vec3 lp=vec3(-10,10,1000);
vec3 ld = normalize(lp - vertex_pos);
float diffuse = dot(n,ld);
color = t_color*0.8 + t_color*diffuse*0.2;
color.a = 1;

color.rgb*=(0.6 + 0.4*shadowFactor);

//vec3 pixel_color = basecolor*ambient + basecolor*diffuse*(1-ambient);
//
//vec3 cd = normalize(camera_pos - vertex_pos);
//vec3 h = normalize(cd+ld);
//float spec = dot(n,h);
//spec = clamp(spec,0,1);
//spec = pow(spec,specpow);
//pixel_color += vec3(1,1,1)*spec*3;
//color = vec4(pixel_color,a);

float len = length(vertex_pos.xz+campos.xz);
len-=41;
len/=8.;
len=clamp(len,0,1);
color.a=1-len;

viewpos = vec4(vertex_pos,1);
viewpos.z *= -1;
}
