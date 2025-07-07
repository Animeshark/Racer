CC = gcc
CFLAGS = -g -Ilibs/include
LDFLAGS = -Llibs/lib -lraylib -lopengl32 -lgdi32 -lwinmm

SRCS = main.c
OBJS = $(SRCS:.c=.o)

TARGET = game.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	del *.o *.exe
