CC = gcc
CFLAGS = -Wall -Wextra -std=c99
INCLUDE = -Ilibs/include

SRC = main.c menu.c util.c gameplay.c
OBJ = $(SRC:.c=.o)

TARGET = game.exe
LIBS = -Llibs/lib -lraylib -lopengl32 -lgdi32 -lwinmm

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	del *.o *.exe
