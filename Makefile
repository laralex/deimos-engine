BUILD_DIR = ./build/$(if $(DEBUG),debug,release)

ENGINE_BASENAME ?= dei
ENGINE_CORE_OUTNAME ?= lib$(ENGINE_BASENAME).so
ENGINE_CORE_SRC_ROOT ?= ./engine/core
ENGINE_CORE_OBJ_ROOT ?= $(BUILD_DIR)/$(ENGINE_CORE_SRC_ROOT)
ENGINE_PLTFM_OUTNAME ?= lib$(ENGINE_BASENAME)Platform.so
ENGINE_PLTFM_SRC_ROOT ?= ./engine/platform
ENGINE_PLTFM_OBJ_ROOT ?= $(BUILD_DIR)/$(ENGINE_PLTFM_SRC_ROOT)
EDITOR_OUTNAME ?= Editor_DevelEngine.exe
EDITOR_SRC_ROOT ?= ./editor
EDITOR_OBJ_ROOT ?= $(BUILD_DIR)/$(EDITOR_SRC_ROOT)

CFLAGS = $(if $(DEBUG),-O0 -g, -O2) -std=c++17

LDFLAGS_EDITOR = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -L$(BUILD_DIR)/$(ENGINE_PLTFM_OUTNAME)
INCLUDES_EDITOR = -I./vendor/glm -I./vendor/cr -I$(ENGINE_PLTFM_SRC_ROOT)/include -I$(ENGINE_CORE_SRC_ROOT)/include

LDFLAGS_ENGINE = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
INCLUDES_ENGINE = -I./vendor/glm -I./vendor/cr -I$(ENGINE_PLTFM_SRC_ROOT)/include -I$(ENGINE_CORE_SRC_ROOT)/include

INCLUDES_PLTFM = -I./vendor/glm -I$(ENGINE_PLTFM_SRC_ROOT)/include

# editor
EDITOR_SRC := EngineHotLoadHost.cpp
EDITOR_OBJ := $(addprefix $(EDITOR_OBJ_ROOT)/, $(EDITOR_SRC:.cpp=.o))
EDITOR_SRC := $(addprefix $(EDITOR_SRC_ROOT)/, $(EDITOR_SRC))
# -- .cpp from source dir -> .o object files in build dir
$(EDITOR_OBJ): $(EDITOR_OBJ_ROOT)/%.o: $(EDITOR_SRC_ROOT)/%.cpp
	mkdir -p $(EDITOR_OBJ_ROOT)
	clang++ $(CFLAGS) -c $< -o $@ $(INCLUDES_EDITOR)

# -- .o from build dir -> executable in build dir
$(BUILD_DIR)/$(EDITOR_OUTNAME): $(EDITOR_OBJ) $(BUILD_DIR)/$(ENGINE_PLTFM_OUTNAME)
	clang++ $(CFLAGS) -o $@ $^ $(LDFLAGS_EDITOR)

# dei_platform
ENGINE_PLTFM_SRC := Util.cpp
ENGINE_PLTFM_SRC += Window.cpp
ENGINE_PLTFM_SRC += Monitor.cpp
ENGINE_PLTFM_OBJ := $(addprefix $(ENGINE_PLTFM_OBJ_ROOT)/, $(ENGINE_PLTFM_SRC:.cpp=.o))
ENGINE_PLTFM_SRC := $(addprefix $(ENGINE_PLTFM_SRC_ROOT)/, $(ENGINE_PLTFM_SRC))
# -- .cpp from source dir -> .o object files in build dir
#$(error @@ $(ENGINE_PLTFM_OBJ) @@ $(ENGINE_PLTFM_SRC))
$(ENGINE_PLTFM_OBJ): $(ENGINE_PLTFM_OBJ_ROOT)/%.o: $(ENGINE_PLTFM_SRC_ROOT)/%.cpp
	mkdir -p $(ENGINE_PLTFM_OBJ_ROOT)
	clang++ $(CFLAGS) -fpic -c $< -o $@ $(INCLUDES_PLTFM)
# -- .o from build dir -> shared lib in build dir
$(BUILD_DIR)/$(ENGINE_PLTFM_OUTNAME): $(ENGINE_PLTFM_OBJ)
	clang++ $(CFLAGS) -shared -fpic -o $@ $^ $(LDFLAGS_ENGINE)

# dei
ENGINE_CORE_SRC := Camera.cpp
ENGINE_CORE_SRC += HotLoadGuest.cpp
ENGINE_CORE_SRC += Entry.cpp
ENGINE_CORE_OBJ := $(addprefix $(ENGINE_CORE_OBJ_ROOT)/, $(ENGINE_CORE_SRC:.cpp=.o))
ENGINE_CORE_SRC := $(addprefix $(ENGINE_CORE_SRC_ROOT)/, $(ENGINE_CORE_SRC))
# -- .cpp from source dir -> .o object files in build dir
$(ENGINE_CORE_OBJ): $(ENGINE_CORE_OBJ_ROOT)/%.o: $(ENGINE_CORE_SRC_ROOT)/%.cpp
	mkdir -p $(ENGINE_CORE_OBJ_ROOT)
	clang++ $(CFLAGS) -fpic -c $< -o $@ $(INCLUDES_ENGINE)
# -- .o from build dir -> shared lib in build dir
$(BUILD_DIR)/$(ENGINE_CORE_OUTNAME): $(ENGINE_CORE_OBJ)
	clang++ $(CFLAGS) -shared -fpic -o $@ $^ $(LDFLAGS_ENGINE)

.PHONY: dei
dei: $(BUILD_DIR)
	$(MAKE) $(BUILD_DIR)/$(ENGINE_CORE_OUTNAME)

.PHONY: build
build: $(BUILD_DIR) $(BUILD_DIR)/$(ENGINE_PLTFM_OUTNAME) $(BUILD_DIR)/$(ENGINE_CORE_OUTNAME) $(BUILD_DIR)/$(EDITOR_OUTNAME)

.PHONY: run
run: build
	$(BUILD_DIR)/$(EDITOR_OUTNAME) $(shell pwd)/$(BUILD_DIR) $(ENGINE_BASENAME)

.PHONY: rm
rm:
	rm -rf $(BUILD_DIR)/$(subst .,*.,$(ENGINE_CORE_OUTNAME)) \
			 $(BUILD_DIR)/$(subst .,*.,$(ENGINE_PLTFM_OUTNAME)) \
			 $(BUILD_DIR)/$(subst .,*.,$(EDITOR_OUTNAME)) \
			 $(BUILD_DIR)/**/*.o \
			 $(BUILD_DIR)/**/*.object \


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
