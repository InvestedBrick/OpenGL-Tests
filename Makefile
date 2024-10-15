CXX = g++
CXXFLAGS = -I$(LOCAL_INCLUDE_DIR) -I$(SHAPES_INCLUDE_DIR)
LDFLAGS = -pthread -lglfw -lGLEW -lGLU -lGL -lrt -ldl -lXrandr -lXxf86vm -lXi -lXinerama -lX11
LOCAL_INCLUDE_DIR = ./Simple_API
SHAPES_INCLUDE_DIR = $(LOCAL_INCLUDE_DIR)/Shapes
BARNES_HUT_INCLUDE_DIR = ./Barnes-Hut
SRC_FILES = $(wildcard $(LOCAL_INCLUDE_DIR)/*.cpp) $(wildcard $(SHAPES_INCLUDE_DIR)/*.cpp) $(wildcard $(BARNES_HUT_INCLUDE_DIR)/Quadtree.cpp)
MAIN_FILE = main.cpp
TARGET = Simulation

all: $(TARGET)

$(TARGET): $(MAIN_FILE) $(SRC_FILES)
	$(CXX) -o $(TARGET) $(MAIN_FILE) $(SRC_FILES) $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

build: 
	make clean
	make

.PHONY: all clean build
