CXX = g++
CXX_FLAGS = -pthread -lglfw -lGLEW -lGLU -lglut -lGL -lXrandr -lXxf86vm -lXi -lXinerama -lX11 -lrt -ldl
SRC_FILES = main.cpp \
	  Renderer.cpp \
	  IndexBuffer.cpp \
	  VertexBuffer.cpp \
	  VertexArray.cpp \
	  VertexBufferLayout.cpp 

TARGET = Window

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	$(CXX)  -o $(TARGET) $(SRC_FILES) $(CXX_FLAGS)

clean: 
	rm $(TARGET)

.PHONY: all clean