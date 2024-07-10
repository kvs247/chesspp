CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

TARGET = build/chess

$(TARGET): src/main.cpp
	$(CXX) $(CXXFLAGS) src/main.cpp -o $(TARGET) 