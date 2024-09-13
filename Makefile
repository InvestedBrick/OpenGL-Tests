CXX = g++
CXX_FLAGS = -pthread -lglfw -lGLEW -lGLU -lglut -lGL -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lrt -ldl
LOCAL_INCLUDE_DIR = ./Simple_API
SRC_FILES = $(wildcard $(LOCAL_INCLUDE_DIR)/*.cpp)
MAIN_FILE = main.cpp
TARGET = Window

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	$(CXX) -o $(TARGET) $(SRC_FILES) $(MAIN_FILE) $(CXX_FLAGS) -I$(LOCAL_INCLUDE_DIR)

clean: 
	rm -f $(TARGET)

.PHONY: all clean
