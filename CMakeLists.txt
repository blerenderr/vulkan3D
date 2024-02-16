cmake_minimum_required(VERSION 3.5.0)
project(vulkan3D VERSION 0.1.0 LANGUAGES C)

#include_directories(SYSTEM $ENV{VULKAN_SDK}/include/)
#add_link_options(-lvulkan -L$ENV{VULKAN_SDK}/lib/)

include(CTest)
enable_testing()    

add_subdirectory(src)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
