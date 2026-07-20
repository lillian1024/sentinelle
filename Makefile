BUILD_DIR=build
TARGET=Sentinelle
BUILD_TYPE=RELEASE

.PHONY: build debug config run clean compile-commands

build: config
	cmake --build $(BUILD_DIR) -j\$(nproc)

debug: BUILD_TYPE=DEBUG
debug: config
	cmake --build $(BUILD_DIR) -j\$(nproc)

config:
	cmake -D CMAKE_BUILD_TYPE=$(BUILD_TYPE) -D CMAKE_EXPORT_COMPILE_COMMANDS=ON -B $(BUILD_DIR)

run: build
	./$(BUILD_DIR)/$(TARGET)

compile-commands: config
	bear -- make -C $(BUILD_DIR)

clean:
	$(RM) -r $(BUILD_DIR)
