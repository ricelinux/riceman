CC = g++

SRC := src
OBJ := build
BIN := riceman

SOURCES := $(wildcard $(SRC)/*.cpp)
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SOURCES))
HEADERS := $(wildcard $(SRC)/*.hpp)

$(BIN): $(OBJECTS) $(HEADERS)
	$(CC) -o $(BIN) $(OBJECTS) $(HEADERS)

$(OBJ)/%.o: $(SRC)/%.cpp $(OBJ)
	$(CC) -c $< -o $@

$(OBJ):
	mkdir -p $@

install:
	install -Dm755 $(BIN) /usr/bin/$(BIN)
	install -Dm644 README.md /usr/share/doc/$(BIN)

clean:
	-rm -rf build