#pragma once

#include <string>
#include <vector>

#include "../game.hpp"
#include "../types.hpp"
#include "../timeControl.hpp"

// class Game;

class FrameBuilder
{
public:
  FrameBuilder() = delete;
  FrameBuilder(Game &);

  std::vector<std::string> makeGameBoardLines(const PiecePlacement &, const int squareWidth, const int squareHeight);
  std::string makeMessage(const int windowWidth);
  std::string handleAmbiguousMove(BoardIndex fromIndex, std::vector<BoardIndex> otherIndexes);
  std::vector<std::string> makeMoveListEntries();
  std::vector<std::string> makeMoveListLines(const std::vector<std::string> &moveListEntries,
                                             const size_t moveListLength, const int moveListWidth,
                                             const int moveListHeight);
  std::string makeInfoString(const std::string username, const TimeControl, const int boardWidth);
  std::string makeBlackInfoString(const int height);
  std::string makeWhiteInfoString(const int height);
  std::vector<std::string> makeInformationModalLines(const int height, const int width);
  void addInformationModal(std::vector<std::string> &outputLines, const int boardHeight,
                           const unsigned short windowHeight, const unsigned short windowWidth);

private:
  Game &game;
};