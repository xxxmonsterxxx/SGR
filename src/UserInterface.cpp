#include "UserInterface.h"

#include "PhysicalDeviceManager.h"
#include "LogicalDeviceManager.h"
#include "DescriptorManager.h"
#include "RenderPassManager.h"
#include "CommandManager.h"

#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_glfw.h"

UIManager* UIManager::_instance = nullptr;

UIManager* UIManager::get()
{
    if (!_instance)
        _instance = new UIManager;

    return _instance;
}

SgrErrCode UIManager::init(GLFWwindow* window, VkInstance instance, std::vector<VkQueueFlagBits> queueFams, uint8_t imageCount)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    _window = window;
    ImGui_ImplGlfw_InitForVulkan(_window, true);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = instance;
    init_info.PhysicalDevice = PhysicalDeviceManager::instance->pickedPhysicalDevice.vkPhysDevice;
    init_info.Device = LogicalDeviceManager::instance->logicalDevice;
    init_info.QueueFamily = queueFams[0];
    init_info.Queue = LogicalDeviceManager::instance->graphicsQueue;
    if (DescriptorManager::instance->createDescriptorPoolForUI() == sgrDescriptorPoolCreateError)
        return sgrDescriptorPoolCreateError;
    init_info.DescriptorPool = DescriptorManager::instance->uiDescriptorPool;
    init_info.RenderPass = RenderPassManager::instance->renderPass; 
    init_info.MinImageCount = imageCount;
    init_info.ImageCount = imageCount;

    ImGui_ImplVulkan_Init(&init_info);

    VkCommandBuffer commandBuffer = CommandManager::instance->beginSingleTimeCommands();
    ImGui_ImplVulkan_CreateFontsTexture();
    CommandManager::instance->endSingleTimeCommands(commandBuffer);

    vkDeviceWaitIdle(LogicalDeviceManager::instance->logicalDevice);
    ImGui_ImplVulkan_DestroyFontsTexture();
    
    return sgrOK;
}

void UIManager::uiRender()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    for (const auto& el : elementsToDraw)
        el->draw();

    ImGui::Render();
    ImDrawData* data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(data, CommandManager::instance->commandBuffers[0]);
    ImGui_ImplVulkan_RenderDrawData(data, CommandManager::instance->commandBuffers[1]);
    ImGui_ImplVulkan_RenderDrawData(data, CommandManager::instance->commandBuffers[2]);
}

SgrErrCode UIManager::destroy()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return sgrOK;
}

SgrErrCode UIManager::drawElement(SgrUIElement& element)
{
    elementsToDraw.push_back(&element);
    return sgrOK;
}

SgrUIElement::SgrUIElement(std::string name, glm::vec2 pos) : _name(name), _position(pos)
{
}

void SgrUIElement::show(bool visible)
{
    _visible = visible;
}

void SgrUIElement::beginElement()
{
    ImGui::SetNextWindowSize(ImVec2(0, 0));
    ImGui::Begin("Invisible", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground |
                                       ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
}

void SgrUIElement::setPos(glm::vec2 pos)
{
    ImVec2 windowSize = ImGui::GetIO().DisplaySize;
    ImGui::SetCursorScreenPos({_position.x * windowSize.x, _position.y * windowSize.y});
}

SgrUIButton::SgrUIButton(std::string name, glm::vec2 pos, void (*callbackFunc)(), std::string text) :
            SgrUIElement(name, pos), _text(text)
{
    buttonFunction = callbackFunc;
}

void SgrUIButton::draw()
{
    if (!_visible)
        return;
    beginElement();

    setPos(_position);
    if (ImGui::Button(_text.c_str())) {
        buttonFunction();
    }

    ImGui::End(); // require after each
}

void SgrUIButton::changeText(std::string newText)
{
    _text = newText;
}

SgrUIText::SgrUIText(std::string name, glm::vec2 pos, std::string text) :
            SgrUIElement(name, pos), _text(text)
{
}

void SgrUIText::draw()
{
    if (!_visible)
        return;
    beginElement();

    setPos(_position);
    ImGui::Text("%s", _text.c_str());

    ImGui::End(); // require after each
}

void SgrUIText::changeText(std::string newText)
{
    _text = newText;
}
