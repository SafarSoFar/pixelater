# EMSCRIPTEN_PATH ?= $(EMSDK_PATH)/upstream/emscripten
# CLANG_PATH = $(EMSDK_PATH)/upstream/bin
# PYTHON_PATH = /path/to/python
# NODE_PATH = $(EMSDK_PATH)/node/12.9.1_64bit/bin
# PATH = $(shell printenv PATH):$(EMSDK_PATH):$(EMSCRIPTEN_PATH):$(CLANG_PATH):$(NODE_PATH):$(PYTHON_PATH)= $(EMSDK_PATH)/node/12.9.1_64bit/bin

cd $HOME/tools/emsdk/

emsdk update
emsdk install latest
emsdk activate latest
source ./emsdk_env.sh
cd build

emcc -o game.html main.cpp -L /raylib/src/libraylib
make
