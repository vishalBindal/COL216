all: main.cpp Cache.cpp Set.cpp Line.cpp general.cpp Main_memory.cpp
	g++ main.cpp Cache.cpp Set.cpp Line.cpp general.cpp Main_memory.cpp -o main

run:
	$(MAKE)
	./main

test: test.cpp Cache.cpp Set.cpp Line.cpp general.cpp Main_memory.cpp
	g++ test.cpp Cache.cpp Set.cpp Line.cpp general.cpp Main_memory.cpp -o test

clean:
	rm main
	rm test

