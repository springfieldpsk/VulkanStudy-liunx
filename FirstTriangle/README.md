# Frist Triangle

## Instacnce

### Create an instance 创建实例

Vulkan中的许多结构要求显式指定sType成员中的类型,将作为`pNext`的成员以指向扩展信息,使用值初始化时将其保留为`nullptr`.

Vulkan中的信息通过`struct`传递信息以代替`function`,通过填充结构体以为创建新实例提供信息

```cpp
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Triangle Build demo";
    appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
```

以`struct`形式创建app信息

```cpp
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
```

以上结构不是可选的，用于告诉Vulkan驱动我们选择的全局扩展及验证层,全局意味着适用于整个程序而不是指定设备

```cpp
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    // 通过GLFW内置函数获取扩展
    
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
```

以上的几行用于指定全局扩展，由于Vulkan是一个平台无关的API，这意味着您需要一个扩展来与系统接口,这里使用GLFW中的内置函数以返回扩展，并传递至结构体

```cpp
    createInfo.enabledLayerCount = 0;
    // 验证层指定，由于暂时没有用到因此置零

    VkResult result = vkCreateInstance(&createInfo,nullptr,&instance);
    // 通过struct 创建Vk实例
```

最终创建VK实例

在Vk中，对象构造函数参数遵循的一般模式为

- 指向具有创建信息的结构的指针
- 指向自定义分配器回调的指针（在此为`nullptr`）
- 指向存储新对象句柄的变量的指针

若构造成功，则实例句柄存储于`Vkinstance`类成员中

几乎所有的Vulkan函数都返回一个`VkResult`型的值，该值要么为`VK_SUCCESS`或一个错误码,通过确认result值来确认创建实例是否成功

在`vkCreateInstance`中存在一个错误码`VK_ERROR_EXTENSION_NOT_PRESENT`,通过这种方式来确认扩展是否支持。

可以通过`vkEnumerateInstanceExtensionProperties`来检索受支持扩展的列表，该函数使用一个存储扩展数的指针与存储扩展详细信息的`VkExtensionProperties`数组，同时还有一个可选的第一个参数，允许我们按照特定的验证层过滤扩展

```cpp
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    // 通过留空获取扩展数量

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr,&extensionCount,extensions.data());
    // 获取扩展详细信息
```

在每个`VkExtensionProperties` 中，包含扩展的名字和版本号，可以通过循环得知这些信息,可以将这些信息与通过`glfwGetRequiredInstanceExtensions`得到的信息相比较

### Cleaning up 清理实例

清理 Vk 实例的唯一机会在程序结束之前，通过`vkDestroyInstance`实现

第一个参数为vk实例，第二个参数是一个可选参数，可以通过传递`nullptr`来忽略，其他的vk资源需要在vk实例清理钱销毁

## Validation layers 验证层

### 前言

由于Vulkan是围绕最小化驱动开销的思想设计的，因此对错误的检查十分有限，所以要求编写者明确自己所做的任何事

当然，较少的错误检查不意味着不检查，通过可选的验证层，来检验错误，常见操作有：

- 根据规范检查参数值以确定是否误用
- 跟踪对象的创建与销毁以查找资源泄露
- 跟踪调用线程以检查线程安全性
- 将调用与参数记录至标准输出
- 跟踪Vulkan调用以分析与重放

例:

```cpp
VkResult vkCreateInstance(
    const VkInstanceCreateInfo* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkInstance* instance) {

    if (pCreateInfo == nullptr || instance == nullptr) {
        log("Null pointer passed to required parameter!");
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    return real_vkCreateInstance(pCreateInfo, pAllocator, instance);
}
```

Vulkan没有任意内置的验证层，LunarG Vulkan SDK 提供了一组很好的层，可以检测常见错误，验证层只有在安装在系统时才能使用

以前 Vulkan 中有两种不同类型的验证层: 实例验证层和设备验证层。这个想法是，实例层只检查与全局 Vulkan 对象相关的调用，比如实例，而设备特定层只检查与特定 GPU 相关的调用。设备特定的层现在已被弃用，这意味着实例验证层应用于所有 Vulkan 调用。规范文档仍然建议您在设备级别启用验证层以实现兼容性，这是某些实现所要求的。我们只需在逻辑设备级别将相同的层指定为实例，稍后我们将看到这一点。

### 使用验证层

这节的目的在于了解如何启用 Vulkan SDK 提供的标准诊断层，需要通过指定验证层的名称来启用验证层，所有标准验证捆绑在一个包含在SDK中的层中，称为`VK_LAYER_KHRONOS_validation`

首先向程序添加两个配置变量以确定启用验证层的目标与是否启用验证层，使用宏来实现确定当前模式

```cpp
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    // 启用 Vulkan SDK 标准诊断层

    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif
```

其次添加一个新的函数`checkValidationLayerSupport`,以确定所有的请求层是否可用，使用`vkEnumerateInstanceExtensionProperties`列出所有可用层，用法与实例与`vkEnumerateInstanceExtensionProperties `相同

```cpp
    bool checkValidationLayerSupport(){
        uint32_t layerCount ;
        vkEnumerateInstanceLayerProperties(&layerCount,nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount,availableLayers.data());
        return false;
    }
    // 确定所有的验证请求层是否可用
```

接下来验证`validationLayers`中的所有层是否存在于`availableelayers` 列表中

```cpp
bool checkValidationLayerSupport(){
        uint32_t layerCount ;
        vkEnumerateInstanceLayerProperties(&layerCount,nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount,availableLayers.data());

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
```
