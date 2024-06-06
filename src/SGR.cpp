#include "SGR.h"

#include "ShaderManager.h"
#include "PipelineManager.h"

#if __linux__ || __APPLE__
	#include <unistd.h>
#endif

SGR::SGR(std::string appName, uint8_t appVersionMajor, uint8_t appVersionMinor)
{
	manualWindow = false;
	sgrRunning = false;
	window = nullptr;
	applicationName = "Simple graphic application";
	applicationName = appName;
	this->appVersionMajor = appVersionMajor;
	this->appVersionMinor = appVersionMinor;
	requiredQueueFamilies.push_back(VK_QUEUE_GRAPHICS_BIT); // because graphics bit support also transfer bit
	requiredExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	withSwapChain = true;

	// right sequence of initialization
	windowManager = WindowManager::get();
	physicalDeviceManager = PhysicalDeviceManager::get();
	logicalDeviceManager = LogicalDeviceManager::get();
	swapChainManager = SwapChainManager::get();
	pipelineManager = PipelineManager::get();
	commandManager = CommandManager::get();
	memoryManager = MemoryManager::get();
	descriptorManager = DescriptorManager::get();
	textureManager = TextureManager::get();
	renderPassManager = RenderPassManager::get();
	shaderManager = ShaderManager::get();

	SgrObject emptyObject;
	emptyObject.name = "empty";
	objects.push_back(emptyObject);

	SgrObjectInstance emptyInstance;
	emptyInstance.name = "empty";
	instances.push_back(emptyInstance);

	currentFrame = 0;
}

SGR::~SGR()
{
}

SgrErrCode SGR::init(uint32_t windowWidth, uint32_t windowHeight, const char *windowName)
{
	if (!manualWindow) {
		windowManager->init(windowWidth, windowHeight, windowName);
	}

	window = windowManager->window;
	if (window == nullptr)
		return sgrInitWindowError;

	SgrErrCode resultInitVulkan = initVulkanInstance();
	if (resultInitVulkan != sgrOK)
		return resultInitVulkan;

	SgrErrCode resultInitPhysicalDeviceManager = physicalDeviceManager->init(vulkanInstance);
	if (resultInitPhysicalDeviceManager != sgrOK)
		return resultInitPhysicalDeviceManager;

	SgrErrCode resultInitSurface = swapChainManager->initSurface(vulkanInstance, window);
	if (resultInitSurface != sgrOK)
		return resultInitSurface;

	SgrErrCode resultGetPhysicalDeviceRequired = physicalDeviceManager->findPhysicalDeviceRequired(requiredQueueFamilies, requiredExtensions, SwapChainManager::get()->surface);
	if (resultGetPhysicalDeviceRequired != sgrOK)
		return resultGetPhysicalDeviceRequired;

	SgrErrCode resultInitLogicalDevice = logicalDeviceManager->initLogicalDevice();
	if (resultInitLogicalDevice != sgrOK)
		return resultInitLogicalDevice;

	SgrErrCode resultInitSwapChain = swapChainManager->initSwapChain();
	if (resultInitSwapChain != sgrOK)
		return resultInitSwapChain;

	maxFrameInFlight = swapChainManager->imageCount;

	SgrErrCode resultInitRenderPass = renderPassManager->init();
	if (resultInitRenderPass != sgrOK)
		return resultInitRenderPass;

	SgrErrCode resultInitFrameBuffers = swapChainManager->initFrameBuffers();
	if (resultInitFrameBuffers != sgrOK)
		return resultInitFrameBuffers;

	SgrErrCode resultInitCommandBuffers = commandManager->initCommandBuffers();
	if (resultInitCommandBuffers != sgrOK)
		return resultInitCommandBuffers;

	SgrErrCode resultInitSemaphores = initSyncObjects();
	if (resultInitSemaphores != sgrOK)
		return resultInitSemaphores;

	sgrRunning = true;
	startRunningTime = SgrTime::now();
	windowManager->setSgrPtr(this);

	return sgrOK;
}

