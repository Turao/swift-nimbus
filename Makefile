# C11 has multi-thread support, we may use that
# you'll need at least gcc 4.7 to use this!
CFLAGS=-std=c11



CC=gcc $(CFLAGS)
# LIB_DIR=./lib/
INC_DIR=./include/
BIN_DIR=./bin/
SRC_DIR=./src/

all: bin

bin:
  mkdir -p $(BIN_DIR)/


# if we ever need to build as a lib
# lib-[LIBNAME]:
  # ar crs $(LIB_DIR)/lib[LIBNAME].a $(BIN_DIR)/[OBJECT].o

clean:
  rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o #$(SRC_DIR)/*~ $(INC_DIR)/*~ *~