# C11 has multi-thread support, we may use that
# you'll need at least gcc 4.7 to use this!
CFLAGS=-std=c++11



CC=g++ $(CFLAGS)
# LIB_DIR=./lib/
INC_DIR=./include/
BIN_DIR=./bin/
SRC_DIR=./src/

all: bin Utilities NimbusFile User Client Server

bin:
	mkdir -p $(BIN_DIR)/


Client:
	$(CC) -c $(SRC_DIR)/Client.cpp -I$(INC_DIR) -Wall
	mv Client.o $(BIN_DIR)


User:
	$(CC) -c $(SRC_DIR)/User.cpp -I$(INC_DIR) -Wall
	mv User.o $(BIN_DIR)


Server:
	$(CC) -c $(SRC_DIR)/Server.cpp -I$(INC_DIR) -Wall
	mv Server.o $(BIN_DIR)


Utilities:
	$(CC) -c $(SRC_DIR)/Utilities.cpp -I$(INC_DIR) -Wall
	mv Utilities.o $(BIN_DIR)


NimbusFile:
	$(CC) -c $(SRC_DIR)/NimbusFile.cpp -I$(INC_DIR) -Wall
	mv NimbusFile.o $(BIN_DIR)


# if we ever need to build as a lib
# lib-[LIBNAME]:
	# ar crs $(LIB_DIR)/lib[LIBNAME].a $(BIN_DIR)/[OBJECT].o

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o #$(SRC_DIR)/*~ $(INC_DIR)/*~ *~