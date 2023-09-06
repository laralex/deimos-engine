BUILD_DIR = ./build

ENGINE_BASENAME ?= dei
ENGINE_CORE_OUTNAME ?= lib$(ENGINE_BASENAME).so
ENGINE_CORE_SRC_ROOT ?= ./engine/core
ENGINE_PIL_OUTNAME ?= lib$(ENGINE_BASENAME)Platform.so
ENGINE_PIL_SRC_ROOT ?= ./engine/platform
EDITOR_OUTNAME ?= Editor_DevelEngine.exe

CFLAGS = $(if $(DEBUG),-O0 -g, -O2) -std=c++17

LDFLAGS_EDITOR = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -L$(BUILD_DIR)/$(ENGINE_PIL_OUTNAME)
INCLUDES_EDITOR = -I./vendor/glm -I./vendor/cr -I./engine/platform/include

LDFLAGS_ENGINE = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
INCLUDES_ENGINE = -I./vendor/glm -I./vendor/cr -I$(ENGINE_PIL_SRC_ROOT)/include -I$(ENGINE_CORE_SRC_ROOT)/include

$(BUILD_DIR)/$(EDITOR_OUTNAME): $(BUILD_DIR)/$(ENGINE_PIL_OUTNAME) editor/EngineHotLoadHost.cpp
	clang++ $(CFLAGS) -o $@ $^ $(LDFLAGS_EDITOR) $(INCLUDES_EDITOR)

ENGINE_PIL_SRC := $(ENGINE_PIL_SRC_ROOT)/Util.cpp
ENGINE_PIL_SRC += $(ENGINE_PIL_SRC_ROOT)/Window.cpp
$(BUILD_DIR)/$(ENGINE_PIL_OUTNAME): $(ENGINE_PIL_SRC)
	clang++ $(CFLAGS) -shared -fpic -o $@ $^ $(LDFLAGS_ENGINE) -I$(ENGINE_PIL_SRC_ROOT)/include

ENGINE_CORE_SRC := $(ENGINE_CORE_SRC_ROOT)/Camera.cpp
ENGINE_CORE_SRC += $(ENGINE_CORE_SRC_ROOT)/HotLoadGuest.cpp
ENGINE_CORE_SRC += $(ENGINE_CORE_SRC_ROOT)/DevelApp.cpp
$(BUILD_DIR)/$(ENGINE_CORE_OUTNAME): $(ENGINE_CORE_SRC)
	clang++ $(CFLAGS) -shared -fpic -o $@ $^ $(LDFLAGS_ENGINE) $(INCLUDES_ENGINE)

.PHONY: dei
dei: $(BUILD_DIR)
	$(MAKE) --always-make $(BUILD_DIR)/$(ENGINE_CORE_OUTNAME)

.PHONY: run
run: $(BUILD_DIR) $(BUILD_DIR)/$(ENGINE_PIL_OUTNAME) $(BUILD_DIR)/$(ENGINE_CORE_OUTNAME) $(BUILD_DIR)/$(EDITOR_OUTNAME)
	$(BUILD_DIR)/$(EDITOR_OUTNAME) $(shell pwd)/$(BUILD_DIR) $(ENGINE_BASENAME)

.PHONY: rm
rm:
	rm -rf $(BUILD_DIR)/$(subst .,*.,$(ENGINE_CORE_OUTNAME)) \
			 $(BUILD_DIR)/$(subst .,*.,$(ENGINE_PIL_OUTNAME)) \
			 $(BUILD_DIR)/$(subst .,*.,$(EDITOR_OUTNAME))


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
