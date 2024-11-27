#pragma once

#include <string>
#include <sys/ioctl.h>
#include <vector>

#include "game.hpp"
#include "types.hpp"

winsize getWindowDimensions();

std::vector<std::string> makeGameBoardLines(const PiecePlacement &, const int, const int);

std::string makeMessage(const Game &game, const int windowWidth);

std::string handleAmbiguousMove(BoardIndex, std::vector<BoardIndex>);

std::vector<std::string> makeMoveListEntries(const Game &game);

size_t numDigits(size_t);

std::vector<std::string> makeMoveListLines(const std::vector<std::string> &, const size_t moveListLength, const int,
                                           const int);

std::string makeInfoString(const std::string username, const TimeControl timeControl, const int boardWidth);

std::string makeBlackInfoString(const Game &, const int);

std::vector<std::string> makeInformationModalLines(const int height, const int width);

void initScreen();

void cleanupScreen();

void draw(const Game &);

void render(const std::vector<std::string> &lines);