BUILD_DIR=build
TARGET=Sentinelle
BUILD_TYPE=RELEASE

.PHONY: build debug config run clean

build: config
	cmake --build $(BUILD_DIR)

debug: BUILD_TYPE=DEBUG
debug: config
	cmake --build $(BUILD_DIR)

config:
	cmake -D CMAKE_BUILD_TYPE=$(BUILD_TYPE) -B $(BUILD_DIR)

run: build
	./$(BUILD_DIR)/$(TARGET)

clean:
	$(RM) -r $(BUILD_DIR)
