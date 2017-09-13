warnings = -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable \
		   -Wduplicated-cond -Wduplicated-branches -Wdouble-promotion \
		   -Wnull-dereference -Wformat=2 -Wdisabled-optimization \
		   -Wsuggest-override -Wlogical-op -Wtrampolines -Wfloat-equal
flags = -ggdb3 -Og -std=c++0x -fno-rtti -fno-exceptions -static
libraries = -lSDL2 -lGLEW -lGL
CXX = g++
BIN = qeikke

CC_OBJS = $(patsubst src/%.cc,.objs/%.o, $(shell find src/ -type f -iname '*.cc'))
CPP_OBJS = $(patsubst src/%.cpp,.objs/%.o, $(shell find src/ -type f -iname '*.cpp'))
OBJS = $(CC_OBJS) $(CPP_OBJS)
DEPS = $(OBJS:.o=.d)
CXXFLAGS = $(warnings) $(flags)
LDFLAGS = $(libraries)

$(shell mkdir -p .objs >/dev/null)
$(shell mkdir -p .objs/client >/dev/null)
$(shell mkdir -p .objs/engine >/dev/null)
# TODO: introduced in porting doom3_collision
$(shell mkdir -p .objs/d3c >/dev/null)
$(shell mkdir -p .objs/d3c/math >/dev/null)
$(shell mkdir -p .objs/d3c/misc >/dev/null)
$(shell mkdir -p .objs/d3c/physics >/dev/null)
$(shell mkdir -p .objs/d3c/scene >/dev/null)
$(shell mkdir -p .objs/d3c/material >/dev/null)
$(shell mkdir -p .objs/d3c/model >/dev/null)
$(shell mkdir -p .objs/d3c/renderer_opengl >/dev/null)

# TODO: introduced in porting doom3_collision
all: $(BIN)
	mv qeikke res
	cd res && ./qeikke

$(BIN): $(OBJS)
	@echo "Linking to $@"
	@$(CXX) -o $@ $^ $(LDFLAGS)

.objs/%.o: src/%.cc
	@echo "Compiling $<"
	@$(CXX) -MMD -MP -c -o $@ $< $(CXXFLAGS)

.objs/%.o: src/%.cpp
	@echo "Compiling $<"
	@$(CXX) -MMD -MP -c -o $@ $< $(CXXFLAGS)

gdb: $(BIN)
	gdb $(BIN)

valgrind: $(BIN)
	valgrind --leak-check=full ./$(BIN)

callgrind: $(BIN)
	@valgrind --tool=callgrind ./$(BIN)
	@kcachegrind callgrind.out.$!

.PHONY : clean
clean:
	@rm -f $(BIN) $(OBJS) $(DEPS)

-include $(DEPS)

