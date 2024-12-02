#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>
#include <memory>

#include "../config.hpp"
#include "../game.hpp"
#include "../logger.hpp"
#include "../pgn.hpp"
#include "../types.hpp"
#include "../utils.hpp"
#include "frameBuilder.hpp"
#include "renderer.hpp"

const int BORDER_WIDTH = 3; // at least 2
const size_t MOVE_LIST_ITEM_WIDTH = 20;

Renderer::Renderer()
{
  initScreen();
  frameBuilder = std::make_unique<FrameBuilder>();
}

Renderer::~Renderer()
{
  cleanupScreen();
}

winsize Renderer::getWindowDimensions()
{
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w;
}

void Renderer::initScreen()
{
  logger.log("initScreen()");
  // enter alternate screen buffer (for full-screen terminal applications)
  std::cout << "\033[?1049h";
  // clear terminal
  std::cout << "\033[2J";
  // hide cursor
  std::cout << "\033[?25l";
}

void Renderer::cleanupScreen()
{
  logger.log("cleanupScreen()");

  const auto window = getWindowDimensions();

  // move cursor to bottom of content
  std::cout << "\033[" << (window.ws_row + 1) << ";1H";
  // show cursor
  std::cout << "\033[?25h";

  std::cout << std::flush;
}

void Renderer::draw(const Game &game)
{
  const auto [windowHeight, windowWidth, windowXPixel, windowYPixel] = getWindowDimensions();
  // squareWidth must be divisible by 4 so that squareHeight = (squareWidth / 2) is even
  int squareWidth = ((windowWidth / 2) - BORDER_WIDTH) / 8 & ~3;
  squareWidth = std::max(squareWidth, 4);
  int squareHeight = squareWidth / 2 & ~1;
  int boardWidth = BORDER_WIDTH + 8 * squareWidth + 1 + 1; // ... + last column + space between board & move list
  int moveListWidth = windowWidth - boardWidth;
  int boardHeight = 8 * squareHeight + 1;

  const auto piecePlacement = game.getPiecePlacement();

  const auto gameBoardLines = frameBuilder->makeGameBoardLines(piecePlacement, squareWidth, squareHeight);
  const auto moveListEntries = frameBuilder->makeMoveListEntries(game);
  const auto moveListLines = frameBuilder->makeMoveListLines(moveListEntries, gameBoardLines.size() - 3, moveListWidth,
                                                            boardHeight);

  std::vector<std::string> outputLines;
  outputLines.reserve(windowHeight);

  outputLines.push_back(frameBuilder->makeBlackInfoString(game, boardWidth));

  size_t j = 0;
  for (size_t i = 0; i < gameBoardLines.size(); ++i)
  {
    std::string line = gameBoardLines[i];
    if (config.showMoveList)
    {
      if (i != 0 && i <= gameBoardLines.size() - 3)
      {
        if (j < moveListLines.size())
        {
          line += "  ";
          line += moveListLines[j];
          ++j;
        }
      }
    }
    outputLines.push_back(line);
  }

  outputLines.push_back(frameBuilder->makeWhiteInfoString(game, boardWidth));

  outputLines.push_back(frameBuilder->makeMessage(game, windowWidth));

  frameBuilder->addInformationModal(outputLines, boardHeight, windowHeight, windowWidth);

  render(outputLines);

  writePgn(moveListEntries);
}

void Renderer::render(const std::vector<std::string> &lines)
{
  std::stringstream frame;

  frame << "\033[H"; // reset cursor

  // position and write each line
  for (size_t i = 0; i < lines.size(); ++i)
  {
    frame << "\033[" << (i + 1) << ";1H";
    frame << lines[i] << "\033[K";
  }

  std::cout << frame.str() << std::flush;
}
