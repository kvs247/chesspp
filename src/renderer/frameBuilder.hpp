#pragma once

#include <string>
#include <sys/ioctl.h>
#include <vector>

#include "../game.hpp"
#include "../timeControl.hpp"
#include "../types.hpp"

class FrameBuilder
{
public:
  FrameBuilder() = delete;
  FrameBuilder(Game &);

  std::vector<std::string> buildFrame();
  winsize getWindowDimensions();

private:
  Game &game;

  std::vector<std::string> makeGameBoardLines(const int squareWidth, const int squareHeight);
  std::vector<std::string> makeMoveListEntries();
  std::string handleAmbiguousMove(BoardIndex fromIndex, std::vector<BoardIndex> otherIndexes);
  std::vector<std::string> makeMoveListLines(
      const std::vector<std::string> &moveListEntries,
      const size_t moveListLength,
      const int moveListWidth,
      const int moveListHeight);

  std::string makeInfoString(const std::string username, const TimeControl, const int boardWidth);
  std::string makeBlackInfoString(const int height);
  std::string makeWhiteInfoString(const int height);
  std::string makeMessage(const int windowWidth);

  std::vector<std::string>
  makeInformationModalLines(const int height, const int width, const std::vector<std::string> &);
  void addInformationModal(
      const std::vector<std::string> &content,
      std::vector<std::string> &outputLines,
      const int boardHeight,
      const unsigned short windowHeight,
      const unsigned short windowWidth);
  std::vector<std::string> makeTitleLines(const size_t windowWidth);
  std::vector<std::string> makePawnPromotionLines();
  std::vector<std::string> makeHelpScreenLines();
};