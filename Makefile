XFCE_TERMINAL_VERSION := $(shell xfce4-terminal --version 2>/dev/null)
ifdef XFCE_TERMINAL_VERSION
	TERMINAL = xfce4-terminal --geometry 80x40 -e
else
	TERMINAL = screen sh -c
endif

BIN = temple
TEST_BIN = temple_test
VERSION = $(shell git tag | tail -1 | sed 's/.*\([0-9]\+\.[0-9]\+\.[0-9]\+\)/\1/')
MAJOR_VERSION = $(shell git tag | tail -1 | sed 's/.*\([0-9]\+\)\.[0-9]\+\.[0-9]\+/\1/')
LIBNAME = libchthon.so
LIBRARY = $(LIBNAME).$(VERSION)
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
LD_LIB_PATH = $(shell pwd)
LD_LIB_ENV = LD_LIBRARY_PATH=$(LD_LIB_PATH):$$LD_LIBRARY_PATH

all: $(BIN)

lib: $(LIBRARY)

INSTALL_PREFIX = /usr/local
INSTALL_PREFIX_LIB = $(INSTALL_PREFIX)/lib

libinstall: lib
	cp $(LIBRARY) $(INSTALL_PREFIX_LIB)
	chmod 0755 $(INSTALL_PREFIX_LIB)/$(LIBRARY)
	ln -s $(INSTALL_PREFIX_LIB)/$(LIBRARY) $(INSTALL_PREFIX_LIB)/$(LIBNAME).$(MAJOR_VERSION)
	ln -s $(INSTALL_PREFIX_LIB)/$(LIBNAME).$(MAJOR_VERSION) $(INSTALL_PREFIX_LIB)/$(LIBNAME)
	ldconfig

run: $(BIN)
	$(TERMINAL) 'sh -c "export $(LD_LIB_ENV); ./$(BIN)"'

test: $(TEST_BIN)
	./$(TEST_BIN) $(TESTS)

$(LIBRARY): $(LIB_OBJ)
	$(CXX) -shared $(LIBS) -o $@ $^

$(TEST_BIN): $(TEST_OBJ)
	$(CXX) $(LIBS) -o $@ $^

$(BIN): $(OBJ)
	$(CXX) $(LIBS) -L. -lchthon -o $@ $^

tmp/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -fpic $< -o $@

.PHONY: clean Makefile

clean:
	$(RM) -rf tmp/* $(BIN) $(TEST_BIN) $(LIBRARY)

$(shell mkdir -p tmp)
$(shell mkdir -p tmp/engine)
$(shell mkdir -p tmp/engine/test)
-include $(OBJ:%.o=%.d)
-include $(TEST_OBJ:%.o=%.d)

