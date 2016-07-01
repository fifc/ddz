
CXXFLAGS += -std=c++14 -O3 -Wall

all: ddz

ddz: ddz.o dealer.o run.o advisor.o main.o
	g++ -o $@ $^
clean:
	rm -rf *.o ddz
run: ddz
	./ddz
