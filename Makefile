debugCmplrFlags := -g -O0 -DDEBUG
releaseCmplrFlags := -O2
binary := vulkan3D
includeFlags := -Isrc/ -I$(VULKAN_SDK)/include/
linkerFlags := -lvulkan -L$(VULKAN_SDK)/lib/

setup:
	mkdir -p ./bin/resource
	cp icon.data ./bin/resource/
	bear -- make debug

shaders:
	glslc ./src/resource/simple_shader.vert -o ./bin/resource/simple_shader.vert.spv
	glslc ./src/resource/simple_shader.frag -o ./bin/resource/simple_shader.frag.spv

debug:
	clang ./src/*.c ./src/SDL/*.c $(debugCmplrFlags) -o ./bin/$(binary) $(includeFlags) $(linkerFlags) `sdl2-config --cflags --libs`

release:
	clang ./src/*.c ./src/SDL/*.c $(ReleaseCmplrFlags) -o ./bin/$(binary) $(includeFlags) $(linkerFlags) `sdl2-config --cflags --libs`

run:
	cd ./bin/ && ./$(binary)

check: debug
	cd ./bin/ && valgrind --leak-check=yes $(binary)

fresh: shaders debug run
