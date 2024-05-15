/*
    Go to bigvulkan_init() to see the order in which the primary functions are called.
    Those functions are defined in descending order, with their dependents defined directly
    above with no spaces.
    Primary function "groups" that include the funcation and its dependents are seperated by newlines.
*/
#include "bigvulkan.h"
#include "SDL_joystick.h"
#include "types.h"
#include "SDL/window.h"
#include <SDL_platform.h>
#include <SDL_video.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan_core.h>
#include <stdio.h>
#include <string.h>

#include "report.h"
#include "utils.h"

const u8 MAX_FRAMES_IN_FLIGHT = 2;
u32 currentFrame = 0;

VkInstance instance;

VkDebugUtilsMessengerEXT debugMessenger;

VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

// vulkan logical device
VkDevice device;


VkQueue graphicsQueue;
VkQueue presentQueue;

VkSurfaceKHR surface;

VkSwapchainKHR swapChain;

VkImage *swapChainImages;
u32 swapChainImageCount;
VkFormat swapChainImageFormat;
VkExtent2D swapChainExtent;

VkImageView *swapChainImageViews;
u32 swapChainImageViewCount;

VkRenderPass renderPass;
VkPipelineLayout pipelineLayout;

VkPipeline graphicsPipeline;

VkFramebuffer *swapChainFramebuffers;
u32 swapChainFramebufferCount;

VkCommandPool commandPool;

// following globals are made arrays because each frame in flight needs its own
VkCommandBuffer *commandBuffers;
u32 commandBufferCount;

VkSemaphore *imageAvailableSemaphores;
u32 imageAvailableSemaphoreCount;

VkSemaphore *renderFinishedSemaphores;
u32 renderFinishedSemaphoreCount;

VkFence *inFlightFences;
u32 inFlightFenceCount;

// instance extensions
u32 extensionCount;
const char **extensionNames;

#ifdef DEBUG
const b8 enableValidationLayers = TRUE;
#else
const b8 enableValidationLayers = FALSE;
#endif

#define VALIDATION_LAYERS_COUNT 1
const char* validationLayers[VALIDATION_LAYERS_COUNT] = {"VK_LAYER_KHRONOS_validation"};

#define DEVICE_EXTENSIONS_COUNT 1
const char* deviceExtensions[DEVICE_EXTENSIONS_COUNT] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#define QUEUE_FAMILY_COUNT 2

typedef struct QueueFamilyIndices {
    optional_u32 graphicsFamily;
    optional_u32 presentFamily;
} QueueFamilyIndices;

typedef struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    u32 formatCount;
    VkSurfaceFormatKHR* formats;
    u32 presentModeCount;
    VkPresentModeKHR* presentModes;
} SwapChainSupportDetails;

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT *createInfo);

b8 checkVLayerSupport() {
    u32 layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    VkLayerProperties availableLayers[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    for(int i = 0; i < VALIDATION_LAYERS_COUNT; i++) {
        const char *layerName = validationLayers[i];
        b8 layerFound = FALSE;

        for(int j = 0; j < layerCount; j++) {
            if(strcmp(layerName, availableLayers[j].layerName) == 0) {
                layerFound = TRUE;
                break;
            }
        }

        if(!layerFound) {
            return FALSE;
        }

    }
    return TRUE;
}
void createInstance() {
    if(enableValidationLayers && !checkVLayerSupport()) {
        report_error("createInstance()","requested validation layers are not available!");
    }
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "vulkan3D";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.pEngineName = "vulkan3D";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {0};
    createInfo.enabledLayerCount = 0;
    if(enableValidationLayers) {
        createInfo.enabledLayerCount = VALIDATION_LAYERS_COUNT;
        createInfo.ppEnabledLayerNames = validationLayers;

        // used for message callback
        extensionNames[extensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
        extensionCount++;

        populateDebugMessengerCreateInfo(&debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    }

    // needed for vulkan to not throw a fit on mac
    const char *platform = SDL_GetPlatform();
    if(strcmp(platform, "Mac OS X") == 0) {
        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
        // remember, extensionCount is the total elements and we're indexing into the next one
        extensionNames[extensionCount] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
        extensionCount++;
        extensionNames[extensionCount] = VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME;
        extensionCount++;
    }

    // pass in our parameters
    createInfo.enabledExtensionCount = extensionCount;
    createInfo.ppEnabledExtensionNames = extensionNames;


    VkResult result = vkCreateInstance(&createInfo, NULL, &instance);
    if(result != VK_SUCCESS) {
        report_fatal("createInstance()", "vkCreateInstance() was not successful");
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    switch(messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            report_info("Vulkan Callback", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            report_warning("Vulkan Callback", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            report_error("Vulkan Callback", pCallbackData->pMessage);
            break;
        default: // there's one more enum value that's not used i think
            break;
    }

    return VK_FALSE;
}
VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, 
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, 
    VkDebugUtilsMessengerEXT* pDebugMessenger) {

    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}
void DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator) {
    
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debugMessenger, pAllocator);
    }
}
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT *createInfo)  {
    createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | 
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo->pfnUserCallback = debugCallback;
    createInfo->pNext = NULL;
    createInfo->flags = 0;

}
void setupDebugMessenger() {
    if(!enableValidationLayers) {return;}

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(&createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, NULL, &debugMessenger) != VK_SUCCESS) {
        report_warning("setupDebugMessenger()", "failed to set up debug messenger!");
    }


    
}

