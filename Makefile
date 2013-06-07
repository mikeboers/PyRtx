
# setenv MACOSX_DEPLOYMENT_TARGET 10.5

SHADERS := $(wildcard shaders/*.sl)
SHADER_OBJS := ${SHADERS:%.sl=%.slo}

LIBS := lib/Gradient.so lib/PythonRtx.so

CFLAGS = $(shell python-config --cflags) -I$$RMANTREE/include
LDFLAGS = $(shell python-config --ldflags)

.PHONY: default build shaders flat clean

default: build shaders

build: ${LIBS}

shaders: ${SHADER_OBJS}

shaders/%.slo: shaders/%.sl
	shader -O2 -o $@ $<

build/%.o: src/%.cpp
	@ mkdir -p build
	g++ ${CFLAGS} -c -o $@ $<

lib/%.so: build/%.o
	@ mkdir -p lib
	g++ ${LDFLAGS} -bundle -undefined dynamic_lookup -o $@ $<

flat: build shaders
	render scenes/flat.rib

clean:
	- rm -rf build
	- rm lib/*.so
	- rm shaders/*.slo
