CXX = g++
CXX_FLAGS = -pthread -lglfw -lGLEW -lGLU -lglut -lGL -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lrt -ldl
LOCAL_INCLUDE_DIR = ./Simple_API
SHAPES_INCLUDE_DIR = $(LOCAL_INCLUDE_DIR)/Shapes
SRC_FILES = $(wildcard $(LOCAL_INCLUDE_DIR)/*.cpp) $(wildcard $(SHAPES_INCLUDE_DIR)/*.cpp)
MAIN_FILE = main.cpp
TARGET = Window

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	$(CXX) -o $(TARGET) $(MAIN_FILE) $(SRC_FILES) $(CXX_FLAGS) -I $(LOCAL_INCLUDE_DIR)

clean: 
	rm -f $(TARGET)

build: 
	make clean
	make

.PHONY: all clean build
