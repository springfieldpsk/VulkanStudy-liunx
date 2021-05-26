#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <GLFW/glfw3.h>

class HelloTriangleApplication {
public:
    void run() {
        initVulkan();
        initWindow();
        mainLoop();
        cleanup();
    }

private:
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    GLFWwindow *  window;
    VkInstance instance; //vulkan 实例

    void createInstance(){
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
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        // 通过GLFW内置函数获取扩展
        
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        createInfo.enabledLayerCount = 0;
        // 验证层指定，由于暂时没有用到因此置零

        // 通过struct 创建Vk实例
        if(vkCreateInstance(&createInfo,nullptr,&instance) != VK_SUCCESS){
            throw std::runtime_error("failed to create instance");
            // 若返回result不为VK_SUCCESS 则抛出错误
        }
    }
    // 创建实例，向驱动程序提供信息以对特定应用优化
    void initVulkan() {
        createInstance();
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
        glfwDestroyWindow(window);
        // 清理窗口

        glfwTerminate();
        // 关闭glfw
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