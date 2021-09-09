#include "PipelineManager.h"
#include "SwapChainManager.h"
#include "LogicalDeviceManager.h"
#include "RenderPassManager.h"

PipelineManager* PipelineManager::instance = nullptr;

PipelineManager::PipelineManager() 
{
    SgrPipeline emptyPipeline;
    emptyPipeline.name = "empty";
    pipelines.push_back(emptyPipeline);
}

PipelineManager::~PipelineManager() { ; }

PipelineManager* PipelineManager::get()
{
	if (instance == nullptr) {
		instance = new PipelineManager();
		return instance;
	}
	else
		return instance;
}

SgrErrCode PipelineManager::createAndAddPipeline(std::string name, ShaderManager::SgrShader objectShaders, DescriptorManager::SgrDescriptorInfo descriptorInfo, bool filled)
{
	SgrPipeline newPipeline;
	newPipeline.name = name;
	newPipeline.filled = filled;
    SgrErrCode resultCreatePipeline = createPipeline(objectShaders, descriptorInfo, newPipeline);
    if (resultCreatePipeline != sgrOK)
        return resultCreatePipeline;
	pipelines.push_back(newPipeline);

    return sgrOK;
}

SgrErrCode PipelineManager::createPipeline(ShaderManager::SgrShader objectShaders, DescriptorManager::SgrDescriptorInfo descriptorInfo, SgrPipeline& sgrPipeline)
{
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = objectShaders.vkShaders.vertex;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = objectShaders.vkShaders.fragment;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(descriptorInfo.vertexBindingDescr.size());
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(descriptorInfo.vertexAttributeDescr.size());
    vertexInputInfo.pVertexBindingDescriptions = descriptorInfo.vertexBindingDescr.data();
    vertexInputInfo.pVertexAttributeDescriptions = descriptorInfo.vertexAttributeDescr.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)SwapChainManager::get()->getExtent().width;
    viewport.height = (float)SwapChainManager::get()->getExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = SwapChainManager::get()->getExtent();

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = sgrPipeline.filled ? VK_POLYGON_MODE_FILL : VK_POLYGON_MODE_LINE;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    // ????? do wee need specify all similar layouts or only one
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = descriptorInfo.setLayouts.data();

    VkDevice logicalDevice = LogicalDeviceManager::instance->logicalDevice;

	VkPipelineLayout* newLayout = new VkPipelineLayout;
    if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, newLayout) != VK_SUCCESS)
        return sgrInitPipelineLayoutError;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = *newLayout;
    pipelineInfo.renderPass = RenderPassManager::instance->renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	VkPipeline* newPipeline = new VkPipeline;
    if (vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, newPipeline) != VK_SUCCESS)
        return sgrInitPipelineError;

	sgrPipeline.pipeline = newPipeline;
	sgrPipeline.pipelineLayout = newLayout;

    return sgrOK;
}

SgrErrCode PipelineManager::destroyAllPipelines()
{
    for (size_t i = 0; i < pipelines.size(); i++) {
        vkDestroyPipeline(LogicalDeviceManager::instance->logicalDevice, *pipelines[i].pipeline, nullptr);
        vkDestroyPipelineLayout(LogicalDeviceManager::instance->logicalDevice, *pipelines[i].pipelineLayout, nullptr);
    }

    return sgrOK;
}

SgrErrCode PipelineManager::reinitAllPipelines()
{
    size_t oldPipelineNum = pipelines.size();
    for (size_t i = 0; i < oldPipelineNum; i++) {
        if (pipelines[i].name == "empty")
            continue;

        SgrErrCode resultCreatePipline = createPipeline(ShaderManager::instance->getShadersByName(pipelines[i].name), DescriptorManager::instance->getDescriptorInfoByName(pipelines[i].name), pipelines[i]);
        if (resultCreatePipline != sgrOK)
            return sgrReinitPipelineError;
    }

    return sgrOK;
}

PipelineManager::SgrPipeline* PipelineManager::getPipelineByName(std::string name)
{
    for (size_t i = 0; i < pipelines.size(); i++) {
        if (pipelines[i].name == name)
            return &pipelines[i];
    }

    return &pipelines[0];
}

