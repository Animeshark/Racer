CC = gcc
CFLAGS = -Wall -Wextra -std=c99

SRC_DIR = src
INC_DIR = include
OBJ_DIR = build

RAYLIB_INC = libs/include
RAYLIB_LIB = libs/lib

SRC = $(SRC_DIR)/main.c \
	$(SRC_DIR)/menu.c \
	$(SRC_DIR)/util.c \
	$(SRC_DIR)/gameplay.c

OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

INCLUDE = -I$(INC_DIR) -I$(RAYLIB_INC)
LIBS = -L$(RAYLIB_LIB) -lraylib -lopengl32 -lgdi32 -lwinmm

TARGET = game.exe

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBS)

$(OBJ_DIR):
	if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	if exist $(OBJ_DIR) del /Q $(OBJ_DIR)\*.o
	if exist $(TARGET) del $(TARGET)
