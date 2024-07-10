CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS = -lncurses

TARGET = build/chess

$(TARGET): src/main.cpp src/render.cpp
	$(CXX) $(CXXFLAGS) src/main.cpp src/render.cpp -o $(TARGET) $(LDFLAGS)

clean:
	rm -f /build