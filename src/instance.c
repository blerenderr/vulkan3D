#include "instance.h"
#include "types.h"
#include <vulkan/vulkan_core.h>
#include <stdio.h>
#include <string.h>

VkInstance instance;
VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

#ifdef DEBUG
const b8 enableValidationLayers = TRUE;
#else
const b8 enableValidationLayers = FALSE;
#endif

#define VALIDATION_LAYERS_COUNT 1
const char* validationLayers[VALIDATION_LAYERS_COUNT] = {"VK_LAYER_KHRONOS_validation"};

typedef struct QueueFamilyIndices_t {
    optional_u32 graphicsFamily;
} QueueFamilyIndices;



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
void instance_init(u32 *extensionCount, const char **extensionNames) {
    if(enableValidationLayers && !checkVLayerSupport()) {
        printf("requested validation layers are not available!\n");
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


    createInfo.enabledLayerCount = 0;
    if(enableValidationLayers) {
        createInfo.enabledLayerCount = VALIDATION_LAYERS_COUNT;
        createInfo.ppEnabledLayerNames = validationLayers;
    }

    // needed for vulkan to not throw a fit on mac
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    // remember, extensionCount is the total elements and we're indexing into the next one
    extensionNames[*extensionCount] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
    *extensionCount = *extensionCount + 1;


    // pass in our parameters
    createInfo.enabledExtensionCount = *extensionCount;
    createInfo.ppEnabledExtensionNames = extensionNames;


    VkResult result = vkCreateInstance(&createInfo, NULL, &instance);
    printf("vkCreateInstance result: %d\n", result);
}
void instance_destroy() {
    vkDestroyInstance(instance, NULL);
}


QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    indices.graphicsFamily.hasValue = FALSE;

    u32 queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);
    VkQueueFamilyProperties queueFamilies[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);


    for(int i = 0; i < queueFamilyCount; i++) {
        if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily.data = i;
            indices.graphicsFamily.hasValue = TRUE;
        }

        if(indices.graphicsFamily.hasValue) {
            break;
        }
    }



    return indices;
}
b8 isDeviceSuitable(VkPhysicalDevice device) {
    QueueFamilyIndices indices = findQueueFamilies(device);
    return indices.graphicsFamily.hasValue;

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);


    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           deviceFeatures.geometryShader;
}
void instance_pickPhysicalDevice() {
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);
    if(deviceCount == 0) {
        printf("failed to find GPUs with vulkan support!\n");
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
        printf("couldn't find GPU with suitable features\n");
    }

}

