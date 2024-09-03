TARGET=./main.out
CC=g++
SRCS=\
	./main.cpp
STD=-std=c++17
WER=-Wall -Wextra -Werror -ansi

all: clean $(TARGET)

$(TARGET): 
	$(CC) -O3 $(STD) -lm -o $(TARGET) $(SRCS)

build: $(TARGET)

clean:
	rm -rf $(TARGET)
