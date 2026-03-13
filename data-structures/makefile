CC = gcc
CFLAGS = -Wall -Wextra -g
INCLUDE = -I include
SRC_DIR = src
OBJ_DIR = obj
TEST_DIR = test

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS)) 
LIB_OBJS = $(filter-out $(OBJ_DIR)/main.o, $(OBJS))          

TARGET = program.out

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)
	
# regla genérica para cualquier test: make test/test_graph
test/%: $(TEST_DIR)/%.c $(LIB_OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $(LIB_OBJS) $< -o $@
	./$@
	rm -f $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)

clean_obj:
	rm -f $(OBJ_DIR)/*.o

.PHONY: clean clean_obj