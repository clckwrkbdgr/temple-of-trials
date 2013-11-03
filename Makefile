run: all
	xfce4-terminal -e ./temple

all: main.cpp
	g++ -o temple -lncurses main.cpp

.PHONY: clean
clean:
	rm -rf temple
