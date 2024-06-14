BUILD_DIR := build
SRC_DIR := src

CPP_Files := $(shell find $(SRC_DIR) -name '*.cpp' -not -path "*/AlgorithmsTest/*")
H_Files := $(shell find $(SRC_DIR) -name '*.h' -not -path "*/AlgorithmsTest/*")
OBJS := $(CPP_Files:%=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/output: $(OBJS)
	mpicxx $(OBJS) -o $@ -O3 -mavx2

$(BUILD_DIR)/%.cpp.o: %.cpp $(H_Files)
	mkdir -p $(dir $@)
	mpicxx -c $< -o $@ -O3 -mavx2

clean:
	rm -r $(BUILD_DIR)

