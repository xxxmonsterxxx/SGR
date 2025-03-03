#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout (binding = 1) uniform UboInstance {
	mat4 model;
} uboInstance;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in int inTexInd;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inColor;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) flat out int fragTexInd;
layout(location = 2) out vec4 fragColor;

void main() {
    gl_Position =  ubo.proj * ubo.view * uboInstance.model * vec4(inPosition, 1.0);

    fragTexCoord = inTexCoord;
    fragTexInd = inTexInd;
    fragColor = inColor;
}