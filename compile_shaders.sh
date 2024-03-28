#!/bin/sh

#should work as long the SDK bin dir is in PATH
glslc src/resource/simple_shader.vert -o build/src/resource/simple_shader.vert.spv
glslc src/resource/simple_shader.frag -o build/src/resource/simple_shader.frag.spv
