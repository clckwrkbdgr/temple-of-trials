XFCE_TERMINAL_VERSION := $(shell xfce4-terminal --version 2>/dev/null)
ifdef XFCE_TERMINAL_VERSION
	TERMINAL = xfce4-terminal --geometry 80x40 -e
else
	TERMINAL = screen
endif

BIN = temple
TEST_BIN = temple_test
LIBNAME = libchthon.so
LIBS = -lncurses
ENGINE_SOURCES = $(wildcard engine/*.cpp)
TEST_SOURCES = $(wildcard engine/test/*.cpp) $(ENGINE_SOURCES)
LIB_SOURCES = $(ENGINE_SOURCES)
SOURCES = $(wildcard *.cpp)
OBJ = $(addprefix tmp/,$(SOURCES:.cpp=.o))
LIB_OBJ = $(addprefix tmp/,$(LIB_SOURCES:.cpp=.o))
TEST_OBJ = $(addprefix tmp/,$(TEST_SOURCES:.cpp=.o))
WARNINGS = -Werror -Wall
#WARNINGS = -pedantic -Werror -Wall -Wextra -Wformat=2 -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wuninitialized -Wunused -Wfloat-equal -Wundef -Wno-endif-labels -Wshadow -Wcast-qual -Wcast-align -Wconversion -Wsign-conversion -Wlogical-op -Wmissing-declarations -Wno-multichar -Wpadded -Wredundant-decls -Wunreachable-code -Winline -Winvalid-pch -Wvla -Wdouble-promotion -Wzero-as-null-pointer-constant -Wuseless-cast -Wvarargs -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wsuggest-attribute=format
CXXFLAGS = -MD -MP $(WARNINGS)

all: $(BIN)

lib: $(LIBNAME)

run: $(BIN)
	$(TERMINAL) './$(BIN)'

test: $(TEST_BIN)
	./$(TEST_BIN) $(TESTS)

$(LIBNAME): $(LIB_OBJ)
	$(CXX) -shared $(LIBS) -o $@ $^

$(TEST_BIN): $(TEST_OBJ)
	$(CXX) $(LIBS) -o $@ $^

$(BIN): $(OBJ)
	$(CXX) $(LIBS) -L. -lchthon -o $@ $^

tmp/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -fpic $< -o $@

.PHONY: clean Makefile

clean:
	$(RM) -rf tmp/* $(BIN) $(TEST_BIN)

$(shell mkdir -p tmp)
$(shell mkdir -p tmp/engine)
$(shell mkdir -p tmp/engine/test)
-include $(OBJ:%.o=%.d)
-include $(TEST_OBJ:%.o=%.d)

