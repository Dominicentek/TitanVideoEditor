CC := g++
SRC_DIR := src
OBJ_DIR := build/objs
BIN_DIR := build
EXECUTABLE := $(BIN_DIR)/titan

SRCS := $(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
CFLAGS := -Wall -g -I src
LDFLAGS :=
LIBS =
ifeq ($(OS),Windows_NT)
	LIBS += -static $(shell pkg-config --libs --static sdl2)
else
	LIBS += -lSDL2 -lSDL2main
endif

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR)

-include $(OBJS:.o=.d)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MM -MT $(@:.d=.o) $< -o $@

-include $(OBJS:.o=.d)