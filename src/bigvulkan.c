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
    SwapChainSupportDetails details = {0};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    VkResult result;

    details.formatCount = 0;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formatCount, NULL);
    //if(details.formatCount != 0) {
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formatCount, details.formats);
    //}

    details.presentModeCount = 0;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.presentModeCount, NULL);
    //if(details.presentModeCount != 0) {    
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.presentModeCount, details.presentModes);
    //}
 


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


    createInfo.enabledLayerCount = 0;
    // device-specific validation layers are deprecated, but we enable them anyway
    if(enableValidationLayers) {
        createInfo.enabledLayerCount = VALIDATION_LAYERS_COUNT;
        createInfo.ppEnabledLayerNames = validationLayers;
    }

    // needed for vulkan to not throw a fit on mac
    if(strcmp(SDL_GetPlatform(), "Mac OS X") == 0) {
        const char *deviceExtensionNames[1] = {"VK_KHR_portability_subset"};
        createInfo.ppEnabledExtensionNames = deviceExtensionNames;
        createInfo.enabledExtensionCount = 1;
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
    vkGetSwapchainImagesKHR(device, swapChain, &swapChainImageCount, swapChainImages);

    // store our stuff for later
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;

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

}
void bigvulkan_cleanup() { // first created, last destroyed
    vkDestroySwapchainKHR(device, swapChain, NULL);
    vkDestroyDevice(device, NULL);
    vkDestroySurfaceKHR(instance, surface, NULL);
    if(enableValidationLayers) {DestroyDebugUtilsMessengerEXT(instance, debugMessenger, NULL);}
    vkDestroyInstance(instance, NULL);

}