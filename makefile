
CXXFLAGS += -std=c++14 -Wall

all: ddz

ddz: ddz.o dealer.o run.o main.o
	g++ -o $@ $^
clean:
	rm -rf *.o ddz
run: ddz
	./ddz
