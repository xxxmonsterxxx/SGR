#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <utility>
#include <set>
#include <algorithm>
#include <fstream>
#include <array>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

using SgrTime = std::chrono::high_resolution_clock;
using SgrTime_t = std::chrono::steady_clock::time_point;

struct Sgr2DVertex {
	glm::vec2 position;
	glm::vec3 color;
	glm::vec2 texCoord;
};

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

enum SgrErrCode
{
	sgrOK,
	sgrBadPointer,
	sgrInitWindowError,
	sgrGPUNotFound,
	sgrInitPhysicalDeviceManagerError,
	sgrInitVulkanError,
	sgrInitSurfaceError,
	sgrInitLogicalDeviceError,
	sgrInitSwapChainError,
	sgrInitImageViews,
	sgrInitPipelineLayoutError,
	sgrInitPipelineError,
	sgrInitRenderPassError,
	sgrInitFrameBuffersError,
	sgrInitCommandPoolError,
	sgrInitCommandBuffersError,
	sgrBeginCommandBufferError,
	sgrEndCommandBufferError,
	sgrInitSyncObjectsError,
	sgrQueueSubmitFailed,
	sgrFailedToAcquireImage,
	sgrFailedPresentImage,
	sgrReinitSwapChainError,
	sgrReinitRenderPassError,
	sgrReinitPipelineError,
	sgrReinitFrameBuffersError,
	sgrReinitCommandBuffersError,
	sgrCreateVkBufferError,
	sgrAllocateMemoryError,
	sgrIncorrectPointer,
	sgrInitDefaultUBODescriptorSetLayoutError,
	sgrInitDefaultUBODescriptorPoolError,
	sgrInitDescriptorSetsError,
	sgrWrongDescrAndUBOSize,
	sgrLoadImageError,
	sgrCreateImageError,
	sgrNoSuitableMemoryFinded,
	sgrUnsupportedLayoutTransition,
	sgrCreateSamplerError,
	sgrIncorrectDescriptorWritesSize,
	sgrIncorrectNumberBuffersForDescriptors
};
