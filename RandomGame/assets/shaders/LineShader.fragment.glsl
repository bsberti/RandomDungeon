#version 400

// in from vertex shader
in vec4 ex_Color;

// Default colour output
layout ( location = 0 ) out vec4 out_Colour;

void main(void)
{
	out_Colour = vec4(ex_Color.xyz, 1);
}
