kernel := $(shell uname -s)

debugCmplrFlags := -g -O0 -DDEBUG
releaseCmplrFlags := -O2
binary := vulkan3D

# for some reason, vulkan SDK ships with SDL on mac but not linux, and the SDL libs from homebrew don't play well with mac
# so we just use the ones from vulkan
ifeq ($(kernel),Darwin)
	includeFlags := -Isrc/ -I$(VULKAN_SDK)/include/ -I$(VULKAN_SDK)/include/SDL2
	linkerFlags := -L$(VULKAN_SDK)/lib/ -lvulkan -lSDL2-2.0
	other := -rpath `pwd`/bin/lib
else
	includeFlags := -Isrc/ -I$(VULKAN_SDK)/include/ 
	linkerFlags := -L$(VULKAN_SDK)/lib/ -lvulkan
	other := `sdl2-config --cflags --libs`
endif

default: shaders release

setup:
	mkdir -p ./bin/resource
	cp icon.data ./bin/resource/

shaders:
	glslc ./src/resource/simple_shader.vert -o ./bin/resource/simple_shader.vert.spv
	glslc ./src/resource/simple_shader.frag -o ./bin/resource/simple_shader.frag.spv

debug:
	clang ./src/*.c $(debugCmplrFlags) -o ./bin/$(binary) $(includeFlags) $(linkerFlags) $(other)

release:
	clang ./src/*.c $(releaseCmplrFlags) -o ./bin/$(binary) $(includeFlags) $(linkerFlags) $(other)

run:
	cd ./bin/ && ./$(binary)

check: debug
	cd ./bin/ && valgrind --leak-check=yes --track-origins=yes ./$(binary)

clean:
	rm ./bin/vulkan3D

clean_all:
	rm -r ./bin/*

fresh: shaders debug run
