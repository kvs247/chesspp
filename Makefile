CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
LDFLAGS = -lgtest
# LDFLAGS = -lncurses -lX11

TARGET = build/chess
TEST_TARGET = build/tests

SRC = src/main.cpp src/draw.cpp src/game.cpp src/utils.cpp src/piece.cpp src/constants.cpp
TEST_SRC = tests/main.cpp tests/test_utils.cpp tests/test_pieces.cpp \
					 src/game.cpp src/utils.cpp src/piece.cpp src/constants.cpp

# main target
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# test target
$(TEST_TARGET): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $(TEST_TARGET) $(LDFLAGS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f ./build/chess
	rm -f ./build/tests