# TODO NDEBUG
BUILD_DIR ?= ./build
MKDIR_P ?= mkdir -p

CXXFLAGS += -std=c++17 -pedantic -Wall -Wextra -O0 -g
INCLUDE += -I include
DEFINE +=
CPPFLAGS += $(DEFINE) $(INCLUDE) -MMD -MP

LIBSOSIM_SRC := $(shell find lib -name *.cpp -or -name *.c)
LIBSOSIM_OBJ := $(LIBSOSIM_SRC:%=$(BUILD_DIR)/%.o)
LIBSOSIM_DEP := $(LIBSOSIM_OBJ:.o=.d)

SIMULATOR_SRC := $(shell find src/simulator -name *.cpp -or -name *.c)
SIMULATOR_OBJ := $(SIMULATOR_SRC:%=$(BUILD_DIR)/%.o)
SIMULATOR_DEP := $(SIMULATOR_OBJ:.o=.d)

all: simulator

libsosim.a: $(LIBSOSIM_OBJ)
	ar rcs $@ $(LIBSOSIM_OBJ)

simulator: $(SIMULATOR_OBJ) libsosim.a
	$(CXX) $(SIMULATOR_OBJ) -o $@ -L. -lsosim

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) libsosim.a
	$(RM) simulator

tidy:
	clang-tidy $(LIBSOSIM_SRC) $(SIMULATOR_SRC) -- $(INCLUDE) $(DEFINE) $(CXXFLAGS)

format:
	find -name '*.cpp' -o -name '*.hpp' -o -name '*.c' -o -name '*.h' \
		| xargs clang-format -i

-include $(LIBSOSIM_DEP)
-include $(SIMULATOR_DEP)

.PHONY: all clean tidy format
