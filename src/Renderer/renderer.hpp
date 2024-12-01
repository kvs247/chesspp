#pragma once

#include <stddef.h>
#include <string>
#include <sys/ioctl.h>
#include <vector>

#include "../game.hpp"
#include "../timeControl.hpp"
#include "../types.hpp"

// class Game;
// class TimeControl;

class Renderer
{
public:
  void initScreen();
  void cleanupScreen();
  void draw(const Game &);

  Renderer();
  ~Renderer();

private:
  int x;

  void render(const std::vector<std::string> &);
  winsize getWindowDimensions();
  std::vector<std::string> makeGameBoardLines(const PiecePlacement &, const int squareWidth, const int squareHeight);
  std::string makeMessage(const Game &, const int windowWidth);
  std::string handleAmbiguousMove(BoardIndex fromIndex, std::vector<BoardIndex> otherIndexes);
  std::vector<std::string> makeMoveListEntries(const Game &);
  size_t numDigits(size_t);
  std::vector<std::string> makeMoveListLines(const std::vector<std::string> &moveListEntries,
                                             const size_t moveListLength, const int moveListWidth,
                                             const int moveListHeight);
  std::string makeInfoString(const std::string username, const TimeControl, const int boardWidth);
  std::string makeBlackInfoString(const Game &, const int boardWidth);
  std::string makeWhiteInfoString(const Game &, const int boardWidth);
  std::vector<std::string> makeInformationModalLines(const int height, const int width);
  void addInformationModal(std::vector<std::string> &outputLines, const int boardHeight,
                           const unsigned short windowHeight, const unsigned short windowWidth);
};
