XFCE_TERMINAL_VERSION := $(shell xfce4-terminal --version 2>/dev/null)
ifdef XFCE_TERMINAL_VERSION
	TERMINAL = xfce4-terminal --geometry 80x40 -e
else
	TERMINAL = screen
endif

BIN = temple
LIBS = -lncurses -lchthon
SOURCES = $(wildcard *.cpp)
OBJ = $(addprefix tmp/,$(SOURCES:.cpp=.o))
# -Wpadded
WARNINGS = -pedantic -Werror -Wall -Wextra -Wformat=2 -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wuninitialized -Wunused -Wfloat-equal -Wundef -Wno-endif-labels -Wshadow -Wcast-qual -Wcast-align -Wconversion -Wsign-conversion -Wlogical-op -Wmissing-declarations -Wno-multichar -Wredundant-decls -Wunreachable-code -Winline -Winvalid-pch -Wvla -Wdouble-promotion -Wzero-as-null-pointer-constant -Wuseless-cast -Wvarargs -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wsuggest-attribute=format
CXXFLAGS = -MD -MP -std=c++0x $(WARNINGS)

all: $(BIN)

run: $(BIN)
	$(TERMINAL) './$(BIN)'

$(BIN): $(OBJ)
	$(CXX) $(LIBS) -o $@ $^

tmp/%.o: %.cpp
	@echo Compiling $<...
	@$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean Makefile

clean:
	$(RM) -rf tmp/* $(BIN) $(TEST_BIN)

$(shell mkdir -p tmp)
-include $(OBJ:%.o=%.d)

