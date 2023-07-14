# Based upon https://makefiletutorial.com/#makefile-cookbook
# Thanks to Job Vranish (https://spin.atomicobject.com/2016/08/26/makefile-c-projects/)
EXEC_EDITOR := pixedit
EXEC_VIEWER := pixview
UNIT_TEST := pixedit_tests

BUILD_DIR := build
SRC_DIRS := src external
TEST_DIRS := test
SCRATCH_DIR := scratch

# Configurations...
DEBUG ?= 1
OPTIMIZE ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS := -DDEBUG $(CFLAGS)
    CXXFLAGS := -DDEBUG $(CXXFLAGS)
else
    CFLAGS := -DNDEBUG $(CFLAGS)
    CXXFLAGS := -DNDEBUG $(CXXFLAGS)
endif
ifeq ($(OPTIMIZE), 1)
    CFLAGS := -O2 $(CFLAGS)
    CXXFLAGS := -O2 $(CXXFLAGS)
endif

# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. The shell will incorrectly expand these otherwise, but we want to send the * directly to the find command.
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')
SCRATCH_SRCS := $(shell find $(SCRATCH_DIR) -name '*.cpp' -or -name '*.c' -or -name '*.s')
NON_DEFAULT_SRCS := src/ViewerApp.cpp src/EditorApp.cpp $(SCRATCH_SRCS)
TEST_SRCS := $(shell find $(TEST_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')

# Prepends BUILD_DIR and appends .o to every src file
# As an example, ./your_dir/hello.cpp turns into ./build/./your_dir/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o) $(SCRATCH_SRCS:%=$(BUILD_DIR)/%.o)
NON_DEFAULT_OBJS := $(NON_DEFAULT_SRCS:%=$(BUILD_DIR)/%.o)
LDFLAGS := $(LDFLAGS)  $(shell sdl2-config --libs) -lSDL2_image
TEST_OBJS := $(TEST_SRCS:%=$(BUILD_DIR)/%.o)

# This is for creating temporary testing programs
SCRATCH := $(SCRATCH_SRCS:%=$(BUILD_DIR)/%.out)

# String substitution (suffix version without %).
# As an example, ./build/hello.cpp.o turns into ./build/hello.cpp.d
DEPS := $(OBJS:.o=.d) $(TEST_OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS)) $(shell sdl2-config --cflags)

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CPPFLAGS := $(INC_FLAGS) -g -MMD -MP -Wall

# # Use latest C++ version
CXXFLAGS := $(CXXFLAGS) -std=gnu++20

# ALL
all: $(BUILD_DIR)/$(EXEC_EDITOR) $(BUILD_DIR)/$(UNIT_TEST) scratch
scratch: $(SCRATCH)

# Removing non default objs
DEFAULT_OBJS := $(filter-out $(NON_DEFAULT_OBJS),$(OBJS))

# The final build step for our editor app.
$(BUILD_DIR)/$(EXEC_EDITOR): $(DEFAULT_OBJS) $(BUILD_DIR)/src/EditorApp.cpp.o
	$(CXX) $^ -o $@ $(LDFLAGS)

# The final build step for our viewer app. Removed temporarily
# $(BUILD_DIR)/$(EXEC_VIEWER): $(DEFAULT_OBJS) $(BUILD_DIR)/src/ViewerApp.cpp.o
# 	$(CXX) $^ -o $@ $(LDFLAGS)

# Unit tests
$(BUILD_DIR)/$(UNIT_TEST): $(DEFAULT_OBJS) $(TEST_OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

# Scratch
$(SCRATCH): %.out: %.o $(DEFAULT_OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@ 


.PHONY: clean all scratch
clean:
	rm -r $(BUILD_DIR)

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