void createSurface() {
    report_info("createSurface()","current extensions:");
    for(int i = 0; i < extensionCount; i++) {
        report_info("createSurface()","\t%s", extensionNames[i]);
    }

    SDL_bool result = SDL_Vulkan_CreateSurface(window_getMain(), instance, &surface);
    if(result == SDL_FALSE) {
        // promote to a fatal later
        report_error("createSurface()", "SDL_Vulkan_CreateSurface() failed, reason: %s", SDL_GetError());
    }

}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    indices.graphicsFamily.hasValue = FALSE;
    indices.graphicsFamily.hasValue = FALSE;

    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
    VkQueueFamilyProperties queueFamilies[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    VkBool32 presentSupport = FALSE;
    for(int i = 0; i < queueFamilyCount; i++) {
        if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily.data = i;
            indices.graphicsFamily.hasValue = TRUE;
        }
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if(presentSupport) {
            indices.presentFamily.data = i;
            indices.presentFamily.hasValue = TRUE;
        }

        if(indices.graphicsFamily.hasValue && indices.presentFamily.hasValue) {
            break;
        }
    }



    return indices;
}
b8 checkDeviceExtensionSupport(VkPhysicalDevice device) {
    u32 devExtensionCount;
    vkEnumerateDeviceExtensionProperties(device, NULL, &devExtensionCount, NULL);
    VkExtensionProperties availableExtensions[devExtensionCount];

    vkEnumerateDeviceExtensionProperties(device, NULL, &devExtensionCount, availableExtensions);
    for(int i = 0; i < DEVICE_EXTENSIONS_COUNT; i++) {
        const char *extensionName = deviceExtensions[i];
        b8 extensionFound = FALSE;

        for(int j = 0; j < devExtensionCount; j++) {
            if(strcmp(extensionName, availableExtensions[j].extensionName) == 0) {
                extensionFound = TRUE;
                break;
            }
        }

        if(!extensionFound) {
            return FALSE;
        }

    }
    return TRUE;
    
}
SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    VkResult result;

    details.formatCount = 0;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formatCount, NULL);
    VkSurfaceFormatKHR formatsArr[details.formatCount];

    result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formatCount, formatsArr);
    details.formats = formatsArr;

    details.presentModeCount = 0;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.presentModeCount, NULL);
    VkPresentModeKHR modesArr[details.presentModeCount];
 
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.presentModeCount, modesArr);
    details.presentModes = modesArr;

 


    return details;
}
b8 isDeviceSuitable(VkPhysicalDevice device) {
    QueueFamilyIndices indices = findQueueFamilies(device);

    b8 extensionsSupported = checkDeviceExtensionSupport(device);

    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
    b8 swapChainGood = swapChainSupport.formatCount >= 1 && swapChainSupport.presentModeCount >= 1;

    return indices.graphicsFamily.hasValue && indices.presentFamily.hasValue && extensionsSupported && swapChainGood;

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);


    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           deviceFeatures.geometryShader;
}
void pickPhysicalDevice() {
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    if(deviceCount == 0) {
        report_fatal("pickPhysicalDevice()", "failed to find a GPU with vulkan support!");
        return;
    }

    VkPhysicalDevice devices[deviceCount];
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);
    for(int i = 0; i < deviceCount; i++) {
        if(isDeviceSuitable(devices[i])) {
            physicalDevice = devices[i];
            break;
        }
    }
    if(physicalDevice == VK_NULL_HANDLE) {
        report_fatal("pickPhysicalDevice()", "couldn't find a GPU with suitable features");
    }

}

void createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    u32 uniqueQueueFamilies[QUEUE_FAMILY_COUNT] = {indices.graphicsFamily.data, indices.presentFamily.data};
    VkDeviceQueueCreateInfo queueCreateInfos[QUEUE_FAMILY_COUNT] = {0};
    f32 queuePriority = 1.0f;

    for(int i = 0; i < QUEUE_FAMILY_COUNT; i++) {
        queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[i].queueFamilyIndex = uniqueQueueFamilies[i];
        queueCreateInfos[i].queueCount = 1;
        queueCreateInfos[i].pQueuePriorities = &queuePriority;
    }


    VkPhysicalDeviceFeatures deviceFeatures = {VK_FALSE};

    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos;
    /*
        hack: both of the queue create infos have the same data and the spec doesn't like that they share incidies so
        we just tell this struct there's only one
    */
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;


    createInfo.enabledExtensionCount = DEVICE_EXTENSIONS_COUNT;
    createInfo.ppEnabledExtensionNames = deviceExtensions;


    // needed for vulkan to not throw a fit on mac
    if(strcmp(SDL_GetPlatform(), "Mac OS X") == 0) {
        const char *neoDeviceExtensions[DEVICE_EXTENSIONS_COUNT+1];
        for(int i = 0; i < DEVICE_EXTENSIONS_COUNT; i++) {
            neoDeviceExtensions[i] = deviceExtensions[i];
        }
        neoDeviceExtensions[DEVICE_EXTENSIONS_COUNT] = "VK_KHR_portability_subset";
        createInfo.enabledExtensionCount = DEVICE_EXTENSIONS_COUNT+1;
        createInfo.ppEnabledExtensionNames = neoDeviceExtensions;

    }


    createInfo.enabledLayerCount = 0;
    // device-specific validation layers are deprecated, but we enable them anyway
    if(enableValidationLayers) {
        createInfo.enabledLayerCount = VALIDATION_LAYERS_COUNT;
        createInfo.ppEnabledLayerNames = validationLayers;
    }

    VkResult result = vkCreateDevice(physicalDevice, &createInfo, NULL, &device);
    if(result != VK_SUCCESS) {
        report_fatal("createLogicalDevice()", "vkCreateDevice was not successful");
    }

    vkGetDeviceQueue(device, indices.graphicsFamily.data, 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.data, 0, &presentQueue);



}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(VkSurfaceFormatKHR *availableFormats, u32 formatCount) {
    for(int i = 0; i < formatCount; i++) {
        VkSurfaceFormatKHR availableFormat = availableFormats[i];
        if(availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0];
}
VkPresentModeKHR chooseSwapPresentMode(VkPresentModeKHR *availablePresentModes, u32 presentModeCount) {
    return VK_PRESENT_MODE_FIFO_KHR; // basically vsync
}
VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR *capabilities) {
    /*
        no hi-dpi support for you, better hope this is the right resolution
        see https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain
    */
    return capabilities->currentExtent;
    
}
void createSwapChain() {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats, swapChainSupport.formatCount);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes, swapChainSupport.presentModeCount);
    VkExtent2D extent = chooseSwapExtent(&swapChainSupport.capabilities);

    /*
        from vulkan-tutorial.com: "[...] simply sticking to this minimum means that we may sometimes have to wait 
        on the driver to complete internal operations before we can acquire another image to render to. 
        Therefore it is recommended to request at least one more image than the minimum"
    */
    u32 imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    u32 queueFamilyIndices[2] = {indices.graphicsFamily.data, indices.presentFamily.data};
    if(indices.graphicsFamily.hasValue && indices.presentFamily.hasValue) {
        if(indices.graphicsFamily.data != indices.presentFamily.data) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = NULL;
        }
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if(vkCreateSwapchainKHR(device, &createInfo, NULL, &swapChain)!= VK_SUCCESS) {
        report_error("createSwapChain()", "failed to create the swap chain");
    }

    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, NULL);
    swapChainImageCount = imageCount;
    swapChainImages = malloc(sizeof(VkImage) * imageCount);

    vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, swapChainImages);


    // store our stuff for later
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;

}

