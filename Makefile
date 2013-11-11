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
CXXFLAGS = -Werror -Wall

run: $(BIN)
	$(TERMINAL) './$(BIN)'

$(BIN): tmp/deps $(OBJ)
	$(CXX) $(LIBS) -o $@ $(OBJ)

tmp/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: tmp/deps clean Makefile

tmp/deps: $(SOURCES)
	$(CXX) -MM $^ > $@

clean:
	$(RM) -rf tmp/*.o $(BIN) tmp/deps

$(shell mkdir -p tmp)
include tmp/deps

