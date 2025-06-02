CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lSDL2
SRC_DIR = src
OBJ_DIR = obj
BIN = LifeCycle

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN)

.PHONY: clean
all: $(BIN)