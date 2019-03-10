BUILD_DIR ?= ./build
MKDIR_P ?= mkdir -p

CXXFLAGS += -std=c++17 -pedantic -Wall -Wextra -O0 -g
INCLUDE += -I include
DEFINE +=
CPPFLAGS += $(DEFINE) $(INCLUDE) -MMD -MP

LIBOSSIM_SRC := $(shell find lib -name *.cpp)
LIBOSSIM_OBJ := $(LIBOSSIM_SRC:%=$(BUILD_DIR)/%.o)
LIBOSSIM_DEP := $(LIBOSSIM_OBJ:.o=.d)

all: libossim.a

libossim.a: $(LIBOSSIM_OBJ)
	ar rcs $@ $(LIBOSSIM_OBJ)

$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) libossim.a

tidy:
	clang-tidy $(LIBOSSIM_SRC) -- $(INCLUDE) $(DEFINE) $(CXXFLAGS)

format:
	find -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i

-include $(LIBOSSIM_DEP)

.PHONY: all clean tidy format
