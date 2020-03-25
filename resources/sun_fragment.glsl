#version 330 core
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 viewpos;
in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
in vec4 view_position;

uniform vec3 centerpos;

uniform sampler2D tex;

void main()
{

//color = vec4(1,1,0,1);
vec4 texturecolor = texture(tex, vertex_tex);
vec4 t_color;
t_color = texturecolor;

//vec3 n = normalize(-vertex_normal);
//vec3 lp=vec3(-10,10,1000);
//vec3 ld = normalize(lp - vertex_pos);
//float diffuse = dot(n,ld);
//color = t_color*0.8 + t_color*diffuse*0.2;
//color.a = 1;

color = t_color;//vec4(1,1,0,1);
//float fact = pow(length(vertex_pos - centerpos)*7,6);
//color.a = (0 - 1)*fact + 1;
color.a = 1;

float len = length(vertex_pos - centerpos);
float fact = pow(len/0.32f,14);
color.a = (0-1)*fact + 1;

color.r = pow(color.r,3);
color.g = pow(color.g,3);
color.b = pow(color.b,3);
viewpos = view_position;
viewpos.z *= -1;
}
