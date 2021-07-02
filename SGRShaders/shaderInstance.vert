#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout (binding = 2) uniform UboInstance {
	mat4 model; 
    vec2 spriteCenter; // center of sprite in spritesheet
    vec2 spriteSize;  // size of sprite in spritesheet
} uboInstance;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

vec2 pointCoordToTexCoord(vec2 pointCoord) {
    return (pointCoord + vec2(1,1)) * 0.5;
}

void main() {
    gl_Position = ubo.view * ubo.proj * uboInstance.model * vec4(inPosition, 0.0, 1.0);

    fragColor = inColor;

    vec2 dTex = uboInstance.spriteSize * 0.5; // delta texture coord according delta point coord

    fragTexCoord = vec2(inPosition.x * dTex.x + uboInstance.spriteCenter.x, inPosition.y * dTex.y + uboInstance.spriteCenter.y);
}