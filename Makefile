#Target
TARGET			= hyp-fw-upgrader
# Flags
CXXFLAGS		= -std=c++11 -O2 -MD -Wall -Wextra -g -fPIC
DEFINES         = -D__KERNEL__ -D__LINUX__
# Compiler
CXX				= g++

# Define directories
BUILD_DIR		= ./build

CFLAGS			= -I. -Iinclude -Isrc/include -I$(ROOT)/common

# Files
SOURCES			+= \
	src/main.cpp \
	src/include/StorageKitAlignedBuffer.cpp \
	src/include/StorageKitBufferParser.cpp \
	src/include/StorageKitParser.cpp \
	src/include/StorageKitStorageDeviceUtils.cpp \
	src/include/StorageKitStringUtility.cpp \
	src/descriptor/StorageKitCommandDesc.cpp \
	src/descriptor/StorageKitAtaCommandDesc.cpp \
	src/descriptor/StorageKitScsiCommandDesc.cpp \
	src/descriptor/StorageKitNvmeCommandDesc.cpp \
	src/descriptor/StorageKitU9VcCommandDesc.cpp \
	src/storage/StorageKitAtaDevice.cpp \
	src/storage/StorageKitSatDevice.cpp \
	src/storage/StorageKitHypDevice.cpp \
	src/storage/StorageKitScsiDevice.cpp \
	src/storage/StorageKitStorageDevice.cpp \
	src/storage/StorageKitNvmeDevice.cpp \
	src/protocol/linux/StorageKitAtaProtocol.cpp \
	src/protocol/linux/StorageKitScsiProtocol.cpp \
	src/protocol/linux/StorageKitStorageProtocol.cpp \
	src/protocol/linux/StorageKitNvmeProtocol.cpp \
	src/protocol/StorageKitScsiProtocolCommon.cpp \
	src/protocol/StorageKitNvmeProtocolCommon.cpp \

OBJECTS			= $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

.PHONY: dirs all clean
all: dirs $(BUILD_DIR)/$(TARGET)

dirs:
	@echo "Create directories"
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(dir $(OBJECTS))

$(BUILD_DIR)/%.o: %.cpp
	@echo "Compiling $< --> $@"
	$(CXX) -c -o $@ $< $(CXXFLAGS) $(CFLAGS) $(DEFINES)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	@echo "Linking: $@"
	@$(CXX) -o $@ $^ $(LFLAGS) $(DEFINES) $(LIBS)
	@echo "Library file: $@"

clean:
	@echo "Clean objects"
	@rm -rf $(BUILD_DIR)
	@rm -rf $(dir $(OBJECTS))
