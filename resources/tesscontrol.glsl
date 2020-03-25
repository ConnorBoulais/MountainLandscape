#version 410 core

layout (vertices = 3) out;

uniform vec3 campos;
//in float posheight[];
in vec3 vertex_pos[];
in vec2 vertex_tex[];
in vec3 vertex_normal[];
//in vec4 fragLightSpacePos[];

out vec2 TE_vertex_tex[];
out vec3 TE_vertex_normal[];
//out vec4 TE_fragLightSpacePos[];

float GetTessLevel(float Distance0, float Distance1)
{
    float AvgDistance = (Distance0 + Distance1) / 2.0;
    float df=50.;
	float dist = df-AvgDistance;
	dist/=df;
	dist = pow(dist,5);
	float tessfact = dist*16;
	return max(0,tessfact);
}

void main(void)
{
	vec3 cp = -campos; 
	float EyeToVertexDistance0 = distance(cp, vertex_pos[0]);
    float EyeToVertexDistance1 = distance(cp, vertex_pos[1]);
    float EyeToVertexDistance2 = distance(cp, vertex_pos[2]);

    gl_TessLevelOuter[0] = GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);
    gl_TessLevelOuter[1] = GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);
    gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];
    
    // Everybody copies their input to their output
    gl_out[gl_InvocationID].gl_Position =  gl_in[gl_InvocationID].gl_Position;
	TE_vertex_tex[gl_InvocationID] = vertex_tex[gl_InvocationID];
	TE_vertex_normal[gl_InvocationID] = vertex_normal[gl_InvocationID];
	//TE_fragLightSpacePos[gl_InvocationID] = fragLightSpacePos[gl_InvocationID];
}