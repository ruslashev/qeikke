warnings = -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable \
		   -Wduplicated-cond -Wduplicated-branches -Wdouble-promotion \
		   -Wnull-dereference -Wformat=2 -Wdisabled-optimization \
		   -Wsuggest-override -Wlogical-op -Wtrampolines -Wfloat-equal
flags = -ggdb3 -Og -std=c++0x -fno-rtti -fno-exceptions -static
libraries = -lSDL2 -lGLEW -lGL
CXX = g++
BIN = qeikke

OBJS = $(patsubst src/%.cc,.objs/%.o, $(shell find src/ -type f -iname '*.cc'))
DEPS = $(OBJS:.o=.d)
CXXFLAGS = $(warnings) $(flags)
LDFLAGS = $(libraries)

$(shell mkdir -p .objs >/dev/null)
$(shell mkdir -p .objs/engine >/dev/null)
$(shell mkdir -p .objs/client >/dev/null)

all: $(BIN)
	./$(BIN)

$(BIN): $(OBJS)
	@echo "Linking to $@"
	@$(CXX) -o $@ $^ $(LDFLAGS)

.objs/%.o: src/%.cc
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

