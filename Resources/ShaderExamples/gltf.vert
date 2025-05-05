#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in vec3 inColor;

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout (binding = 2) uniform UboInstance {
	mat4 model;
} uboInstance;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outUV;
layout (location = 3) out vec3 outViewVec;
layout (location = 4) out vec3 outLightVec;

void main()
{
	outNormal = inNormal;
	outColor = inColor;
	outUV = inUV;
	gl_Position = ubo.proj * ubo.view * uboInstance.model * vec4(inPos.xyz, 1.0);
	
	vec4 pos = ubo.view * vec4(inPos, 1.0);
	outNormal = mat3(ubo.view) * inNormal;
	vec4 lightPos = vec4(5.0f, 5.0f, -5.0f, 1.0f);
	outLightVec = lightPos.xyz - pos.xyz;
	vec4 viewPos = vec4(0.0f, 0.0f, -1.0f, 1.0f);
	outViewVec = viewPos.xyz - pos.xyz;
}