void createImageViews() {
    swapChainImageViewCount = swapChainImageCount;

    swapChainImageViews = malloc(sizeof(VkImageView) * swapChainImageViewCount);

    for(u8 i = 0; i < swapChainImageCount; i++) {
        VkImage image = swapChainImages[i];
        VkImageViewCreateInfo createInfo = {0};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = image;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if(vkCreateImageView(device, &createInfo, NULL, &swapChainImageViews[i]) != VK_SUCCESS) {
            report_error("createImageViews()", "vkCreateImageView failed for one of the swap images");
        }
    }
}

void createRenderPass() {
    VkAttachmentDescription colorAttachment = {0};
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {0};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency = {0};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo = {0};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if(vkCreateRenderPass(device, &renderPassInfo, NULL, &renderPass) != VK_SUCCESS) {
        report_error("createRenderPass()", "vkCreateRenderPass failed");
    }

}

VkShaderModule createShaderModule(u32 codeSize, void * code) {
    VkShaderModuleCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = codeSize;
    createInfo.pCode = code;
    
    VkShaderModule shaderModule;
    if(vkCreateShaderModule(device, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
        report_error("createShaderModule()", "vkCreateShaderModule failed");
    }
    return shaderModule;
}
void createGraphicsPipeline() { // loading the shaders twice cuz im lazy
    u32 vertCodeSize = 0;
    void *vertCode = utils_readFile("resource/simple_shader.vert.spv", &vertCodeSize, 1);
    u32 fragCodeSize = 0;
    void *fragCode = utils_readFile("resource/simple_shader.frag.spv", &fragCodeSize, 1);


    VkShaderModule vertShaderModule = createShaderModule(vertCodeSize, vertCode);
    VkShaderModule fragShaderModule = createShaderModule(fragCodeSize, fragCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {0};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {0};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamicState = {0};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = NULL;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = NULL;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {0};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {0};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapChainExtent.width;
    viewport.height = (float) swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {0};
    VkOffset2D offset = {0, 0};
    scissor.offset = offset;
    scissor.extent = swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState = {0};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE; // antialiasing
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
                                        VK_COLOR_COMPONENT_G_BIT | 
                                        VK_COLOR_COMPONENT_B_BIT | 
                                        VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {0};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    if(vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &pipelineLayout)!= VK_SUCCESS) {
        report_error("createGraphicsPipeline()", "failed to create pipeline layout!");
    }


    VkGraphicsPipelineCreateInfo pipelineInfo = {0};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = NULL;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;

    if(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline) != VK_SUCCESS) {
        report_error("createGraphicsPipeline()", "vkCreateGraphicsPipelines failed!");
    }

    vkDestroyShaderModule(device, vertShaderModule, NULL);
    vkDestroyShaderModule(device, fragShaderModule, NULL);

    utils_free(vertCode);
    utils_free(fragCode);
}

void createFramebuffers() {
    swapChainFramebufferCount = swapChainImageViewCount;
    swapChainFramebuffers = malloc(sizeof(VkFramebuffer) * swapChainFramebufferCount);

    for(int i = 0; i < swapChainImageViewCount; i++) {
        VkImageView attachments[1] = {swapChainImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo = {0};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if(vkCreateFramebuffer(device, &framebufferInfo, NULL, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            report_error("createFramebuffers()", "one of the framebuffers failed creation");
        }

    }
}

void createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);
    if(!queueFamilyIndices.graphicsFamily.hasValue) {
        report_error("createCommandPool()", "graphicsFamily index is missing");
    }

    VkCommandPoolCreateInfo poolInfo = {0};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.data;

    if(vkCreateCommandPool(device, &poolInfo, NULL, &commandPool) != VK_SUCCESS) {
        report_error("createCommandPool()", "vkCreateCommandPool failed, see above for possible reason");
    }


}

