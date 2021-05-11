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

struct Sgr2DVertex {
	glm::vec2 position;
	glm::vec3 color;
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
	sgrIncorrectPointer
};
