CC = g++
CFLAGS = -Wall -Wextra

all: main child

main: main.cpp
	$(CC) $(CFLAGS) -o main main.cpp

child: child.cpp func.cpp 
	$(CC) $(CFLAGS) -o child child.cpp func.cpp
	

clean:
	rm -f main child *.txt 