.PHONY: all clean

PROJECT_NAME       ?= game
RAYLIB_VERSION     ?= 4.5.0
RAYLIB_PATH        ?= ../..

COMPILER_PATH      ?= C:/raylib/w64devkit/bin

PLATFORM           ?= PLATFORM_DESKTOP

DESTDIR ?= /usr/local
RAYLIB_INSTALL_PATH ?= $(DESTDIR)/lib
RAYLIB_H_INSTALL_PATH ?= $(DESTDIR)/include

RAYLIB_LIBTYPE        ?= STATIC
BUILD_MODE            ?= RELEASE

USE_EXTERNAL_GLFW     ?= FALSE
USE_WAYLAND_DISPLAY   ?= FALSE

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(OS),Windows_NT)
        PLATFORM_OS=WINDOWS
        export PATH := $(COMPILER_PATH):$(PATH)
    else
        UNAMEOS=$(shell uname)
        ifeq ($(UNAMEOS),Linux)
            PLATFORM_OS=LINUX
        endif
        ifeq ($(UNAMEOS),Darwin)
            PLATFORM_OS=OSX
        endif
    endif
endif

CC = gcc  # Use C compiler instead of C++

MAKE = mingw32-make
ifeq ($(PLATFORM_OS),LINUX)
    MAKE = make
endif
ifeq ($(PLATFORM_OS),OSX)
    MAKE = make
endif

CFLAGS += -Wall -std=c89 -D_DEFAULT_SOURCE -Wno-missing-braces

ifeq ($(BUILD_MODE),DEBUG)
    CFLAGS += -g -O0
else
    CFLAGS += -s -O1
endif

INCLUDE_PATHS = -I. -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external
ifneq ($(wildcard /opt/homebrew/include/.*),)
    INCLUDE_PATHS += -I/opt/homebrew/include
endif

LDFLAGS = -L.
ifneq ($(wildcard $(RAYLIB_RELEASE_PATH)/.*),)
    LDFLAGS += -L$(RAYLIB_RELEASE_PATH)
endif
ifneq ($(wildcard $(RAYLIB_PATH)/src/.*),)
    LDFLAGS += -L$(RAYLIB_PATH)/src
endif
ifneq ($(wildcard /opt/homebrew/lib/.*),)
    LDFLAGS += -L/opt/homebrew/lib
endif

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),WINDOWS)
        LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
    endif
    ifeq ($(PLATFORM_OS),LINUX)
        LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    endif
    ifeq ($(PLATFORM_OS),OSX)
        LDLIBS = -lraylib -framework OpenGL -framework OpenAL -framework Cocoa -framework IOKit
    endif
endif

SRC_DIR = src
OBJ_DIR = obj

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

all: $(PROJECT_NAME)

$(PROJECT_NAME): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -rf $(OBJ_DIR)/*.o $(PROJECT_NAME)
