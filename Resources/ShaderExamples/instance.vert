#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout (binding = 2) uniform UboInstance {
	mat4 model;
	vec4 color;
    vec2 deltaCoord; // direvative of countur
    vec2 startMeshPoint; // left top point
	vec2 startTextPoint; // left top point
} uboInstance;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position =  ubo.proj * ubo.view * uboInstance.model * vec4(inPosition, 1.0);

    fragColor = vec3(uboInstance.color.x,uboInstance.color.y,uboInstance.color.z);

    fragTexCoord = vec2(uboInstance.startTextPoint.x + (inPosition.x - uboInstance.startMeshPoint.x)*uboInstance.deltaCoord.x, uboInstance.startTextPoint.y + (inPosition.y - uboInstance.startMeshPoint.y)*uboInstance.deltaCoord.y);
}