SgrErrCode SGR::destroy()
{
	VkDevice device = logicalDeviceManager->logicalDevice;

	vkDeviceWaitIdle(device);

	for (uint8_t i = 0; i < maxFrameInFlight; i++) {
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
    	vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	shaderManager->destroy();
	commandManager->destroy();
	renderPassManager->destroy();
	swapChainManager->destroy(vulkanInstance);
	logicalDeviceManager->destroy();
	physicalDeviceManager->destroy();
	vkDestroyInstance(vulkanInstance, nullptr);
	windowManager->destroy();

	return sgrOK;
}

SgrErrCode SGR::initSGRWindow(GLFWwindow* newWindow, const char* windowName)
{
	SgrErrCode resultCreateWindow = windowManager->init(newWindow, windowName);

	if (resultCreateWindow == sgrOK) {
		manualWindow = true;
		window = windowManager->window;
		return resultCreateWindow;
	}
	
	return resultCreateWindow;
}

void SGR::setAspectRatio(uint8_t x, uint8_t y)
{
	windowManager->setAspectRatio(x, y);
}

SgrErrCode SGR::drawFrame()
{
	SgrTime_t startDrawFrameTime = SgrTime::now();

	drawDataUpdate();

	if (!commandManager->buffersEnded) {
		buildDrawingCommands();
		commandManager->endInitCommandBuffers();
	}

	if (windowManager->windowMinimized)
		glfwWaitEvents();

	vkWaitForFences(logicalDeviceManager->logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkDevice device = logicalDeviceManager->logicalDevice;
	VkSwapchainKHR swapChain = swapChainManager->swapChain;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		unbindAllMeshesAndPiplines();
		SgrErrCode reinitSwapChain = swapChainManager->reinitSwapChain();
		if (reinitSwapChain != sgrOK) {
			return reinitSwapChain;
		}
		imagesInFlight.resize(swapChainManager->imageCount, VK_NULL_HANDLE);
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		return sgrFailedToAcquireImage;
	}

	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	// Mark the image as now being in use by this frame
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandManager->commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(logicalDeviceManager->logicalDevice, 1, &inFlightFences[currentFrame]);

	result = vkQueueSubmit(logicalDeviceManager->graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);
	if (result != VK_SUCCESS)
		return sgrQueueSubmitFailed;

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	result = vkQueuePresentKHR(logicalDeviceManager->presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || windowManager->windowResized) {
		windowManager->windowResized = false;
		unbindAllMeshesAndPiplines();
		swapChainManager->reinitSwapChain();
	}
	else if (result != VK_SUCCESS) {
		return sgrFailedPresentImage;
	}

	currentFrame = (currentFrame + 1) % maxFrameInFlight;

	float drawFrameTime = getSgrTimeDuration(startDrawFrameTime,SgrTime::now());

	if (drawFrameTime < 1.f/fpsDesired) {
		#if __linux__ || __APPLE__
			usleep((1.f/fpsDesired - drawFrameTime)*1000000);
		#endif

		#if _WIN64
			Sleep(DWORD((1.f / fpsDesired - drawFrameTime) * 1000));
		#endif
	}

	return sgrOK;
}

bool SGR::isSGRRunning()
{
	glfwPollEvents();

	if (glfwWindowShouldClose(window))
		sgrRunning = false;

	return sgrRunning;
}

SgrErrCode SGR::initSyncObjects()
{
	imageAvailableSemaphores.resize(maxFrameInFlight);
	renderFinishedSemaphores.resize(maxFrameInFlight);
	inFlightFences.resize(maxFrameInFlight);
	imagesInFlight.resize(swapChainManager->imageCount, VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < maxFrameInFlight; i++) {
		if (vkCreateSemaphore(logicalDeviceManager->logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(logicalDeviceManager->logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(logicalDeviceManager->logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			return sgrInitSyncObjectsError;
		}
	}

	return sgrOK;
}

SgrErrCode SGR::initVulkanInstance()
{
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = applicationName.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(appVersionMajor, appVersionMinor, 0);
	appInfo.pEngineName = "Simple Graphic Renderer";
	appInfo.engineVersion = VK_MAKE_VERSION(this->engineVersionMajor, this->appVersionMinor, this->enginePatch);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#if __APPLE__
	// since VulkanSDK 1.3.216 we should to add VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
	extensions.push_back("VK_KHR_portability_enumeration");
#endif
	// possible???
	// extensions.push_back("VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME");
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = nullptr;

	if (vkCreateInstance(&createInfo, nullptr, &vulkanInstance) != VK_SUCCESS) {
		return sgrInitVulkanError;
	}

	return sgrOK;
}

std::vector<SgrPhysicalDevice> SGR::getAllPhysDevInstances()
{
	return physicalDeviceManager->physicalDevices;
}

void SGR::setRequiredQueueFamilies(std::vector<VkQueueFlagBits> reqFam)
{
	requiredQueueFamilies = reqFam;
}

SgrErrCode SGR::setRenderPhysicalDevice(SgrPhysicalDevice sgrDevice)
{
	if (std::find(physicalDeviceManager->physicalDevices.begin(),
		physicalDeviceManager->physicalDevices.end(), sgrDevice) == physicalDeviceManager->physicalDevices.end())
		return sgrGPUNotFound;

	physicalDeviceManager->pickedPhysicalDevice = sgrDevice;
	return sgrOK;
}

SgrErrCode SGR::setupInstancesUniformBufferObject(SgrBuffer* dynUBOBuffer)
{
	dynamicUBO = dynUBOBuffer;
	return sgrOK;
}

SgrErrCode SGR::addNewObjectGeometry(std::string name, std::vector<SgrVertex> vertices, std::vector<uint16_t> indices,
									 std::string shaderVert, std::string shaderFrag, bool filled,
									 std::vector<VkVertexInputBindingDescription> bindingDescriptions,
									 std::vector<VkVertexInputAttributeDescription> attributDescrtions,
									 std::vector<VkDescriptorSetLayoutBinding> setDescriptorSetsLayoutBinding)
{
	SgrObject newObject;
	newObject.name = name;

	// create vertex buffer
	VkDeviceSize size = sizeof(vertices[0]) * vertices.size();
	newObject.vertices = nullptr;
	SgrErrCode resultAllocateMemoryBuffer = memoryManager->createVertexBuffer(newObject.vertices, size, vertices.data());
	if (resultAllocateMemoryBuffer != sgrOK)
		return resultAllocateMemoryBuffer;

	// create index buffer
	newObject.indicesCount = (uint16_t)indices.size();
	size = sizeof(indices[0]) * indices.size();
	newObject.indices = nullptr;
	resultAllocateMemoryBuffer = memoryManager->createIndexBuffer(newObject.indices, size, indices.data());
	if (resultAllocateMemoryBuffer != sgrOK)
		return resultAllocateMemoryBuffer;


	SgrErrCode initShaderResult = shaderManager->createShaders(name, shaderVert, shaderFrag);
	if (initShaderResult != sgrOK)
		return initShaderResult;

	ShaderManager::SgrShader objectShaders = shaderManager->getShadersByName(name);
	if (objectShaders.name == "empty")
		return sgrMissingShaders;

	DescriptorManager::SgrDescriptorInfo newDescriptorInfo;
	newDescriptorInfo.name = name;
	newDescriptorInfo.vertexBindingDescr = bindingDescriptions;
	newDescriptorInfo.vertexAttributeDescr = attributDescrtions;
	newDescriptorInfo.setLayoutBinding = setDescriptorSetsLayoutBinding;
	descriptorManager->addNewDescriptorInfo(newDescriptorInfo);

	pipelineManager->createAndAddPipeline(name, objectShaders, newDescriptorInfo, filled);

	objects.push_back(newObject);

	return sgrOK;
}

SgrErrCode SGR::addObjectInstance(std::string name, std::string geometry, uint32_t dynamicUBOalignment)
{
	if (findObjectByName(geometry).name == "empty")
		return sgrUnknownGeometry;

	SgrObjectInstance newInstance;
	newInstance.name = name;
	newInstance.geometry = geometry;
	newInstance.uboDataAlignment = dynamicUBOalignment;

	if (instances.back().geometry == geometry) {
		instances.push_back(newInstance);
		return sgrOK;
	} else {
		for (size_t i = instances.size() - 1; i > 0; --i) {
			if (instances[i].geometry == geometry) {
				instances.emplace(instances.begin() + i + 1, newInstance);
				return sgrOK;
			}
		}
	}

	instances.push_back(newInstance);
	return sgrOK;
}

SGR::SgrObject& SGR::findObjectByName(std::string name)
{
	for (size_t i = 0; i < objects.size(); i++) {
		if (objects[i].name == name)
			return objects[i];
	}

	return objects[0];
}

SGR::SgrObjectInstance& SGR::findInstanceByName(std::string name)
{
	for (size_t i = 0; i < instances.size(); i++) {
		if (instances[i].name == name)
			return instances[i];
	}

	return instances[0];
}

SgrErrCode SGR::setupGlobalUniformBufferObject(SgrBuffer* uboBuffer)
{
	UBO = uboBuffer;
	return sgrOK;
}

void SGR::unbindAllMeshesAndPiplines()
{
	for (size_t i = 0; i < objects.size(); i++)
		objects[i].meshDataAndPiplineBinded = false;
}

SgrErrCode SGR::drawObject(std::string instanceName)
{
	SgrObjectInstance& instance = findInstanceByName(instanceName);
	if (instance.name == "empty") 
		return sgrMissingInstance;

	SgrObject& objectToDraw = findObjectByName(instance.geometry);
	if (objectToDraw.name == "empty")
		return sgrMissingObject;

	PipelineManager::SgrPipeline* objectPipeline = pipelineManager->instance->getPipelineByName(instance.geometry);
	if (objectPipeline->name == "empty")
		return sgrMissingPipeline;

	DescriptorManager::SgrDescriptorSets descrSets = descriptorManager->getDescriptorSetsByName(instanceName);
	if (descrSets.name == "empty")
		return sgrMissingDescriptorSets;

	instance.needToDraw = true;

	return sgrOK;
}

SgrErrCode SGR::updateInstancesUniformBufferObject(SgrInstancesUniformBufferObject dynUBO)
{ 
	VkDevice device = logicalDeviceManager->instance->logicalDevice;

	MemoryManager::copyDataToBuffer(dynamicUBO, dynUBO.data);

	VkMappedMemoryRange mappedMemoryRange{};
	mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedMemoryRange.memory = dynamicUBO->bufferMemory;
	mappedMemoryRange.size = dynamicUBO->size;
	vkFlushMappedMemoryRanges(device, 1, &mappedMemoryRange);
	return sgrOK;
}

SgrErrCode SGR::updateGlobalUniformBufferObject(SgrGlobalUniformBufferObject obj)
{
	MemoryManager::copyDataToBuffer(UBO, &obj);
	return sgrOK;
}

SgrErrCode SGR::writeDescriptorSets(std::string name, std::vector<void*> data)
{
	std::string geometry = findInstanceByName(name).geometry;
	return descriptorManager->updateDescriptorSets(name, descriptorManager->getDescriptorInfoByName(geometry).name, data);
}

float SGR::getSgrTimeDuration(SgrTime_t start, SgrTime_t end)
{
	return std::chrono::duration<float, std::chrono::seconds::period>(end - start).count();
}

bool SGR::setFPSDesired(uint8_t fps)
{
	if (fps == 0)
		return false;

	fpsDesired = fps;
	return true;
}

SgrErrCode SGR::buildDrawingCommands()
{
	for (size_t i = 0; i < instances.size(); i++) {
		const SgrObjectInstance& instance = instances[i];
		if (instance.name == "empty") 
			continue;

		if (!instance.needToDraw)
			continue;

		SgrObject& objectToDraw = findObjectByName(instance.geometry);
		if (objectToDraw.name == "empty")
			return sgrMissingObject;

		PipelineManager::SgrPipeline* objectPipeline = pipelineManager->instance->getPipelineByName(instance.geometry);
		if (objectPipeline->name == "empty")
			return sgrMissingPipeline;

		if (!objectToDraw.meshDataAndPiplineBinded) {
			commandManager->bindPipeline(&objectPipeline->pipeline);
			std::vector<VkBuffer> vertices{ objectToDraw.vertices->vkBuffer };
			commandManager->bindVertexBuffer(vertices);
			commandManager->bindIndexBuffer(objectToDraw.indices->vkBuffer);
			objectToDraw.meshDataAndPiplineBinded = true;
		}

		DescriptorManager::SgrDescriptorSets descrSets = descriptorManager->getDescriptorSetsByName(instance.name);
		if (descrSets.name == "empty")
			return sgrMissingDescriptorSets;

		std::vector<uint32_t> dynamicOffset = { static_cast<uint32_t>(instance.uboDataAlignment) };

		for (size_t i = 0; i < commandManager->commandBuffers.size(); i++)
			commandManager->bindDescriptorSet(&objectPipeline->pipelineLayout, static_cast<uint8_t>(i), descrSets.descriptorSets[i], 0, 1, dynamicOffset);

		commandManager->drawIndexed(objectToDraw.indicesCount, 1, 0, 0, 0);
	}

	return sgrOK;
}

SgrErrCode SGR::getWindow(GLFWwindow* &ptr)
{
	if (!window)
		return sgrInitWindowError;

	ptr = window;
	return sgrOK;
}