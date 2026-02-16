BUILD_DIR = build

all:
	cmake --build $(BUILD_DIR)

clean:
	cmake --build $(BUILD_DIR) --target clean

rebuild:
	rm -rf $(BUILD_DIR)
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)

run:
	./$(BUILD_DIR)/Nixman

.PHONY: all clean rebuild run