void createCommandBuffers() {
    commandBufferCount = MAX_FRAMES_IN_FLIGHT;
    commandBuffers = malloc(sizeof(VkCommandBuffer) * commandBufferCount);

    VkCommandBufferAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = commandBufferCount;

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers) != VK_SUCCESS) {
        report_error("createCommandBuffer()", "vkAllocateCommandBuffers failed");
    }
}

void createSyncObjects() {
    imageAvailableSemaphoreCount = MAX_FRAMES_IN_FLIGHT;
    imageAvailableSemaphores = malloc(sizeof(VkSemaphore) * imageAvailableSemaphoreCount);
    renderFinishedSemaphoreCount = MAX_FRAMES_IN_FLIGHT;
    renderFinishedSemaphores = malloc(sizeof(VkSemaphore) * renderFinishedSemaphoreCount);
    inFlightFenceCount = MAX_FRAMES_IN_FLIGHT;
    inFlightFences = malloc(sizeof(VkFence) * inFlightFenceCount);

    VkSemaphoreCreateInfo semaphoreInfo = {0};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {0};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

        if(vkCreateSemaphore(device, &semaphoreInfo, NULL, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, NULL, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
        vkCreateFence(device, &fenceInfo, NULL, &inFlightFences[i]) != VK_SUCCESS) {
            report_fatal("createSyncObjects()", "failed to create semaphores and fences!");
        }

    }

}

void bigvulkan_init(u32 exCount, const char **exNames) {
    extensionCount = exCount;
    extensionNames = exNames;


    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createCommandBuffers();
    createSyncObjects();

}

void recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex) {
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        report_error("recordCommandBuffer()", "failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo = {0};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    VkOffset2D offsetZero = {0, 0};
    renderPassInfo.renderArea.offset = offsetZero;
    renderPassInfo.renderArea.extent = swapChainExtent;
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    VkViewport viewport = {0};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = swapChainExtent.width;
    viewport.height = swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor = {0};
    scissor.offset = offsetZero;
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        report_fatal("recordCommandBuffer()", "failed to record the command buffer");
    }

}


void bigvulkan_drawFrame() {
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &inFlightFences[currentFrame]);

    u32 imageIndex;
    vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(commandBuffers[currentFrame], 0);
    recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[1] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
    VkSemaphore signalSemaphores[1] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        report_fatal("bigvulkan_drawFrame()", "vkQueueSubmit failed");
    }

    VkPresentInfoKHR presentInfo = {0};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[1] = {swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(presentQueue, &presentInfo);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;


}

void bigvulkan_cleanup() { // first created, last destroyed
    vkDeviceWaitIdle(device);
    for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, imageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(device, renderFinishedSemaphores[i], NULL);
        vkDestroyFence(device, inFlightFences[i], NULL);
    }
    free(imageAvailableSemaphores);
    free(renderFinishedSemaphores);
    free(inFlightFences);
    vkDestroyCommandPool(device, commandPool, NULL);
    free(commandBuffers);
    for(int i = 0; i < swapChainFramebufferCount; i++) {
        vkDestroyFramebuffer(device, swapChainFramebuffers[i], NULL);
    }
    free(swapChainFramebuffers);
    vkDestroyPipeline(device, graphicsPipeline, NULL);
    vkDestroyPipelineLayout(device, pipelineLayout, NULL);
    vkDestroyRenderPass(device, renderPass, NULL);
    free(swapChainImages);
    for(int i = 0; i < swapChainImageViewCount; i++) {
        vkDestroyImageView(device, swapChainImageViews[i], NULL);
    }
    free(swapChainImageViews);
    vkDestroySwapchainKHR(device, swapChain, NULL);
    vkDestroyDevice(device, NULL);
    vkDestroySurfaceKHR(instance, surface, NULL);
    if(enableValidationLayers) {DestroyDebugUtilsMessengerEXT(instance, debugMessenger, NULL);}
    vkDestroyInstance(instance, NULL);

    }