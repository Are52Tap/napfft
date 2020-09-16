CC = /usr/bin/g++
CFLAGS = --std=gnu++11 -Wall -Werror -g -pthread
LDFLAGS = -lavcodec -lavformat -lavutil -lswresample -lasound


#MODULES := network network/frames network/ssl handlers handlers/responsehandlers utils
#SRC_DIR := $(addprefix ./,$(MODULES))
SRC_DIR = ./
INCLUDES := $(addprefix -I,$(SRC_DIR))
#LINCLUDE = -I. -Isrc -Isrc/handlers -Isrc/network -Isrc/network/frames -Isrc/network/ssl

SRC := $(addsuffix *.cpp,$(SRC_DIR))
HEADERS := $(addsuffix *.h,$(SRC_DIR))


all: build clean

build: cppfiles
	$(CC) $(CFLAGS) $(INCLUDES) -o napfft *.o $(LDFLAGS)

cppfiles: $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $(SRC) $(LDFLAGS)

clean:
	rm -f *.o core