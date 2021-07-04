CC = g++

SRC := src
OBJ := build
BIN := riceman

SOURCES := $(wildcard $(SRC)/*.cpp)
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SOURCES))
HEADERS := $(wildcard $(SRC)/*.hpp)

$(BIN): $(OBJECTS) $(HEADERS)
	$(CC) -o $(BIN) $(OBJECTS) $(HEADERS)

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC) -c $< -o $@