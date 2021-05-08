#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <utility>
#include <set>
#include <algorithm>
#include <fstream>

enum sgrErrCode
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
	sgrReinitCommandBuffersError
};
