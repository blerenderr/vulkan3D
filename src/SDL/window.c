#include "window.h"
#include <SDL_hints.h>
#include <vulkan/vulkan.h>


SDL_Window *mainWindow;


b8 window_init() {
    SDL_Window *pWindow = SDL_CreateWindow("hi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");

    u32 extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
    printf("vulkan extensions supported: %d\n", extensionCount);


    if(pWindow == NULL) {
        printf("failed to create window\n");
        return 1;
    }
    mainWindow = pWindow;
    return 0;
}

SDL_Window* window_getMain() {
    return mainWindow;
}
void window_destroy() {
    SDL_DestroyWindow(mainWindow);
}