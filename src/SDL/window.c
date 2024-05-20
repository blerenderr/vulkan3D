#include "window.h"
#include "../utils.h"
#include "../report.h"
#include <SDL_hints.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>


SDL_Window * mainWindow;
SDL_Surface * windowIcon;
void *windowIconData;

void setWindowIcon() {
    u32 imgSize = 0;
    void *imgData = utils_readFile("resource/icon.data", &imgSize, 1);

    if(imgData == NULL) {
        report_warning("setWindowIcon()","couldn't find resource/icon.data, falling back to default");
        return;
    }

    // copy the pointer so we can free it later
    windowIconData = imgData;


    windowIcon = SDL_CreateRGBSurfaceWithFormatFrom(imgData, 100, 100, 8, 400, SDL_PIXELFORMAT_RGBA8888);

    SDL_SetWindowIcon(mainWindow, windowIcon);


}
b8 window_init(u32 *extensionCount, const char **extensionNames) {
    SDL_Window *pWindow = SDL_CreateWindow("hi", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                            SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
    u32 exCount;
    vkEnumerateInstanceExtensionProperties(NULL, &exCount, NULL);
    report_info("window_init()","vulkan extensions supported by local install: %u", exCount);

    if(pWindow == NULL) {
        report_fatal("window_init()","failed to create window");
        return FALSE;
    }
    mainWindow = pWindow;

    if(SDL_Vulkan_GetInstanceExtensions(mainWindow, extensionCount, extensionNames) == SDL_FALSE) {
        report_error("window_init()","failed to get needed vulkan extensions from SDL");
        return FALSE;
    }

    setWindowIcon();

    return TRUE;
}

void window_destroy() {
    SDL_DestroyWindow(mainWindow);
    SDL_FreeSurface(windowIcon);
    free(windowIconData);
}