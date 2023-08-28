CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
INCLUDES = -I./vendor/glm -I./engine/include
BUILD_DIR = ./build

$(BUILD_DIR)/VulkanTest: app/main.cpp
	clang++ $(CFLAGS) -o $(BUILD_DIR)/VulkanTest app/main.cpp $(LDFLAGS) $(INCLUDES)

.PHONY: run
run: $(BUILD_DIR) $(BUILD_DIR)/VulkanTest
	$(BUILD_DIR)/VulkanTest

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)/VulkanTest


.PHONY: install_linux
install_linux: vendor/glm
	sudo apt update
	sudo apt install vulkan-tools libvulkan-dev vulkan-validationlayers-dev
	sudo apt install libglfw3-dev

vendor/glm:
	wget https://github.com/g-truc/glm/releases/download/0.9.9.7/glm-0.9.9.7.zip \
		-O /tmp/glm.zip && mkdir -p ./vendor && unzip /tmp/glm.zip -d ./vendor/

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
