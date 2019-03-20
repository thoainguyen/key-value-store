WORKING_DIR=$(shell pwd)
BUILD_DIR=$(WORKING_DIR)/build
$(shell mkdir -p $(BUILD_DIR))

LIB_DIR=$(WORKING_DIR)/libs
META_DIR=$(LIB_DIR)/meta
APP_DIR=$(WORKING_DIR)/app

##################### Shared environment variables
# define the C compiler to use
CXX = gcc
AR = ar
CR = crs
# define any compile-time flags
CXXFLAGS = "-Wall -Wextra -pthread -g -O0  -fPIC"
LIB_META = $(BUILD_DIR)/libmeta.a

ENV = BUILD_DIR=$(BUILD_DIR) CXX=$(CXX) AR=$(AR) CR=$(CR) CXXFLAGS=$(CXXFLAGS) \
	LIB_META=$(LIB_META) META_DIR=$(META_DIR) APP_DIR=$(APP_DIR)

.PHONY: all app clean

all: app

app:
	cd $(META_DIR)/src; make $(ENV)
	cd $(APP_DIR)/src; make $(ENV)

clean:
	cd $(META_DIR)/src; make $(ENV) clean
	cd $(APP_DIR)/src; make $(ENV) clean



