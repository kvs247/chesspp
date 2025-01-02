#include <sstream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

#include "../game.hpp"
#include "../logger.hpp"
#include "../pgn.hpp"
#include "../utils.hpp"
#include "config.hpp"
#include "frameBuilder.hpp"

FrameBuilder::FrameBuilder(Game &g) : game(g) {}

std::vector<std::string> FrameBuilder::buildFrame()
{
  const auto [windowHeight, windowWidth, windowXPixel, windowYPixel] = getWindowDimensions();

  // squareWidth must be divisible by 4 so that squareHeight = (squareWidth / 2) is even
  int squareWidth = ((windowWidth / 2) - BORDER_WIDTH) / 8 & ~3;
  squareWidth = std::max(squareWidth, 4);
  int squareHeight = squareWidth / 2 & ~1;
  int boardWidth = BORDER_WIDTH + 8 * squareWidth + 1 + 1; // ... + last column + space between board & move list
  int moveListWidth = windowWidth - boardWidth;
  int boardHeight = 8 * squareHeight + 1;

  const auto gameBoardLines = makeGameBoardLines(squareWidth, squareHeight);
  const auto moveListEntries = makeMoveListEntries();
  const auto moveListLines = makeMoveListLines(moveListEntries, gameBoardLines.size() - 3, moveListWidth, boardHeight);

  writePgn(moveListEntries);

  std::vector<std::string> outputLines;
  outputLines.reserve(windowHeight);

  for (const auto &s : makeTitleLines(windowWidth))
  {
    outputLines.push_back(s);
  }

  outputLines.push_back(makeBlackInfoString(boardWidth));

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

  outputLines.push_back(makeWhiteInfoString(boardWidth));

  outputLines.push_back(makeMessage(windowWidth));

  if (game.modalState == Game::ModalState::HELP)
  {
    const auto helpScreenContent = makeHelpScreenLines();
    addInformationModal(helpScreenContent, outputLines, boardHeight, windowHeight, windowWidth);
  }

  return outputLines;
}

winsize FrameBuilder::getWindowDimensions()
{
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w;
}

std::vector<std::string> FrameBuilder::makeGameBoardLines(const int squareWidth, const int squareHeight)
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
        c = chessPieceToChar(game.state.piecePlacement[pieceIndex]) ?: ' ';
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

std::vector<std::string> FrameBuilder::makeMoveListEntries()
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

std::string FrameBuilder::handleAmbiguousMove(BoardIndex fromIndex, std::vector<BoardIndex> otherIndexes)
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

std::vector<std::string> FrameBuilder::makeMoveListLines(
    const std::vector<std::string> &moveListEntries,
    const size_t moveListLength,
    const int moveListWidth,
    const int moveListHeight)
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

std::string FrameBuilder::makeInfoString(const std::string username, const TimeControl timeControl, const int boardWidth)
{
  std::stringstream ss;
  std::string timeString = timeControl.isEnabled ? timeControl.getFormattedTimeString() : "";
  const int numChars = username.length() + timeString.length();
  const int gapSize = boardWidth - numChars - BORDER_WIDTH - 1;
  ss << std::string(BORDER_WIDTH, ' ') << username << std::string(gapSize, ' ') << timeString << " \n";
  return ss.str();
};

std::string FrameBuilder::makeBlackInfoString(const int boardWidth)
{
  return makeInfoString(config.blackUsername, game.blackTime, boardWidth);
};

std::string FrameBuilder::makeWhiteInfoString(const int boardWidth)
{
  return makeInfoString(config.whiteUsername, game.whiteTime, boardWidth);
}

std::string FrameBuilder::makeMessage(const int windowWidth)
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

