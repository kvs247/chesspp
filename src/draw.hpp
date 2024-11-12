#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

#include "config.hpp"
#include "constants.hpp"
#include "game.hpp"
#include "pgn.hpp"
#include "types.hpp"
#include "utils.hpp"

using std::cout;

const int BORDER_WIDTH = 3; // at least 2
const size_t MOVE_LIST_ITEM_WIDTH = 20;

winsize getWindowDimensions()
{
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w;
}

std::vector<std::string> makeGameBoardLines(const PiecePlacement &piecePlacement, const int squareWidth,
                                            const int squareHeight)
{
  std::vector<std::string> res;
  char c;

  for (int row = 0; row <= 8 * squareHeight; ++row)
  {
    std::stringstream line;

    const bool drawRank = row % squareHeight == squareHeight / 2;

    line << ' ';
    if (drawRank)
    {
      int rank = 8 - row / squareHeight;
      line << std::to_string(rank) << std::string(BORDER_WIDTH - 2, ' ');
    }
    else
    {
      line << std::string(BORDER_WIDTH - 1, ' ');
    }

    for (int col = 0; col <= 8 * squareWidth; ++col)
    {
      const bool drawHorizLine = row % squareHeight == 0;
      const bool drawVertLine = col % squareWidth == 0;
      const bool drawPiece = col % (squareWidth / 2) == 0 && row % (squareHeight / 2) == 0;

      if (drawHorizLine && drawVertLine)
      {
        c = '+';
      }
      else if (drawHorizLine)
      {
        c = '-';
      }
      else if (drawVertLine)
      {
        c = '|';
      }
      else if (drawPiece)
      {
        int pieceIndex = row / squareHeight * 8 + col / squareWidth;
        c = chessPieceToChar(piecePlacement[pieceIndex]) ?: ' ';
      }
      else
      {
        c = ' ';
      }
      line << c;
    }
    res.push_back(line.str());
  }

  // files
  std::stringstream line;
  line << std::string(BORDER_WIDTH / 2 - 1, '\n') << std::string(BORDER_WIDTH, ' ');
  for (int i = 0; i <= 8 * squareWidth + 2; i++)
  {
    if ((i + squareWidth / 2) % squareWidth == 0)
    {
      const char c = i / squareWidth + 'A';
      line << c;
    }
    else
      line << ' ';
  }
  res.push_back(line.str());

  return res;
}

std::string makeMessage(const std::string message, const int windowWidth)
{
  std::stringstream res("\n\n");
  const auto messageSize = message.size();
  if (!messageSize)
  {
    return res.str();
  }

  res << std::string(windowWidth / 2 - (message.size() / 2), ' ') << message << "\n\n";
  return res.str();
}

