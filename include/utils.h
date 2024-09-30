#pragma once


#if _WIN64
#include "Windows.h"
#define PATH_MAX MAX_PATH
#endif

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
#include <glm/glm.hpp>

#include <cstdint>
#include <string.h>
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

using SgrTime = std::chrono::steady_clock;
using SgrTime_t = std::chrono::steady_clock::time_point;
using SgrVertex = glm::vec3;

struct SgrGlobalUniformBufferObject {
	glm::mat4 view = glm::mat4(1.f);
	glm::mat4 proj = glm::mat4(1.f);
};

struct SgrInstancesUniformBufferObject {
	void* data = nullptr;
	size_t instanceSize = 0;
	size_t instnaceCount = 0;
	size_t dynamicAlignment = 0;
	size_t dataSize = 0;
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
	sgrIncorrectNumberBuffersForDescriptors,
	sgrDoubleSettingVertices,
	sgrDescrUpdateError,
	sgrMissingShaders,
	sgrMissingObject,
	sgrMissingPipeline,
	sgrMissingDescriptorSets,
	sgrUnknownVkDescriptorType,
	sgrShaderToDeleteNotFound,
	sgrUnknownGeometry,
	sgrDescriptorsWithUnknownInfo,
	sgrMissingInstance,
	sgrSupportedFormatsNotFound,
	sgrLoadFontPixelsError,
	sgrIncorrectImagePixelFormat,
	sgrValidationLayerNotSupported,
	sgrExtensionNotSupport,
	sgrDebugMessengerCreationFailed,
	sgrDebugMessengerDestructionFailed,
	sgrDescriptorsSetsUpdated
};

#if __APPLE__
	#include <mach-o/dyld.h>
	#include <CoreFoundation/CFBundle.h>
#endif

#if __linux__
	#include <unistd.h>
#endif

std::string getExecutablePath();
