Vulkan中的许多结构要求显式指定sType成员中的类型,将作为`pNext`的成员以指向扩展信息,使用值初始化时将其保留为`nullptr`.

Vulkan中的信息通过`struct`传递信息以代替`function`,通过填充结构体以为创建新实例提供信息

```
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Triangle Build demo";
    appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
```

以`struct`形式创建app信息

```
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
```

以上结构不是可选的，用于告诉Vulkan驱动我们选择的全局扩展及验证层,全局意味着适用于整个程序而不是指定设备

```
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    // 通过GLFW内置函数获取扩展
    
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
```

以上的几行用于指定全局扩展，由于Vulkan是一个平台无关的API，这意味着您需要一个扩展来与系统接口,这里使用GLFW中的内置函数以返回扩展，并传递至结构体

```
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