std::vector<std::string> makeMoveListEntries(const Game &game)
{
  std::vector<std::string> res;

  for (auto it = game.moveList.cbegin(); it != game.moveList.cend(); ++it)
  {
    const auto moveListItem = *it;
    std::stringstream item;

    const auto fromIndex = moveListItem.fromIndex;
    const auto fromPiece = moveListItem.fromPiece;

    const bool isPawn = fromPiece == ChessPiece::BlackPawn || fromPiece == ChessPiece::WhitePawn;
    const bool isCapture = moveListItem.toPiece != ChessPiece::Empty || moveListItem.isEnPassantCapture;
    const bool isPromotion = moveListItem.promotionPiece != ChessPiece::Empty;
    const bool isOpponentInCheck = moveListItem.isOpponentInCheck;

    if (!moveListItem.castlingString.empty())
    {
      res.push_back(moveListItem.castlingString);
      continue;
    }

    if (!isPawn)
    {
      const char piece = std::toupper(chessPieceToChar(moveListItem.fromPiece));
      item << piece;
      if (!moveListItem.samePieceIndexes.empty())
      {
        const auto [pieceFile, pieceRank] = indexToFileRank(fromIndex);
        bool isDiffFile = false;
        bool isDiffRank = false;

        for (auto &idx : moveListItem.samePieceIndexes)
        {
          const auto [file, rank] = indexToFileRank(idx);
          if (file != pieceFile)
          {
            isDiffFile = true;
          }
          if (rank != pieceRank)
          {
            isDiffRank = true;
          }
        }

        const auto pieceSquare = indexToAlgebraic(fromIndex);
        if (isDiffFile)
        {
          item << pieceSquare[0];
        }
        if (isDiffRank)
        {
          item << pieceSquare[1];
        }
      }
    }

    if (isCapture)
    {
      if (isPawn)
      {
        item << indexToAlgebraic(moveListItem.fromIndex)[0];
      }
      item << 'x';
    }

    item << indexToAlgebraic(moveListItem.toIndex);

    if (isPromotion)
    {
      const char promotionChar = std::toupper(chessPieceToChar(moveListItem.promotionPiece));
      item << '=' << promotionChar;
    }

    if (isOpponentInCheck)
    {
      char c = '+';

      const auto lastEntry = it == --game.moveList.cend();
      if (lastEntry && game.isGameOver == true)
      {
        c = '#';
      }

      item << c;
    }

    res.push_back(item.str());
  }

  return res;
}

size_t numDigits(size_t x)
{
  size_t res = 1;
  while (x / 10 != x)
  {
    x /= 10;
    ++res;
  }
  return res;
}

std::vector<std::string> makeMoveListLines(const std::vector<std::string> &moveListEntries, const size_t moveListLength)
{
  std::vector<std::string> res;
  size_t counter = 1;
  size_t moveListEntriesIdx = 0;

  while (moveListEntriesIdx < moveListEntries.size())
  {
    std::stringstream line;
    size_t n = 0;

    while (moveListEntriesIdx + moveListLength * n * 2 < moveListEntries.size())
    {
      std::stringstream entry;
      size_t j = moveListEntriesIdx + moveListLength * n * 2;
      const size_t index = counter + moveListLength * n;

      entry << index << ". " << moveListEntries[j];
      if (j + 1 < moveListEntries.size())
      {
        entry << " " << moveListEntries[j + 1];
      }

      entry << std::string(MOVE_LIST_ITEM_WIDTH - entry.str().size(), ' ');
      line << entry.str();
      ++n;
    }

    moveListEntriesIdx += 2;
    ++counter;
    res.push_back(line.str());
  }

  return res;
}

void clearScreen() { cout << "\033[2J\033[H"; }

void draw(const Game &game)
{
  const auto [windowHeight, windowWidth, windowXPixel, windowYPixel] = getWindowDimensions();
  // squareWidth must be divisible by 4 so that squareHeight = (squareWidth / 2) is even
  int squareWidth = ((windowWidth / 2) - BORDER_WIDTH) / 8 & ~3;
  squareWidth = std::max(squareWidth, 4);
  int squareHeight = squareWidth / 2 & ~1;

  std::stringstream buffer;

  const auto piecePlacement = game.getPiecePlacement();

  const auto gameBoardLines = makeGameBoardLines(piecePlacement, squareWidth, squareHeight);
  const auto moveListEntries = makeMoveListEntries(game);
  const auto moveListLines = makeMoveListLines(moveListEntries, gameBoardLines.size() - 3);

  size_t j = 0;
  for (size_t i = 0; i < gameBoardLines.size(); ++i)
  {
    buffer << gameBoardLines[i];
    if (config.showMoveList)
    {
      if (i != 0 && i <= gameBoardLines.size() - 3)
      {
        if (j < moveListLines.size())
        {
          buffer << "  " << moveListLines[j];
          ++j;
        }
      }
    }
    buffer << "\n";
  }

  buffer << makeMessage(game.message, windowWidth);

  clearScreen();
  std::cout << buffer.str();

  writePgn(moveListEntries);
}
