#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
	vec4 texColor = texture(texSampler, fragTexCoord);
    if (texColor.a < 1.f)
        discard;
    outColor = texColor;
}