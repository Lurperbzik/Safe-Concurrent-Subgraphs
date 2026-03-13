CC = gcc
CFLAGS = -Wall -Wextra -g
DS_DIR = data-structures
INCLUDE = -I $(DS_DIR)/include
SRC_DIR = $(DS_DIR)/src
OBJ_DIR = $(DS_DIR)/obj
TEST_DIR = $(DS_DIR)/test

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
MAIN_OBJ = main.o

TARGET = program

all: $(TARGET)

$(TARGET): $(OBJS) $(MAIN_OBJ)
	$(CC) $(CFLAGS) $(OBJS) $(MAIN_OBJ) -o $(TARGET)

# regla genérica para cualquier test: make data-structures/test/test_graph
$(TEST_DIR)/%: $(TEST_DIR)/%.c $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $(OBJS) $< -o $@
	./$@
	rm -f $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o $(MAIN_OBJ) $(TARGET)

clean_obj:
	rm -f $(OBJ_DIR)/*.o $(MAIN_OBJ)

.PHONY: clean clean_obj