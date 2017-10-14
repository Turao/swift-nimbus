# C11 has multi-thread support, we may use that
# you'll need at least gcc 4.7 to use this!
CFLAGS=-std=c++11



CC=g++ $(CFLAGS)
# LIB_DIR=./lib/
INC_DIR=./include/
BIN_DIR=./bin/
SRC_DIR=./src/

all: bin Utilities NimbusFile User Socket Session SessionsManager Client NimbusClient Server NimbusServer

bin:
	mkdir -p $(BIN_DIR)/

clean:
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*.o #$(SRC_DIR)/*~ $(INC_DIR)/*~ *~



# Application

NimbusClient:
	$(CC) -c $(SRC_DIR)/NimbusClient.cpp -I$(INC_DIR) -Wall
	mv NimbusClient.o $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/NimbusClient \
					 $(BIN_DIR)/NimbusClient.o \
					 $(BIN_DIR)/Client.o \
					 $(BIN_DIR)/User.o \
					 $(BIN_DIR)/Session.o \
					 $(BIN_DIR)/SessionsManager.o \
					 $(BIN_DIR)/Socket.o \
					 $(BIN_DIR)/NimbusFile.o \
					 $(BIN_DIR)/Utilities.o \
					 -lpthread \
					 -Wall


NimbusServer:
	$(CC) -c $(SRC_DIR)/NimbusServer.cpp -I$(INC_DIR) -Wall
	mv NimbusServer.o $(BIN_DIR)
	$(CC) -o $(BIN_DIR)/NimbusServer \
					 $(BIN_DIR)/NimbusServer.o \
					 $(BIN_DIR)/Server.o \
					 $(BIN_DIR)/Session.o \
					 $(BIN_DIR)/SessionsManager.o \
					 $(BIN_DIR)/Socket.o \
					 $(BIN_DIR)/NimbusFile.o \
					 $(BIN_DIR)/Utilities.o \
					 -lpthread \
					 -Wall

# Classes

Client:
	$(CC) -c $(SRC_DIR)/Client.cpp -I$(INC_DIR) -Wall
	mv Client.o $(BIN_DIR)


User:
	$(CC) -c $(SRC_DIR)/User.cpp -I$(INC_DIR) -Wall
	mv User.o $(BIN_DIR)


Session:
	$(CC) -c $(SRC_DIR)/Session.cpp -I$(INC_DIR) -Wall
	mv Session.o $(BIN_DIR)

SessionsManager:
	$(CC) -c $(SRC_DIR)/SessionsManager.cpp -I$(INC_DIR) -Wall
	mv SessionsManager.o $(BIN_DIR)


Socket:
	$(CC) -c $(SRC_DIR)/Socket.cpp -I$(INC_DIR) -Wall
	mv Socket.o $(BIN_DIR)


Server:
	$(CC) -c $(SRC_DIR)/Server.cpp -I$(INC_DIR) -Wall
	mv Server.o $(BIN_DIR)


NimbusFile:
	$(CC) -c $(SRC_DIR)/NimbusFile.cpp -I$(INC_DIR) -Wall
	mv NimbusFile.o $(BIN_DIR)


Utilities:
	$(CC) -c $(SRC_DIR)/Utilities.cpp -I$(INC_DIR) -Wall
	mv Utilities.o $(BIN_DIR)
