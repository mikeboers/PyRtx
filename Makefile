
# setenv MACOSX_DEPLOYMENT_TARGET 10.5

SHADERS := $(wildcard shaders/*.sl)
SHADER_OBJS := ${SHADERS:%.sl=%.slo}

SRCS := src/utils.cpp
OBJS := ${SRCS:src/%.cpp=build/%.o}
LIBS := lib/DebugRtx.so lib/PythonRtx.so

CFLAGS := -g -Iinclude -I$$RMANTREE/include $(shell python-config --includes)
LDFLAGS := $(shell python-config --libs)
RENDERFLAGS :=

TEXTURES := textures/van.jpg

.PHONY: default build shaders flat clean textures
.PRECIOUS: build/%.o

default: build

build: ${LIBS}

shaders: ${SHADER_OBJS}

shaders/%.slo: shaders/%.sl
	shader -O2 -o $@ $<

build/%.o: src/%.cpp
	@ mkdir -p build
	g++ ${CFLAGS} -c -o $@ $<

lib/%.so: build/%.o ${OBJS}
	@ mkdir -p lib
	g++ ${LDFLAGS} -bundle -undefined dynamic_lookup -o $@ $^

textures: ${TEXTURES}
textures/van.jpg:
	@ mkdir -p textures
	curl -o $@ http://farm9.staticflickr.com/8213/8369920379_3e29553cef_b.jpg

flat: build shaders textures
	@ mkdir -p out
	render ${RENDERFLAGS} scenes/flat.rib
debug: build shaders textures
	@ mkdir -p out
	render ${RENDERFLAGS} scenes/debug.rib
flickr: build shaders textures
	@ mkdir -p out
	render ${RENDERFLAGS} scenes/flickr.rib

clean:
	- rm -rf build
	- rm lib/*.so
	- rm shaders/*.slo
