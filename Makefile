CC = clang

SRC := src
OBJ := build
BIN := riceman
SUB := deps
CPR_SRC := $(SUB)/cpr/cpr
CPR_OBJ := $(OBJ)/cpr

VERSION := $(shell git describe --long --tags | sed 's/\([^-]*-g\)/r\1/;s/-/./g')

SOURCES := $(wildcard $(SRC)/*.cpp)
OBJECTS := $(patsubst $(SRC)/%.cpp, $(OBJ)/%.o, $(SOURCES))
HEADERS := $(wildcard $(SRC)/*.hpp)

CPR_SOURCES := $(wildcard $(CPR_SRC)/*.cpp)
CPR_OBJECTS := $(patsubst $(CPR_SRC)/%.cpp, $(CPR_OBJ)/%.o, $(CPR_SOURCES))

LIBS 	 := libcurl fmt libgit2 libcryptopp
INCFLAGS := -Ideps/cpr/include -Ideps/argparse
CFLAGS   := $(shell pkg-config --cflags $(LIBS)) -std=c++17
LDFLAGS  := $(shell pkg-config --libs $(LIBS)) -lstdc++ -lm -lstdc++fs

debug: CFLAGS += -O0
debug: $(BIN)

release: CFLAGS += -Os
release: $(BIN)

$(BIN): $(OBJECTS) $(HEADERS) $(CPR_OBJECTS)
	$(CC) -o $(BIN) $(OBJECTS) $(CPR_OBJECTS) $(LDFLAGS) $(INCFLAGS)

$(OBJ)/%.o: $(SRC)/%.cpp | $(OBJ)
	$(CC) -c $< -o $@ -D VERSION=\"$(VERSION)\" $(CFLAGS) $(INCFLAGS)

$(CPR_OBJ)/%.o: $(CPR_SRC)/%.cpp | $(CPR_OBJ)
	$(CC) -c $< -o $@ $(CFLAGS) -Ideps/cpr/include

$(OBJ):
	mkdir -p $@

$(CPR_OBJ):
	mkdir -p $@

$(CPR_INTERNAL_OBJ):
	mkdir -p $@

install: $(BIN)
	install -Dm755 $(BIN) /usr/bin/$(BIN)
	install -Dm644 README.md /usr/share/doc/$(BIN)

clean:
	-find $(OBJ) -maxdepth 1 -type f -delete

# Forces all objects including deps be rebuilt (very rarely needed)
clean_all:
	-rm -rf build
	-git submodule deinit -f .
	-git submodule update --init --recursive