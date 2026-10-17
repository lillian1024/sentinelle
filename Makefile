BUILD_DIR=build
TARGET=Sentinelle
BUILD_TYPE=RELEASE

.PHONY: build debug config run clean compile-commands docker

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

docker:
	docker build -t lilian1024/sentinelle:0.0.1 .
	docker build --build-arg BASE_IMAGE=lilian1024/opencv:4.x-cuda13 -t lilian1024/sentinelle:0.0.1-cuda13 .
	docker build --build-arg BASE_IMAGE=lilian1024/opencv:4.x-cuda12.9 --build-arg ENABLE_ADDITIONAL_GCC=true -t lilian1024/sentinelle:0.0.1-cuda12.9 .

clean:
	$(RM) -r $(BUILD_DIR)
