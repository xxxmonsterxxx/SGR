#include "SGR.h"

#include "ShaderManager.h"
#include "PipelineManager.h"

SGR::SGR(std::string appName, uint8_t appVersionMajor, uint8_t appVersionMinor)
{
	manualWindow = false;
	sgrRunning = false;
	window = nullptr;
	applicationName = "Simple graphic application";
	appVersionMajor = 1;
	appVersionMinor = 0;
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

	return sgrOK;
}

SgrErrCode SGR::destroy()
{
	vkDeviceWaitIdle(logicalDeviceManager->logicalDevice);
	windowManager->destroy();
	glfwTerminate();

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

SgrErrCode SGR::drawFrame()
{
	if (!commandManager->buffersEnded)
		commandManager->endInitCommandBuffers();

	if (windowManager->windowMinimized)
		glfwWaitEvents();

	vkWaitForFences(logicalDeviceManager->logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkDevice device = logicalDeviceManager->logicalDevice;
	VkSwapchainKHR swapChain = swapChainManager->swapChain;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		SgrErrCode reinitSwapChain = swapChainManager->reinitSwapChain();
		if (reinitSwapChain != sgrOK) {
			return reinitSwapChain;
		}
		imagesInFlight.resize(swapChainManager->imageCount, VK_NULL_HANDLE);
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		return sgrFailedToAcquireImage;
	}

	updateUniformBuffer();

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
		swapChainManager->reinitSwapChain();
	}
	else if (result != VK_SUCCESS) {
		return sgrFailedPresentImage;
	}

	currentFrame = (currentFrame + 1) % maxFrameInFlight;

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
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "Simple Graphic Renderer";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
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

SgrErrCode SGR::addToFrameSimpleTestObject()
{
	return sgrOK;
}

SgrErrCode SGR::setRenderPhysicalDevice(SgrPhysicalDevice sgrDevice)
{
	if (std::find(physicalDeviceManager->physicalDevices.begin(),
		physicalDeviceManager->physicalDevices.end(), sgrDevice) == physicalDeviceManager->physicalDevices.end())
		return sgrGPUNotFound;

	physicalDeviceManager->pickedPhysicalDevice = sgrDevice;
	return sgrOK;
}

SgrErrCode SGR::addNewTypeObject(std::string name, std::vector<Sgr2DVertex> vertices, std::vector<uint16_t> indices,
								std::string shaderVert, std::string shaderFrag, std::vector<VkDescriptorSetLayoutBinding> setLayoutBinding,
								std::vector<VkVertexInputBindingDescription> bindingDescriptions,
								std::vector<VkVertexInputAttributeDescription> attributDescrtions)
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
	size = sizeof(indices[0]) * indices.size();
	newObject.indices = nullptr;
	resultAllocateMemoryBuffer = memoryManager->createIndexBuffer(newObject.indices, size, indices.data());
	if (resultAllocateMemoryBuffer != sgrOK)
		return resultAllocateMemoryBuffer;


	SgrErrCode initShaderResult = shaderManager->createShaders(name, shaderVert, shaderFrag);
	if (initShaderResult != sgrOK)
		return initShaderResult;

	ShaderManager::SgrShader objectShaders = shaderManager->getShadersByName(name);

	DescriptorManager::SgrDescriptorInfo newDescriptorInfo;
	newDescriptorInfo.name = name;
	newDescriptorInfo.vertexBindingDescr = bindingDescriptions;
	newDescriptorInfo.vertexAttributeDescr = attributDescrtions;
	newDescriptorInfo.setLayoutBinding = setLayoutBinding;
	descriptorManager->addNewDescriptorInfo(newDescriptorInfo);

	pipelineManager->createPipeline(name, RenderPassManager::instance->renderPass, objectShaders, newDescriptorInfo);

	objects.push_back(newObject);

	return sgrOK;
}

SGR::SgrObject& SGR::findObjectByName(std::string name)
{
	for (size_t i = 0; i < objects.size(); i++) {
		if (objects[i].name == name)
			return objects[i];
	}
}

SgrErrCode SGR::setupUniformBuffers(std::string name, SgrBuffer* uboBuffer, SgrBuffer* instanceUBO)
{
	SgrObject& objectToDraw = findObjectByName(name);

	objectToDraw.dynamicUBO = instanceUBO;

	return sgrOK;
}

SgrErrCode SGR::drawObject(std::string objName, uint16_t count)
{
	SgrObject objectToDraw = findObjectByName(objName);

	/*objectToDraw.dynamicUniformBuffer.addOneElement();*/

	for (uint16_t i = 0; i < count; i++) {
		PipelineManager::SgrPipeline objectPipeline = pipelineManager->instance->getPipelineByName(objName);
		commandManager->bindPipeline(objectPipeline.pipeline);
		std::vector<VkBuffer> vertices{ objectToDraw.vertices->vkBuffer };
		commandManager->bindVertexBuffer(vertices);
		commandManager->bindIndexBuffer(objectToDraw.indices->vkBuffer);

		DescriptorManager::SgrDescriptorInfo descrInfo = descriptorManager->getDescriptorInfoByName(objName);
		std::vector<uint32_t> dynamicOffset = { i * static_cast<uint32_t>(objectToDraw.dynamicAlignment) };

		for (size_t i = 0; i < commandManager->commandBuffers.size(); i++)
			commandManager->bindDescriptorSet(objectPipeline.pipelineLayout, i, descrInfo.descriptorSets[i], 0, 1, dynamicOffset);

		commandManager->drawIndexed(6, 1, 0, 0, 0);
	}

	return sgrOK;
}

SgrErrCode SGR::updateDynamicUniformBuffer(std::string objName, SgrDynamicUniformBufferObject dynamicUBO)
{
	SgrObject& object = findObjectByName(objName);
	object.dynamicAlignment = dynamicUBO.dynamicAlignment;
	VkDevice device = logicalDeviceManager->instance->logicalDevice;

	void* tempDataPointer;
	vkMapMemory(device, object.dynamicUBO->bufferMemory, 0, object.dynamicUBO->size, 0, &tempDataPointer);
	memcpy(tempDataPointer, dynamicUBO.data, object.dynamicUBO->size);

	VkMappedMemoryRange mappedMemoryRange{};
	mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedMemoryRange.memory = object.dynamicUBO->bufferMemory;
	mappedMemoryRange.size = object.dynamicUBO->size;
	vkFlushMappedMemoryRanges(device, 1, &mappedMemoryRange);
	vkUnmapMemory(device, object.dynamicUBO->bufferMemory);
	return sgrOK;
}

SgrErrCode SGR::updateUniformBuffer(std::string objName, UniformBufferObject obj)
{
	SgrObject object = findObjectByName(objName);
	return sgrOK;
}

SgrErrCode SGR::updateDescriptorSets(std::string name, std::vector<void*> data)
{
	return descriptorManager->updateDescriptorSets(name, data);
}
