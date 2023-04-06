#version 400

// in from vertex shader
in vec4 ex_Position;
in vec4 ex_PositionWorld;
in vec4 ex_Normal;
in vec4 ex_UV_x2;
in vec4 ex_Color;

// Default colour output
layout ( location = 0 ) out vec4 out_Colour;

// Textures
uniform sampler2D Texture00;

uniform vec3 Color;

void main(void)
{
	if (ex_Color.x == 0) out_Colour = vec4(1, 0, 0, 1);
	if (ex_Color.x == 1) out_Colour = vec4(0, 1, 0, 1);
	if (ex_Color.x == 2) out_Colour = vec4(0, 0, 1, 1);
	if (ex_Color.x == 3) out_Colour = vec4(1, 1, 0, 1);
	if (ex_Color.x == 4) out_Colour = vec4(1, 0, 1, 1);
	if (ex_Color.x == 5) out_Colour = vec4(0, 1, 1, 1);
	if (ex_Color.x == 6) out_Colour = vec4(0.5, 0, 0, 1);
	if (ex_Color.x == 7) out_Colour = vec4(0.5, 0.5, 0, 1);
	if (ex_Color.x == 8) out_Colour = vec4(0.5, 0, 0.5, 1);
	if (ex_Color.x == 9) out_Colour = vec4(0, 0.5, 0.5, 1);
	if (ex_Color.x == 10) out_Colour = vec4(0.25, 0, 0, 1);
	if (ex_Color.x == 11) out_Colour = vec4(0.25, 0.25, 0, 1);
	if (ex_Color.x == 12) out_Colour = vec4(0.25, 0, 0.25, 1);
	if (ex_Color.x == 13) out_Colour = vec4(0, 0.25, 0.25, 1);
	if (ex_Color.x == 14) out_Colour = vec4(0.75, 0, 0, 1);
	if (ex_Color.x == 15) out_Colour = vec4(0.75, 0.75, 0, 1);
	if (ex_Color.x == 16) out_Colour = vec4(0.75, 0, 0.75, 1);
	if (ex_Color.x == 17) out_Colour = vec4(0, 0.75, 0.75, 1);
	if (ex_Color.x == 18) out_Colour = vec4(0.75, 0.75, 0.75, 1);
	if (ex_Color.x == 19) out_Colour = vec4(0.25, 0.25, 0.25, 1);
	if (ex_Color.x == 20) out_Colour = vec4(0.5, 0.5, 0.5, 1);
	if (ex_Color.x > 20) out_Colour = vec4(1, 1, 1, 1);

}
