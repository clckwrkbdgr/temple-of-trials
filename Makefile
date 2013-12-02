XFCE_TERMINAL_VERSION := $(shell xfce4-terminal --version 2>/dev/null)
ifdef XFCE_TERMINAL_VERSION
	TERMINAL = xfce4-terminal --geometry 80x40 -e
else
	TERMINAL = screen
endif

BIN = temple
LIBS = -lncurses
ENGINE_SOURCES = $(wildcard engine/*.cpp)
TEST_SOURCES = $(wildcard engine/test/*.cpp) $(ENGINE_SOURCES)
SOURCES = $(wildcard *.cpp) $(ENGINE_SOURCES)
OBJ = $(addprefix tmp/,$(SOURCES:.cpp=.o))
TEST_OBJ = $(addprefix tmp/,$(TEST_SOURCES:.cpp=.o))
CXXFLAGS = -MD -MP -Werror -Wall

run: $(BIN)
	$(TERMINAL) './$(BIN)'

all: $(BIN)

test: $(TEST_OBJ)
	$(CXX) $(LIBS) -o $@ $(TEST_OBJ)
	./test

$(BIN): $(OBJ)
	$(CXX) $(LIBS) -o $@ $(OBJ)

tmp/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean Makefile

clean:
	$(RM) -rf tmp/* $(BIN)

$(shell mkdir -p tmp)
$(shell mkdir -p tmp/engine)
$(shell mkdir -p tmp/engine/test)
-include $(OBJ:%.o=%.d)

