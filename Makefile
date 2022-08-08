ifneq ("$(MAKECMDGOALS)", "clean")
ifeq ("$(GOOGLE_TEST_HOME)", "")
$(error "Please define GOOGLE_TEST_HOME, ie. `make GOOGLE_TEST_HOME=~/googletest-release-1.8.0'")
endif
endif

TARGET := test_ring_buffer

CC := g++
GMOCK_INCLUDE_PATH = $(GOOGLE_TEST_HOME)/googlemock
GTEST_INCLUDE_PATH = $(GOOGLE_TEST_HOME)/googletest

GMOCK_LIBRARY_PATH = $(GOOGLE_TEST_HOME)/lib
GTEST_LIBRARY_PATH = $(GOOGLE_TEST_HOME)/lib

# 'override' allows you to pass in extra flags when invoking `make', ie.
# make CPPFLAGS=-std=c++14
override CPPFLAGS += -std=c++17 -W -Wall -g -pthread -I $(GTEST_INCLUDE_PATH)/include -I $(GMOCK_INCLUDE_PATH)/include
override LDFLAGS += -L$(GTEST_LIBRARY_PATH) -L$(GMOCK_LIBRARY_PATH)
override LDLIBS += -lgtest_main -lgtest -lgmock -lpthread

.PHONY : all clean

all: test

$(TARGET).o: $(TARGET).cpp ring_buffer.h

$(TARGET): $(TARGET).o

test: $(TARGET)
	./$<

clean:
	rm -rf *.o $(TARGET)
