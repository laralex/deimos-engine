BUILD_DIR = ./build

CFLAGS = $(if $(DEBUG),-O0 -g, -O2) -std=c++17

LDFLAGS_APP = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi #build/libdei.so -Wl,-rpath,libdei.so
INCLUDES_APP = -I./vendor/glm -I./vendor/cr -I./engine/include

LDFLAGS_ENGINE = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
INCLUDES_ENGINE = -I./vendor/glm -I./vendor/cr -I./engine/include

$(BUILD_DIR)/HotLoader.exe: hotload/Main.cpp
	clang++ $(CFLAGS) -o $@ $^ $(LDFLAGS_APP) $(INCLUDES_APP)

$(BUILD_DIR)/libdeiapp.so: app/HotLoadMain.cpp
	clang++ $(CFLAGS) -shared -fpic -o $@ $^ $(LDFLAGS_APP) $(INCLUDES_APP)

$(BUILD_DIR)/libdei.so: engine/Camera.cpp engine/HotLoadMain.cpp
	clang++ $(CFLAGS) -shared -fpic -o $@ $^ $(LDFLAGS_ENGINE) $(INCLUDES_ENGINE)


.PHONY: app
app: $(BUILD_DIR) $(BUILD_DIR)/libdei.so
	$(MAKE) --always-make $(BUILD_DIR)/libdeiapp.so

.PHONY: dei
dei: $(BUILD_DIR) $(BUILD_DIR)/libdeiapp.so
	$(MAKE) --always-make $(BUILD_DIR)/libdei.so

.PHONY: run
run: $(BUILD_DIR) $(BUILD_DIR)/libdei.so $(BUILD_DIR)/libdeiapp.so $(BUILD_DIR)/HotLoader.exe
	$(BUILD_DIR)/HotLoader.exe $(shell pwd)/$(BUILD_DIR) dei deiapp

.PHONY: rm
rm:
	rm -rf $(BUILD_DIR)/libdei*.so \
			 $(BUILD_DIR)/libdeiapp*.so \
			 $(BUILD_DIR)/HotLoader.exe


.PHONY: install_linux
install_linux: vendor/glm vendor/cr
	sudo apt update
	sudo apt install vulkan-tools libvulkan-dev vulkan-validationlayers-dev
	sudo apt install libglfw3-dev

vendor/cr:
	git submodule add -b master https://github.com/fungos/cr.git vendor/cr

vendor/glm:
#git submodule add -b tags/0.9.9.8 https://github.com/g-truc/glm.git vendor/glm
	wget https://github.com/g-truc/glm/releases/download/0.9.9.7/glm-0.9.9.7.zip \
		-O /tmp/glm.zip && mkdir -p ./vendor && unzip /tmp/glm.zip -d ./vendor/

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
