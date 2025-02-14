#pragma once

#include "utils.h"

class UIManager;

class SgrUIElement {
    friend UIManager;
public:
    SgrUIElement(std::string name, glm::vec2 pos);
    virtual ~SgrUIElement() = default;

    glm::vec2 _position{ 0.5,0.5 };
    glm::vec2 _size{ 50,50 };
    bool _visible = true;

    virtual void beginElement();

protected:
    std::string _name = "Name";
    
    virtual void draw() = 0;
    virtual void setPos(glm::vec2 pos);
};

class SgrUIButton : public SgrUIElement {
public:
    SgrUIButton(std::string name, glm::vec2 pos, void (*callbackFunc)(), std::string text=std::string("Button"));
    ~SgrUIButton() {;}

    void changeText(std::string newText);
    
private:
    std::string _text{};
    void (*buttonFunction)() = nullptr;

protected:
    void draw() override;
};

class SgrUIText : public SgrUIElement {
public:
    SgrUIText(std::string name, glm::vec2 pos, std::string text=std::string());
    ~SgrUIText(){;}

    void changeText(std::string newText);
private:
    std::string _text{};

protected:
    void draw() override;
};

class CommandManager;

class UIManager {
    friend CommandManager;
public:
    static UIManager* get();
    SgrErrCode init(GLFWwindow* window, VkInstance instance, uint8_t imageCount);
    SgrErrCode destroy();

    SgrErrCode drawElement(SgrUIElement& element);

    void uiRender();
    void setupUICallback();
private:
    static UIManager* _instance;

    GLFWwindow* _window = nullptr;

    std::vector<SgrUIElement*> elementsToDraw{};

    UIManager() {;}
    UIManager(const UIManager&) = delete;
    UIManager operator=(UIManager&) = delete;
};
