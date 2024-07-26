CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
# LDFLAGS = -lncurses -lX11

TARGET = build/chess

$(TARGET): src/main.cpp src/draw.cpp
	$(CXX) $(CXXFLAGS) src/main.cpp src/draw.cpp -o $(TARGET) $(LDFLAGS)

clean:
	rm -f /build