CC := g++
SRC_DIR := src
OBJ_DIR := build/objs
BIN_DIR := build
TOOLS_SRCDIR := tools
TOOLS_BINDIR := build/tools
EXECUTABLE := $(BIN_DIR)/titan

SRCS := $(shell find $(SRC_DIR) -type f -name "*.cpp")
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
TOOLS_SRC := $(shell find $(TOOLS_SRCDIR) -type f -name "*.cpp")
TOOLS_EXEC := $(patsubst $(TOOLS_SRCDIR)/%.cpp,$(TOOLS_BINDIR)/%,$(TOOLS_SRC))
CFLAGS = -Wall -g -I src
LDFLAGS :=
LIBS :=

ifeq ($(OS),Windows_NT)
	CFLAGS += -DWINDOWS
	LIBS += -static $(shell pkg-config --libs --static sdl2 libpng)
else
	LIBS += -lSDL2 -lSDL2main -lpng
endif

.PHONY: all clean compile-tools run-tools compile

all:
	@make compile-tools --silent
	@make run-tools --silent
	@make compile --silent

compile-tools: $(TOOLS_EXEC)

$(TOOLS_BINDIR)/%: $(TOOLS_SRCDIR)/%.cpp
	@printf "\033[1m\033[32mCompiling \033[36m$< \033[32m-> \033[36m$@\033[0m\n"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $< -o $@

run-tools:
	@for tool in $(TOOLS_EXEC); do \
		printf "\033[1m\033[32mRunning tool \033[36m$$tool\033[0m\n"; \
		$$tool; \
	done

compile: $(EXECUTABLE)
$(EXECUTABLE): $(OBJS)
	@printf "\033[1m\033[32mLinking \033[36m$(EXECUTABLE)\033[0m\n"
	@mkdir -p $(BIN_DIR)
	@$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@printf "\033[1m\033[32mCompiling \033[36m$< \033[32m-> \033[36m$@\033[0m\n"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR)

-include $(OBJS:.o=.d)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MM -MT $(@:.d=.o) $< -o $@

-include $(OBJS:.o=.d)
