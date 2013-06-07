
# setenv MACOSX_DEPLOYMENT_TARGET 10.5

SHADERS := $(wildcard shaders/*.sl)
SHADER_OBJS := ${SHADERS:%.sl=%.slo}

.PHONY: default build shaders flat

default: build shaders

build: lib/Gradient.so

shaders: ${SHADER_OBJS}

shaders/%.slo: shaders/%.sl
	shader -O2 -o $@ $<

build/%.o: src/%.cpp
	@ mkdir -p build
	g++ -I$$RMANTREE/include -c -o $@ $<

lib/%.so: build/%.o
	g++ -bundle -undefined dynamic_lookup -o $@ $<

flat: build shaders
	render scenes/flat.rib
