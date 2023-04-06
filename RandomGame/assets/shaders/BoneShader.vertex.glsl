#version 400

layout(location=0) in vec4 in_Position;
layout(location=1) in vec4 in_UV_x2;
layout(location=2) in vec4 in_Normal;
layout(location=3) in vec4 in_BoneWeights;
layout(location=4) in vec4 in_BoneIds;

out vec4 ex_Position;			
out vec4 ex_PositionWorld;
out vec4 ex_Normal;
out vec4 ex_UV_x2;
out vec4 ex_Color;

uniform mat4 ModelMatrix;	
uniform mat4 RotationMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

// BoneMatrix is Transforms, Rotations, and Scale
uniform mat4 BoneMatrices[52];

// Only include Rotations, for updating the normals
uniform mat4 BoneRotationMatrices[52];

void main(void)
{
	mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	
	mat4 boneTransform = BoneMatrices[int(in_BoneIds[0])] * in_BoneWeights[0];
	boneTransform += BoneMatrices[int(in_BoneIds[1])] * in_BoneWeights[1];
	boneTransform += BoneMatrices[int(in_BoneIds[2])] * in_BoneWeights[2];
	boneTransform += BoneMatrices[int(in_BoneIds[3])] * in_BoneWeights[3];
	vec4 position = boneTransform * in_Position;

	ex_Color.x = in_BoneIds[0];

	// Next step
	// Include Rotations
//	vec4 normal = vec4(0,0,0,0);
//	normal += BoneMatrices[in_BoneIds[0]] * normalize(in_Normal) * in_BoneWeights[0];
//	normal += BoneMatrices[in_BoneIds[1]] * normalize(in_Normal) * in_BoneWeights[1];
//	normal += BoneMatrices[in_BoneIds[2]] * normalize(in_Normal) * in_BoneWeights[2];
//	normal += BoneMatrices[in_BoneIds[3]] * normalize(in_Normal) * in_BoneWeights[3];

	gl_Position = MVP * position;
	ex_Position = position;
	ex_PositionWorld = ModelMatrix * position;
	ex_Normal = normalize(in_Normal);
	ex_UV_x2 = in_UV_x2;
}
