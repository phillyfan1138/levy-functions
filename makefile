INCLUDES=-I ./rapidjson/include/rapidjson 
GCCVAL=g++

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	GCCVAL=g++-7
endif

levyfunctions:main.o
	$(GCCVAL) -std=c++14 -O3  -pthread --coverage main.o $(INCLUDES) -o levyfunctions -fopenmp
main.o: main.cpp parse_json.h
	$(GCCVAL) -std=c++14 -O3  -pthread --coverage -c main.cpp  $(INCLUDES) -fopenmp 
clean:
	-rm *.o test

test: test.cpp parse_json.h
	g++ -std=c++14 -pthread test.cpp $(INCLUDES) -o test -fopenmp