#version 330 core
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 viewpos;

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;
in vec4 view_position;

uniform sampler2D tex;

void main()
{
color = texture(tex, vertex_tex);
viewpos = view_position;
viewpos.z *= -1;
}
