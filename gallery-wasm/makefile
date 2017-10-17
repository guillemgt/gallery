CC=emcc
SOURCES:=$(wildcard ../code/*.cpp ../code/Engine/*.cpp wasm.cpp)
EXPORTS_FILE=makefile_exports.txt
INC_PATH=freetype_headers/
LDFLAGS=-O2 --llvm-opts 2
OUTPUT=build/index.html

all: $(SOURCES) $(OUTPUT)

$(OUTPUT): $(SOURCES)
	$(CC) $(SOURCES) --bind -s FULL_ES2=1 -s STB_IMAGE=1 -std=c++11 $(LDFLAGS) -I$(INC_PATH) -o $(OUTPUT) --preload-file ../bundle -s WASM=1 -s USE_FREETYPE=1 -s TOTAL_MEMORY=33554432 -s USE_SDL=1 --js-library library.js --shell-file shell.html -s EXPORTED_FUNCTIONS="['_main', '_jsResizeWindow']"

clean:
	    rm $(OUTPUT)
