#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <bits/stdc++.h>
#include <GLFW/glfw3.h>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};
// 启用 Vulkan SDK 标准诊断层

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

class HelloTriangleApplication {
public:
    void run() {
        initVulkan();
        initWindow();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow *  window;
    VkInstance instance; //vulkan 实例

    VkDebugUtilsMessengerEXT debugMessenger;
    void initVulkan() {
        createInstance();
        setupDebugMessenger();
    }

    void initWindow(){
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // 标注非opengl
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        // 禁用调整窗口大小

        window = glfwCreateWindow(WIDTH,HEIGHT,"vulkan",nullptr,nullptr);
        // 创建一个 glfw 窗口
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)){
            glfwPollEvents();
        }
        
    }

    void cleanup() {
        vkDestroyInstance(instance, nullptr);
        // 清理 vk 实例

        glfwDestroyWindow(window);
        // 清理窗口

        glfwTerminate();
        // 关闭glfw
    }

    bool checkValidationLayerSupport(){
        uint32_t layerCount ;
        vkEnumerateInstanceLayerProperties(&layerCount,nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount,availableLayers.data());
        // std::cout<<layerCount<<'\n';
        for(const char* layerName : validationLayers){
            bool layerFound = false;

            for(const auto& layerProperties : availableLayers){
                if(strcmp(layerName,layerProperties.layerName) == 0){
                    layerFound = true;
                    break;
                }
            }

            if(!layerFound){
                return false;
            }
        }

        return true;
    }
    // 确定所有的验证请求层是否可用

    std::vector<const char*> getRequiredExtensions(){
        uint32_t glfwExtensionCount = 0;
        const char ** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*>extensions(glfwExtensions,glfwExtensions + glfwExtensionCount);

        if(enableValidationLayers){
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }
    // 根据验证层是否启用来返回所需扩展列表

    void createInstance(){
        if(enableValidationLayers && !checkValidationLayerSupport()){
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Triangle Build demo";
        appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
    
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        // 以上结构不是可选的，用于告诉Vulkan驱动我们选择的全局扩展及验证层
        // 全局意味着适用于整个程序而不是指定设备

        // 接下来的几行用于指定全局扩展，由于Vulkan是一个平台无关的API，这意味着您需要一个扩展来与系统接口
        // 这里使用GLFW中的内置函数以返回扩展，并传递至结构体
        
        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        if(enableValidationLayers){
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
            // 启用验证层
        }
        else {
            createInfo.enabledLayerCount = 0;
            // 禁用下置零
        }

        // 通过struct 创建Vk实例
        if(vkCreateInstance(&createInfo,nullptr,&instance) != VK_SUCCESS){
            throw std::runtime_error("failed to create instance");
            // 若返回result不为VK_SUCCESS 则抛出错误
        }

    }
    // 创建实例，向驱动程序提供信息以对特定应用优化

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData){

            if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT){
                // Message is important enough to show
            }
            std::cerr << "validation layer: "<< pCallbackData->pMessage << std::endl;
            return VK_FALSE;
    }

    // 使用 PFN_vkDebugUtilsMessengerCallbackEXT 的原型添加一个新的静态成员函数，VKAPI_ATTR 与 VKAPI_CALL 确保这个函数具有Vulkan调用它的正确签名 

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo){
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;
    }
    // 填充有关消息管理器和回调的细节
    
    void setupDebugMessenger(){
        if(!enableValidationLayers) return;
    }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}