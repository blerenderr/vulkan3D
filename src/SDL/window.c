#include "window.h"
#include "../utils.h"
#include <SDL_hints.h>
#include <SDL_pixels.h>
#include <SDL_surface.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>


SDL_Window *mainWindow;
SDL_Surface *windowIcon;
void *windowIconData;

void setWindowIcon() {
    u32 imgSize = 0;
    void *imgData = utils_readFile("resource/icon.data", &imgSize, 1);

    if(imgData == NULL) {
        printf("[WARNING] couldn't find resource/icon.data, falling back to default\n");
        return;
    }

    // copy the pointer so we can free it later
    windowIconData = imgData;


    windowIcon = SDL_CreateRGBSurfaceWithFormatFrom(imgData, 100, 100, 8, 400, SDL_PIXELFORMAT_RGBA8888);

    SDL_SetWindowIcon(mainWindow, windowIcon);


}

b8 window_init(u32 *extensionCount, const char **extensionNames) {
    SDL_Window *pWindow = SDL_CreateWindow("hi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");

    u32 exCount;
    vkEnumerateInstanceExtensionProperties(NULL, &exCount, NULL);
    printf("vulkan extensions supported by local install: %i\n", exCount);


    if(pWindow == NULL) {
        printf("failed to create window\n");
        return FALSE;
    }
    mainWindow = pWindow;

    if(SDL_Vulkan_GetInstanceExtensions(mainWindow, extensionCount, extensionNames) == SDL_FALSE) {
        printf("failed to get needed vulkan extensions from SDL\n");
        return FALSE;
    }

    setWindowIcon();
    
    return TRUE;
}

SDL_Window* window_getMain() {
    return mainWindow;
}
void window_destroy() {
    SDL_DestroyWindow(mainWindow);
    SDL_FreeSurface(windowIcon);
    free(windowIconData);
}