#version 450

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require

layout(binding = 2) uniform sampler2D texSamplers[];

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in flat uint fragTexInd;
layout(location = 2) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    if (nonuniformEXT(fragTexInd) == -1)
        outColor = fragColor;
    else {
	    vec4 texColor = texture(texSamplers[nonuniformEXT(fragTexInd)], fragTexCoord);
        texColor[3] = fragColor[3];
        //if (texColor.a < 1.f)
        //    outColor = vec4(1, 0, 0, 1);
        //else
            outColor = texColor;
    }
}