std::vector<std::string>
FrameBuilder::makeInformationModalLines(const int height, const int width, const std::vector<std::string> &lines)
{
  const int horizPadSize = 4;
  const int vertPad = 2;

  const std::string horizPad = std::string(horizPadSize, ' ');

  size_t linesIndex = 0;

  std::vector<std::string> res;
  const auto emptyLine = std::string(width, ' ');
  for (int i = 0; i < height; ++i)
  {
    std::string line;

    // padding
    if (i < vertPad || i > height - vertPad - 1)
    {
      line = std::string(width, ' ');
    }
    // border
    else if (i == vertPad || i == height - vertPad - 1)
    {
      line = horizPad + "+" + std::string(width - 2 * horizPadSize - 2, '-') + "+" + horizPad;
    }
    // content
    else if (linesIndex < lines.size())
    {
      const auto lineLength = lines[linesIndex].size();
      const int spaceSize = (width / 2) - (lineLength / 2) - (horizPadSize + 1);
      const std::string space = std::string(spaceSize, ' ');
      line = horizPad + "|" + space + lines[linesIndex++] + space + (lineLength % 2 ? "" : " ") + "|" + horizPad;
    }
    else
    {
      line = horizPad + "|" + std::string(width - 2 * horizPadSize - 2, ' ') + "|" + horizPad;
    }

    res.push_back(line);
  }

  return res;
}

void FrameBuilder::addInformationModal(
    const std::vector<std::string> &content,
    std::vector<std::string> &outputLines,
    const int boardHeight,
    const unsigned short windowHeight,
    const unsigned short windowWidth)
{
  const int modalWidth = windowWidth / 2;
  const int modalHeight = windowHeight / 2;

  const size_t modalHeightStart = boardHeight / 2 - modalHeight / 2;
  const size_t modalHeightEnd = boardHeight / 2 + modalHeight / 2;
  const size_t modalWidthStart = windowWidth / 2 - modalWidth / 2;
  const size_t modalWidthEnd = windowWidth / 2 + modalWidth / 2;

  const auto modalLines = makeInformationModalLines(modalHeight, modalWidth, content);

  size_t i = 0;
  size_t modalLinesIndex = 0;
  for (auto &line : outputLines)
  {
    if (i < modalHeightStart || i > modalHeightEnd || modalLinesIndex == modalLines.size())
    {
      ++i;
      continue;
    }

    const std::string prefix = modalWidthStart < line.length() ? line.substr(0, modalWidthStart - 1) : "";
    const std::string suffix = modalWidthEnd < line.length() ? line.substr(modalWidthEnd, line.length()) : "";
    line = prefix + modalLines[modalLinesIndex++] + suffix;

    ++i;
  }
}

std::vector<std::string> FrameBuilder::makeTitleLines(const size_t windowWidth)
{
  const std::string title = "~~~Chesspp by Kyle Schneider~~~";
  const std::string titlePad = std::string((windowWidth / 2) - (title.size() / 2), ' ');
  const std::string titleString = titlePad + title + titlePad;

  const std::string helpMessage = "press 'j' to open the help menu";
  const std::string helpPad = std::string((windowWidth / 2) - (helpMessage.size() / 2), ' ');
  const std::string helpString = helpPad + helpMessage + helpPad;

  return {titleString, helpString};
}

std::vector<std::string> FrameBuilder::makePawnPromotionLines()
{
  std::vector<std::string> lines{"Choose promotion piece:", "Queen", "Rook", "Bishop", "Knight"};
  std::vector<std::string> res;

  std::stringstream ss;
  for (const auto &s : lines)
  {
    res.push_back(s);
    res.push_back("");
  }

  return res;
}

std::vector<std::string> FrameBuilder::makeHelpScreenLines()
{
  const std::string resigningPlayer = game.state.activeColor == PieceColor::White ? "White" : "Black";
  const std::string resignationSuffix = " (as " + resigningPlayer + ")";

  std::vector<std::string> lines{
      "Enter moves as a from-square and to-square (example: e4 or a7)",
      "Press ENTER to submit to/from square",
      "Press 'x' to exit the menu/program",
      "Press 'd' to declare a draw",
      "Press 'r' to resign" + resignationSuffix};
  std::vector<std::string> res;

  std::stringstream ss;
  for (const auto &s : lines)
  {
    res.push_back(s);
    res.push_back("");
  }

  return res;
}