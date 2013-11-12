XFCE_TERMINAL_VERSION := $(shell xfce4-terminal --version 2>/dev/null)
ifdef XFCE_TERMINAL_VERSION
	TERMINAL = xfce4-terminal -e
else
	TERMINAL = screen
endif

BIN = temple
LIBS = -lncurses
SOURCES = $(wildcard *.cpp)
OBJ = $(addprefix tmp/,$(SOURCES:.cpp=.o))
CXXFLAGS = -MD -MP -Werror -Wall

run: $(BIN)
	$(TERMINAL) './$(BIN)'

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(LIBS) -o $@ $(OBJ)

tmp/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean Makefile

clean:
	$(RM) -rf tmp/* $(BIN)

$(shell mkdir -p tmp)
-include $(OBJ:%.o=%.d)

