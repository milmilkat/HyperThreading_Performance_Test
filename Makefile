
output: main.o
	g++ -pthread main.o -o output

main.o: main.cpp
	g++ -c main.cpp

clean:
	rm *.o output


