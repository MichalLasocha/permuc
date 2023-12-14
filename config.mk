SHELL ?= /bin/sh

#app info
VERSION    ?= 0.0.1
TARGET     ?= permuc
NAME       ?= PermuC
#APP_ID can start with a website or email in reverse url format
APP_ID     ?= com.email.name.$(TARGET)
#APP_PREFIX is APP_ID converted to a path.
APP_PREFIX ?= $(shell echo $(APP_ID) | sed 's:\.:/:g;s:^:/:g')
COPYRIGHT  ?= Copyright (C) 2023
AUTHOR     ?= Michał Łasocha
COMMENT    ?= GTK+ 3.0 template Application
CATEGORIES ?= Utility;ComputerScience;GNOME;GTK;

# Customize below to fit your system

# Install paths
PREFIX ?= /usr

#Project directory
PD = $(shell pwd)

#Build/Source paths
SRC     ?= $(PD)/source
BLD     ?= $(PD)/build
DATA    ?= $(SRC)/data

#Files
BIN       ?= $(BLD)/bin/$(TARGET)
OBJ       ?= $(BLD)/main.o $(BLD)/version.o $(BLD)/data.o
GLADE     ?= $(DATA)/window_main.glade
RESOURCES ?= $(DATA)/icon.svg $(DATA)/window_main.glade
DESKTOP   ?= $(BLD)/$(TARGET).desktop

#Dependencies
PKG_CONFIG ?= pkg-config

INCS = `$(PKG_CONFIG) --cflags gtk+-3.0` \
#      `$(PKG_CONFIG) --cflags next_library`

LIBS = `$(PKG_CONFIG) --libs gtk+-3.0` \
#      `$(PKG_CONFIG) --libs next_library`

#Optional flags
CFLAGS         ?= -march=native -pipe
RELEASE_CFLAGS  = -O2 -g -flto
RELEASE_LDFLAGS = -flto
DEBUG_CPPFLAGS  = -DDEBUG
DEBUG_CFLAGS    = -O0 -ggdb -Wpedantic -Wall -Wextra -fsanitize=address -fsanitize=undefined -fstack-protector-strong
DEBUG_LDFLAGS   = -fsanitize=address -fsanitize=leak -fsanitize=undefined

#Required flags
CPPFLAGS += -DVERSION=\"$(VERSION)\" -DNAME=\""$(NAME)"\" -DAPP_ID=\"$(APP_ID)\" -DAPP_PREFIX=\"$(APP_PREFIX)\" -DAUTHOR=\""$(AUTHOR)"\" -DCOPYRIGHT="\"$(COPYRIGHT)\"" -DTARGET=\"$(TARGET)\"
CFLAGS   += $(INCS) -I$(BLD)
LDFLAGS  += $(LIBS) -rdynamic
