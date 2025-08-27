CC = gcc
CFLAGS = -g -O3 -Iinclude -I/usr/bin/include/
LDFLAGS = -L/usr/bin/lib/
LDLIBS = -lflint -lgmp -lmpfr -lsodium -lm

SRC_DIR = src
INC_DIR = include

SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/matrix.c \
       $(SRC_DIR)/utils.c \
       $(SRC_DIR)/params.c \
       $(SRC_DIR)/keygen.c \
       $(SRC_DIR)/signer.c \
       $(SRC_DIR)/verifier.c

TEST_SRC = $(SRC_DIR)/test_weight_distribution.c \
          $(SRC_DIR)/matrix.c \
          $(SRC_DIR)/utils.c \
          $(SRC_DIR)/params.c \
          $(SRC_DIR)/keygen.c

OBJS = $(SRCS:.c=.o)
TEST_OBJS = $(TEST_SRC:.c=.o)
TARGET = sig
TEST_TARGET = test_weight

.PHONY: all clean test

all: $(TARGET)

test: $(TEST_TARGET)

# lib: libsigscheme.so

# libsigscheme.so: $(SRC_DIR)/keygen.c $(SRC_DIR)/matrix.c $(SRC_DIR)/utils.c $(SRC_DIR)/params.c
# 	$(CC) -shared -fPIC $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) $(TEST_OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

# Rule to compile .c to .o (object files)
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_DIR)/*.o $(TARGET) $(TEST_TARGET)
