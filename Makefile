CC = g++

SRC := src
OBJ := build
BIN := riceman
SUB := deps
CURLPP_SRC := $(SUB)/curlpp/src/curlpp
CURLPP_OBJ := $(OBJ)/curlpp
CURLPP_INTERNAL_SRC := $(CURLPP_SRC)/internal
CURLPP_INTERNAL_OBJ := $(CURLPP_OBJ)/internal

VERSION := $(shell git describe --long --tags | sed 's/\([^-]*-g\)/r\1/;s/-/./g')

SOURCES := $(wildcard $(SRC)/*.cpp)
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SOURCES))
HEADERS := $(wildcard $(SRC)/*.hpp)

CURLPP_SOURCES := $(wildcard $(CURLPP_SRC)/*.cpp) $(wildcard $(CURLPP_SRC)/internal/*.cpp)
CURLPP_OBJECTS := $(patsubst $(CURLPP_SRC)/%.cpp, $(CURLPP_OBJ)/%.o, $(CURLPP_SOURCES))

LIBS 	:= libcryptopp libcurl
CFLAGS  := $(shell pkg-config --cflags $(LIBS)) -Ideps/curlpp/include -Ideps/argparse
LDFLAGS := $(shell pkg-config --libs $(LIBS)) -Ideps/argparse

default: $(BIN)

$(BIN): $(OBJECTS) $(HEADERS) # $(CURLPP_OBJECTS)
	$(CC) -o $(BIN) $(OBJECTS) $(HEADERS) $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.cpp $(OBJ)
	$(CC) -c $< -o $@ -D VERSION="$(VERSION)" $(CFLAGS)

$(CURLPP_OBJ)/%.o: $(CURLPP_SRC)/%.cpp $(CURLPP_OBJ)
	$(CC) -c $< -o $@ $(CFLAGS)

$(CURLPP_INTERNAL_OBJ)/%.o: $(CURLPP_INTERNAL_SRC)/%.cpp $(CURLPP_INTERNAL_OBJ)
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJ):
	mkdir -p $@

$(CURLPP_OBJ):
	mkdir -p $@

$(CURLPP_INTERNAL_OBJ):
	mkdir -p $@

install: $(BIN)
	install -Dm755 $(BIN) /usr/bin/$(BIN)
	install -Dm644 README.md /usr/share/doc/$(BIN)

clean:
	-rm -rf build
	-git submodule deinit -f .
	-git submodule update --init --recursive