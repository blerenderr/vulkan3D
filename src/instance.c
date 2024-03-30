#include "instance.h"
#include <vulkan/vulkan_core.h>
#include <stdio.h>

VkInstance instance;

void instance_init(u32 *extensionCount, const char **extensionNames) {
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

    // to change later
    createInfo.enabledLayerCount = 0;

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