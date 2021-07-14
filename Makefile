CC = g++

SRC := src
OBJ := build
BIN := riceman
SUB := deps

VERSION := $(shell git describe --long --tags | sed 's/\([^-]*-g\)/r\1/;s/-/./g')

SOURCES := $(wildcard $(SRC)/*.cpp)
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SOURCES))
HEADERS := $(wildcard $(SRC)/*.hpp)

LIBS 	:= libcryptopp libcurl
CFLAGS  := $(shell pkg-config --cflags $(LIBS)) -Ideps/curlpp/include
LDFLAGS := $(shell pkg-config --libs $(LIBS)) -Ldeps/curlpp -l:libcurlpp.so

.PHONY: deps/curlpp

$(BIN): deps/curlpp $(OBJECTS) $(HEADERS)
	$(CC) -o $(BIN) $(OBJECTS) $(HEADERS) $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.cpp $(OBJ)
	$(CC) -c $< -o $@ -D VERSION=\"$(VERSION)\" $(CFLAGS)

deps/curlpp:
	cd deps/curlpp; \
	cmake .; \
	make;

$(OBJ):
	mkdir -p $@

install:
	install -Dm755 $(BIN) /usr/bin/$(BIN)
	install -Dm644 README.md /usr/share/doc/$(BIN)

clean:
	-rm -rf build
	-git submodule deinit -f .
	-git submodule update --init --recursive
