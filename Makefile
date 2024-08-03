CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS = -lgtest
# LDFLAGS = -lncurses -lX11

TARGET = build/chess
TEST_TARGET = build/tests

SRC = src/main.cpp src/draw.cpp src/game.cpp src/utils.cpp src/move.cpp src/constants.cpp src/logger.cpp
TEST_SRC = tests/test_utils.cpp src/utils.cpp src/move.cpp src/constants.cpp src/logger.cpp

# main target
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# test target
$(TEST_TARGET): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $(TEST_TARGET) $(LDFLAGS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f /build