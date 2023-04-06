#version 400

layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_Color;
		
out vec4 ex_Color;

uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main(void)
{
	mat4 VP = ProjectionMatrix * ViewMatrix;
	gl_Position = VP * in_Position;
	ex_Color = in_Color;
}
