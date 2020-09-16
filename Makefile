CC = /usr/bin/g++
CFLAGS = --std=gnu++11 -Wall -Werror -g -pthread
LDFLAGS = -lavcodec -lavformat -lavutil -lswresample -lasound


#MODULES := network network/frames network/ssl handlers handlers/responsehandlers utils
#SRC_DIR := $(addprefix ./,$(MODULES))
SRC_DIR = src
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_DIR = build/bin
OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))
OUT_DIR = build

INCLUDES := $(addprefix -I,$(SRC_DIR))
#LINCLUDE = -I. -Isrc -Isrc/handlers -Isrc/network -Isrc/network/frames -Isrc/network/ssl

#SRC := $(addsuffix *.cpp,$(SRC_DIR))
#HEADERS := $(addsuffix *.h,$(SRC_DIR))


all: build clean

createdirs:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OUT_DIR)

build: createdirs $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUT_DIR)/napfft $(OBJ) $(LDFLAGS)

#cppfiles: $(HEADERS)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
#	$(info $@)
#	$(info $<)
	$(CC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $@ -c $<

clean:
	rm -rf $(OBJ_DIR)
#	rm -f $(OBJ) core
