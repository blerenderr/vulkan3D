debugCmplrFlags := -g -O0 -DDEBUG
releaseCmplrFlags := -O2
binary := vulkan3D
includeFlags := -Isrc/ -I$(VULKAN_SDK)/include/ -I$(VULKAN_SDK)/include/SDL2/
linkerFlags := -L$(VULKAN_SDK)/lib/ -lvulkan -lSDL2-2.0

default: shaders release

setup:
	mkdir -p ./bin/resource
	cp icon.data ./bin/resource/

shaders:
	glslc ./src/resource/simple_shader.vert -o ./bin/resource/simple_shader.vert.spv
	glslc ./src/resource/simple_shader.frag -o ./bin/resource/simple_shader.frag.spv

debug:
	clang ./src/*.c ./src/SDL/*.c $(debugCmplrFlags) -o ./bin/$(binary) $(includeFlags) $(linkerFlags) -rpath `pwd`/bin/lib

release:
	clang ./src/*.c ./src/SDL/*.c $(releaseCmplrFlags) -o ./bin/$(binary) $(includeFlags) $(linkerFlags) -rpath `pwd`/bin/lib

run:
	cd ./bin/ && ./$(binary)

check: debug
	cd ./bin/ && valgrind --leak-check=yes --track-origins=yes ./$(binary)

clean:
	rm -r ./bin/*

fresh: shaders debug run
