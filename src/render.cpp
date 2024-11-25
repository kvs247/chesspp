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
#include "render.hpp"
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

std::string makeMessage(const Game &game, const int windowWidth)
{
  std::stringstream res("\n\n");

  const auto userInputSize = game.userInput.size();
  res << std::string(windowWidth / 2 - (userInputSize / 2), ' ') << game.userInput << "\n";

  const auto messageSize = game.message.size();
  if (!messageSize)
  {
    return res.str();
  }

  res << std::string(windowWidth / 2 - (messageSize / 2), ' ') << game.message << "\n\n";

  return res.str();
}

std::string handleAmbiguousMove(BoardIndex fromIndex, std::vector<BoardIndex> otherIndexes)
{
  const auto [pieceFile, pieceRank] = indexToFileRank(fromIndex);
  bool isSameFile = false;
  bool isSameRank = false;

  for (auto &otherIdx : otherIndexes)
  {
    const auto [otherFile, otherRank] = indexToFileRank(otherIdx);
    if (otherFile == pieceFile)
    {
      isSameFile = true;
    }
    if (otherRank == pieceRank)
    {
      isSameRank = true;
    }
  }

  std::string res;
  const auto pieceSquare = indexToAlgebraic(fromIndex);
  if (isSameRank)
  {
    res += pieceSquare[0];
  }
  if (isSameFile)
  {
    res += pieceSquare[1];
  }

  return res;
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
        item << handleAmbiguousMove(fromIndex, moveListItem.samePieceIndexes);
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

std::vector<std::string> makeMoveListLines(const std::vector<std::string> &moveListEntries, const size_t moveListLength,
                                           const int moveListWidth, const int moveListHeight)
{
  const size_t maxNumEntries = (moveListWidth / MOVE_LIST_ITEM_WIDTH) * (moveListHeight - 2);

  auto formatMoveEntry = [&](const size_t moveNum, const size_t moveListIndex) -> std::string
  {
    std::stringstream entry;
    entry << moveNum << ". " << moveListEntries[moveListIndex];
    if (moveListIndex + 1 < moveListEntries.size())
    {
      entry << " " << moveListEntries[moveListIndex + 1];
    }

    std::string res = entry.str();
    res += std::string(MOVE_LIST_ITEM_WIDTH - res.size(), ' ');
    return res;
  };

  std::vector<std::string> lines(moveListLength);
  for (size_t i = 0; i < std::min(maxNumEntries, (moveListEntries.size() + 1) / 2); ++i)
  {
    const size_t moveListIndex = i * 2;
    if (i == maxNumEntries - 1)
    {
      lines[i % moveListLength] += "...";
      break;
    }

    lines[i % moveListLength] += formatMoveEntry(i + 1, moveListIndex);
  }

  return lines;
}

std::string makeInfoString(const std::string username, const TimeControl timeControl, const int boardWidth)
{
  std::stringstream ss;
  std::string timeString = timeControl.isEnabled ? timeControl.getFormattedTimeString() : "";
  const int numChars = username.length() + timeString.length();
  const int gapSize = boardWidth - numChars - BORDER_WIDTH - 1;
  ss << std::string(BORDER_WIDTH, ' ') << username << std::string(gapSize, ' ') << timeString << " \n";
  return ss.str();
};

std::string makeBlackInfoString(const Game &game, const int boardWidth)
{
  return makeInfoString(config.blackUsername, game.blackTime, boardWidth);
};

std::string makeWhiteInfoString(const Game &game, const int boardWidth)
{
  return makeInfoString(config.whiteUsername, game.whiteTime, boardWidth);
}

void addInformationModal(std::vector<std::string> &outputLines, const int boardHeight,
                         const unsigned short windowHeight, const unsigned short windowWidth)
{
  const int modalWidth = windowWidth / 2;
  const int modalHeight = windowHeight / 2;

  const int modalHeightStart = boardHeight / 2 - modalHeight / 2;
  const int modalHeightEnd = boardHeight / 2 + modalHeight / 2;
  const int modalWidthStart = windowWidth / 2 - modalWidth / 2;
  const int modalWidthEnd = windowWidth / 2 + modalWidth / 2;

  int i = 0;
  for (auto &line : outputLines)
  {
    if (i < modalHeightStart || i > modalHeightEnd)
    {
      ++i;
      continue;
    }

    const std::string prefix = modalWidthStart < line.length() ? line.substr(0, modalWidthStart - 1) : "";
    const std::string suffix = modalWidthEnd < line.length() ? line.substr(modalWidthEnd, line.length()) : "";
    line = prefix + std::string(modalWidth, '~') + suffix;

    ++i;
  }
}

void clearScreen() { cout << "\033[2J\033[H"; }

void draw(const Game &game)
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

  const auto gameBoardLines = makeGameBoardLines(piecePlacement, squareWidth, squareHeight);
  const auto moveListEntries = makeMoveListEntries(game);
  const auto moveListLines = makeMoveListLines(moveListEntries, gameBoardLines.size() - 3, moveListWidth, boardHeight);

  // std::stringstream buffer;
  std::vector<std::string> outputLines;
  outputLines.reserve(windowHeight);

  // buffer << makeBlackInfoString(game, boardWidth);
  outputLines.push_back(makeBlackInfoString(game, boardWidth));

  size_t j = 0;
  for (size_t i = 0; i < gameBoardLines.size(); ++i)
  {
    // buffer << gameBoardLines[i];
    std::string line = gameBoardLines[i];
    if (config.showMoveList)
    {
      if (i != 0 && i <= gameBoardLines.size() - 3)
      {
        if (j < moveListLines.size())
        {
          // buffer << "  " << moveListLines[j];
          line += "  ";
          line += moveListLines[j];
          ++j;
        }
      }
    }
    // buffer << "\n";
    outputLines.push_back(line);
  }

  // buffer << makeWhiteInfoString(game, boardWidth);
  outputLines.push_back(makeWhiteInfoString(game, boardWidth));

  // buffer << makeMessage(game, windowWidth) << "\n";
  outputLines.push_back(makeMessage(game, windowWidth));

  addInformationModal(outputLines, boardHeight, windowHeight, windowWidth);

  std::stringstream buffer;
  for (auto &l : outputLines)
  {
    buffer << l << "\n";
  }

  clearScreen();
  std::cout << buffer.str();

  writePgn(moveListEntries);
}
