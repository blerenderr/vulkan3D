#include "window.h"
#include <SDL_hints.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>


SDL_Window *mainWindow;


b8 window_init(u32 *extensionCount, const char **extensionNames) {
    SDL_Window *pWindow = SDL_CreateWindow("hi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");

    u32 exCount;
    vkEnumerateInstanceExtensionProperties(NULL, &exCount, NULL);
    printf("vulkan extensions supported by local install: %i\n", exCount);


    if(pWindow == NULL) {
        printf("failed to create window\n");
        return 1;
    }
    mainWindow = pWindow;

    if(SDL_Vulkan_GetInstanceExtensions(mainWindow, extensionCount, extensionNames) == SDL_FALSE) {
        printf("failed to get needed vulkan extensions from SDL\n");
        return 1;
    }
    
    return 0;
}

SDL_Window* window_getMain() {
    return mainWindow;
}
void window_destroy() {
    SDL_DestroyWindow(mainWindow);
}