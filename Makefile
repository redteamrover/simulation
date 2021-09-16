
vpath %.cpp src

CXX ?= g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Weffc++ -Wpedantic -Og -ggdb
CPPFLAGS := -D_GNU_SOURCE
LD ?= ld
LDFLAGS := -Wl,-O1,--relax,--as-needed
LIBS := -lboost_program_options -lsqlite3

ifeq ($(CXX),g++)
    CXXFLAGS := $(CXXFLAGS) -fanalyzer
endif

MKDIR := mkdir -p
RM := rm -f

SRCS := $(notdir $(wildcard src/*.cpp))
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

TARGET := simulation

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	$(RM) $(OBJS) $(TARGET)

