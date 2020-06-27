CC=g++
CPPFLAGS=-w -Wall
TARGET=SnakeGame
all: $(TARGET)

$(TARGET): NcursesSnake.cpp
	$(CC) $(CPPFLAGS) -o $@ $^ -lncurses