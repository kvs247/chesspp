#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "config.hpp"
#include "constants.hpp"
#include "game.hpp"
#include "pgn.hpp"
#include "types.hpp"
#include "utils.hpp"

using std::cout;

int BORDER_WIDTH = 3; // at least 2
int COL_MULT = 8;     // even
int ROW_MULT = 4;     // even

std::vector<std::string> makeGameBoardLines(PiecePlacement &piecePlacement)
{
  std::vector<std::string> res;
  char c;
  size_t pieceIndex = 0;
  for (int row = 0; row <= 8 * ROW_MULT; ++row)
  {
    std::stringstream line;

    // ranks
    line << ' ';
    if ((row + ROW_MULT / 2) % ROW_MULT == 0)
    {
      line << std::to_string(8 - row / ROW_MULT) << std::string(BORDER_WIDTH - 2, ' ');
    }
    else
    {
      line << std::string(BORDER_WIDTH - 1, ' ');
    }

    for (int col = 0; col <= 8 * COL_MULT; ++col)
    {
      bool drawRow = row % ROW_MULT == 0;
      bool drawCol = col % COL_MULT == 0;
      bool drawPiece = col % (COL_MULT / 2) == 0 && row % (ROW_MULT / 2) == 0;

      if (drawRow && drawCol)
      {
        c = '+';
      }
      // horizontal lines
      else if (drawRow)
      {
        c = '-';
      }
      // vertical lines
      else if (drawCol)
      {
        c = '|';
      }
      // pieces
      else if (drawPiece)
      {
        char pieceChar = chessPieceToChar(piecePlacement[pieceIndex]);
        c = pieceChar ? pieceChar : ' ';
        ++pieceIndex;
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
  for (int i = 0; i <= 8 * COL_MULT + 2; i++)
  {
    if ((i + COL_MULT / 2) % COL_MULT == 0)
    {
      char c = i / COL_MULT + 'A';
      line << c;
    }
    else
      line << ' ';
  }
  res.push_back(line.str());

  return res;
}

std::string makeMessage(std::string message)
{
  std::stringstream res("\n\n");
  auto messageSize = message.size();
  if (!messageSize)
  {
    return res.str();
  }

  int boardWidth = 8 * COL_MULT / 2;
  int msgOffest = messageSize / 2 + messageSize % 2;
  int numSpace = BORDER_WIDTH + boardWidth - msgOffest;
  res << std::string(numSpace, ' ')
      << '~'
      << message
      << ((messageSize % 2 == 0) ? ' ' : '~')
      << "\n\n";
  return res.str();
}

std::vector<std::string> makeMoveListEntries(Game &game)
{
  std::vector<std::string> res;

  for (auto &moveListItem : game.moveList)
  {
    std::stringstream item;

    const auto fromIndex = moveListItem.fromIndex;
    const auto toIndex = moveListItem.toIndex;
    const auto fromPiece = moveListItem.fromPiece;
    const auto [fromFile, fromRank] = indexToFileRank(fromIndex);
    const auto [toFile, toRank] = indexToFileRank(toIndex);

    const bool isPawn = fromPiece == ChessPiece::BlackPawn || fromPiece == ChessPiece::WhitePawn;
    const bool isCapture = moveListItem.toPiece != ChessPiece::Empty;
    const bool isKingMove = fromPiece == ChessPiece::BlackKing || fromPiece == ChessPiece::WhiteKing;
    const bool isShortCastle = isKingMove && fromFile == 5 && toFile == 7;
    const bool isLongCastle = isKingMove && fromFile == 5 && toFile == 3;
    const bool isPromotion = moveListItem.promotionPiece != ChessPiece::Empty;
    const bool isOpponentInCheck = moveListItem.isOpponentInCheck;

    if (isShortCastle)
    {
      res.push_back("0-0");
      continue;
    }

    if (isLongCastle)
    {
      res.push_back("O-O-O");
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
      item << "+";
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

size_t moveListEntryLength = 16;

std::vector<std::string> makeMoveListLines(std::vector<std::string> &moveListEntries, size_t moveListLength)
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
      size_t index = counter + moveListLength * n;

      entry << index << ". " << moveListEntries[j];
      if (j + 1 < moveListEntries.size())
      {
        entry << " " << moveListEntries[j + 1];
      }

      entry << std::string(moveListEntryLength - entry.str().size(), ' ');
      line << entry.str();
      ++n;
    }

    moveListEntriesIdx += 2;
    ++counter;
    res.push_back(line.str());
  }

  return res;
}

void draw(Game &game)
{
  std::stringstream buffer;

  auto piecePlacement = game.getPiecePlacement();

  auto gameBoardLines = makeGameBoardLines(piecePlacement);
  auto moveListEntries = makeMoveListEntries(game);
  auto moveListLines = makeMoveListLines(moveListEntries, gameBoardLines.size() - 3);

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

  auto message = game.message;
  buffer << makeMessage(game.message);

  std::cout << buffer.str();

  writePgn(moveListEntries);
}

void clearScreen() { cout << "\033[2J\033[H"; }