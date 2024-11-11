#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <exception>
#include <fstream>
#include <iostream>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "config.hpp"
#include "constants.hpp"
#include "game.hpp"
#include "logger.hpp"
#include "piece.hpp"
#include "positionHash.hpp"
#include "types.hpp"
#include "utils.hpp"

Game::State Game::State::fromFEN(const std::string &fen)
{
  State res{};
  size_t pos = 0;
  std::string token;
  std::string fen_copy = fen;

  for (int tokenCount = 1; tokenCount != 7; ++tokenCount)
  {
    pos = fen_copy.find(' ');
    token = fen_copy.substr(0, pos);
    fen_copy.erase(0, pos + 1);

    switch (tokenCount)
    {
    case 1:
      res.piecePlacement = piecePlacementStringToArray(token);
      break;
    case 2:
      res.activeColor = charToColor(token[0]);
      break;
    case 3:
      res.castlingAvailability = parseCastlingAvailability(token);
      break;
    case 4:
      res.enPassantIndex = token == "-" ? std::nullopt : std::optional{algebraicToIndex(token)};
      break;
    case 5:
      res.halfmoveClock = token == "-" ? 0 : std::stoi(token);
      break;
    case 6:
      res.fullmoveClock = std::stoi(token);
      break;
    default:
      break;
    }
  }

  return res;
};

// getters/setters
std::string Game::getFenStr() const
{
  std::ostringstream fen;
  fen << piecePlacementArrayToString(piecePlacement) << " " << colorToChar(activeColor) << " "
      << castlingAvailabilityToString(castlingAvailability) << " " << indexToAlgebraic(enPassantIndex) << " "
      << std::to_string(halfmoveClock) << " " << std::to_string(fullmoveClock);

  return fen.str();
}

// constructors
Game::Game() : Game(State::newGameState())
{
  positionCount[{piecePlacement, castlingAvailability, enPassantIndex}] = 1;
}

Game::Game(const State &initialGameState)
    : piecePlacement(initialGameState.piecePlacement), activeColor(initialGameState.activeColor),
      castlingAvailability(initialGameState.castlingAvailability), enPassantIndex(initialGameState.enPassantIndex),
      halfmoveClock(initialGameState.halfmoveClock), fullmoveClock(initialGameState.fullmoveClock), pawn(*this),
      knight(*this), bishop(*this), rook(*this), queen(*this), king(*this)
{
  positionCount[{piecePlacement, castlingAvailability, enPassantIndex}] = 1;
}

Game::Game(const std::string &fen) : Game(State::fromFEN(fen)) {}

// public methods

bool Game::processNextMove()
{
  const auto [fromIndex, toIndex] = getNextMove();

  if (validateMove(fromIndex, toIndex))
  {
    const auto fromPiece = piecePlacement[fromIndex];
    const auto fromColor = pieceColor(fromPiece);
    const auto toPiece = piecePlacement[toIndex];
    const auto samePieceIndexes = getSamePieceIndexes(fromIndex, toIndex);

    const auto castlingString = handleCastling(fromIndex, toIndex);
    const auto isEnPassantCapture = handleEnPassant(fromIndex, toIndex);
    const auto promotionPiece = handlePawnPromotion(fromPiece, toIndex);

    updateHalfMoveClock(fromPiece, toPiece);

    const auto newToPiece = promotionPiece != ChessPiece::Empty ? promotionPiece : fromPiece;
    piecePlacement[toIndex] = newToPiece;
    piecePlacement[fromIndex] = ChessPiece::Empty;
    activeColor = !activeColor;

    positionCount[{piecePlacement, castlingAvailability, enPassantIndex}] += 1;

    if (handleGameOver())
    {
      logger.log("GAME OVER");
    }

    const auto isOpponentInCheck = isKingInCheck(!fromColor, piecePlacement);
    const MoveListItem moveListItem = {fromIndex,      fromPiece,         toIndex,
                                       toPiece,        samePieceIndexes,  promotionPiece,
                                       castlingString, isOpponentInCheck, isEnPassantCapture};
    moveList.push_back(moveListItem);

    return true;
  };

  message = "Illegal move.";
  return false;
}

std::vector<BoardIndex> Game::getPieceLegalMoves(const BoardIndex index) const
{
  const auto piece = piecePlacement[index];
  if (piece == ChessPiece::Empty)
  {
    throw std::invalid_argument("getPieceLegalMove(): no piece at given index");
  }

  std::vector<BoardIndex> indexes = {};
  switch (piece)
  {
  case ChessPiece::BlackPawn:
  case ChessPiece::WhitePawn:
    indexes = pawn.legalSquareIndexes(index);
    break;
  case ChessPiece::BlackKnight:
  case ChessPiece::WhiteKnight:
    indexes = knight.legalSquareIndexes(index);
    break;
  case ChessPiece::BlackBishop:
  case ChessPiece::WhiteBishop:
    indexes = bishop.legalSquareIndexes(index);
    break;
  case ChessPiece::BlackRook:
  case ChessPiece::WhiteRook:
    indexes = rook.legalSquareIndexes(index);
    break;
  case ChessPiece::BlackQueen:
  case ChessPiece::WhiteQueen:
    indexes = queen.legalSquareIndexes(index);
    break;
  case ChessPiece::BlackKing:
  case ChessPiece::WhiteKing:
    indexes = king.legalSquareIndexes(index);
    break;
  default:
    break;
  }

  return indexes;
}

bool Game::validateMove(const BoardIndex fromIndex, const BoardIndex toIndex) const
{
  const auto fromPiece = piecePlacement[fromIndex];
  if (fromPiece == ChessPiece::Empty)
  {
    return false;
  }

  const auto fromColor = pieceColor(fromPiece);
  if (!config.disableTurnOrder && fromColor != activeColor)
  {
    return false;
  }

  const auto indexes = getPieceLegalMoves(fromIndex);
  if (std::find(indexes.cbegin(), indexes.cend(), toIndex) == indexes.cend())
  {
    return false;
  }

  return true;
}

// private methods

std::pair<BoardIndex, BoardIndex> Game::getNextMove()
{
  BoardIndex fromIndex, toIndex;

  if (config.blackIsCpu && activeColor == PieceColor::Black)
  {
    std::tie(fromIndex, toIndex) = generateCpuMove(PieceColor::Black);
  }
  else if (config.whiteIsCpu && activeColor == PieceColor::White)
  {
    std::tie(fromIndex, toIndex) = generateCpuMove(PieceColor::White);
  }
  else
  {
    std::tie(fromIndex, toIndex) = getUserMove(std::cin, std::cout);
  }

  return {fromIndex, toIndex};
};

std::pair<BoardIndex, BoardIndex> Game::getUserMove(std::istream &is, std::ostream &os)
{
  BoardIndex fromIndex, toIndex;

  std::string fromAlgebraic, toAlgebraic;

  os << "from square: ";
  is >> fromAlgebraic;
  os << "\n";
  fromIndex = algebraicToIndex(fromAlgebraic);

  os << "to square: ";
  is >> toAlgebraic;
  os << "\n";
  toIndex = algebraicToIndex(toAlgebraic);

  return {fromIndex, toIndex};
}

std::pair<BoardIndex, BoardIndex> Game::generateCpuMove(const PieceColor cpuColor)
{
  std::vector<int> cpuPiecesIdxs;
  cpuPiecesIdxs.reserve(32);

  for (size_t i = 0; i < piecePlacement.size(); ++i)
  {
    auto piece = piecePlacement[i];
    if (piece != ChessPiece::Empty && pieceColor(piecePlacement[i]) == cpuColor)
    {
      cpuPiecesIdxs.push_back(i);
    }
  }

  std::random_device rd;
  std::mt19937 g(rd());

  std::shuffle(cpuPiecesIdxs.begin(), cpuPiecesIdxs.end(), g);

  BoardIndex resFromIndex, resToIndex;

  for (auto fromIndex : cpuPiecesIdxs)
  {
    auto indexes = getPieceLegalMoves(fromIndex);
    if (!indexes.size())
    {
      continue;
    }

    std::shuffle(indexes.begin(), indexes.end(), g);

    for (auto toIndex : indexes)
    {
      if (validateMove(fromIndex, toIndex))
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(config.cpuMoveDelayMs));
        return {fromIndex, toIndex};
      }
    }
  }

  throw std::runtime_error("generateCpuMove(): control reached end of function unexpectedly");
};

void Game::updateHalfMoveClock(const ChessPiece fromPiece, const ChessPiece toPiece)
{
  const auto isCapture = toPiece != ChessPiece::Empty;
  const auto isPawnMove = fromPiece == ChessPiece::BlackPawn || fromPiece == ChessPiece::WhitePawn;
  if (isCapture || isPawnMove)
  {
    halfmoveClock = 0;
  }
  else
  {
    halfmoveClock += 1;
  }
}

bool Game::handleEnPassant(const BoardIndex fromIndex, const BoardIndex toIndex)
{
  const auto fromPiece = piecePlacement[fromIndex];
  if (fromPiece == ChessPiece::Empty)
  {
    throw std::invalid_argument("handleEnPassant(): no piece at given index");
  }

  const auto fromColor = pieceColor(fromPiece);

  // capture
  if (toIndex == enPassantIndex)
  {
    piecePlacement[toIndex + (fromColor == PieceColor::White ? +8 : -8)] = ChessPiece::Empty;
    return true;
  }

  // create en passant opportunity
  const bool isPawn = (fromPiece == ChessPiece::BlackPawn || fromPiece == ChessPiece::WhitePawn);
  if (isPawn && abs(fromIndex - toIndex) == 16)
  {
    enPassantIndex = fromIndex + (fromColor == PieceColor::White ? -8 : +8);
  }
  else
  {
    enPassantIndex.reset();
  }

  return false;
}

ChessPiece Game::handlePawnPromotion(const ChessPiece fromPiece, const BoardIndex toIndex)
{
  const auto [file, rank] = indexToFileRank(toIndex);
  if (fromPiece == ChessPiece::WhitePawn && rank == 8)
  {
    const ChessPiece promotedPiece = ChessPiece::WhiteQueen;
    piecePlacement[toIndex] = promotedPiece;
    return promotedPiece;
  }
  if (fromPiece == ChessPiece::BlackPawn && rank == 1)
  {
    const ChessPiece promotedPiece = ChessPiece::BlackQueen;
    piecePlacement[toIndex] = promotedPiece;
    return promotedPiece;
  }

  return ChessPiece::Empty;
}

std::string Game::handleCastling(const BoardIndex fromIndex, const BoardIndex toIndex)
{
  const auto piece = piecePlacement[fromIndex];
  if (piece == ChessPiece::Empty)
  {
    throw std::invalid_argument("handleCastling(): no piece at given index");
  }

  std::string res;

  if (piece == ChessPiece::WhiteKing)
  {
    castlingAvailability.whiteShort = false;
    castlingAvailability.whiteLong = false;

    if (fromIndex == 60 && toIndex == 62)
    {
      piecePlacement[63] = ChessPiece::Empty;
      piecePlacement[61] = ChessPiece::WhiteRook;
      res = shortCastleString;
    }

    if (fromIndex == 60 && toIndex == 58)
    {
      piecePlacement[56] = ChessPiece::Empty;
      piecePlacement[59] = ChessPiece::WhiteRook;
      res = longCastleString;
    }
  }

  if (piece == ChessPiece::BlackKing)
  {
    castlingAvailability.blackShort = false;
    castlingAvailability.blackLong = false;

    if (fromIndex == 4 && toIndex == 6)
    {
      piecePlacement[7] = ChessPiece::Empty;
      piecePlacement[5] = ChessPiece::BlackRook;
      res = shortCastleString;
    }

    if (fromIndex == 4 && toIndex == 2)
    {
      piecePlacement[0] = ChessPiece::Empty;
      piecePlacement[3] = ChessPiece::BlackRook;
      res = longCastleString;
    }
  }

  if (fromIndex == 63 && piece == ChessPiece::WhiteRook)
  {
    castlingAvailability.whiteShort = false;
  }

  if (fromIndex == 56 && piece == ChessPiece::WhiteRook)
  {
    castlingAvailability.whiteLong = false;
  }

  if (fromIndex == 7 && piece == ChessPiece::BlackRook)
  {
    castlingAvailability.blackShort = false;
  }

  if (fromIndex == 0 && piece == ChessPiece::BlackRook)
  {
    castlingAvailability.blackLong = false;
  }

  return res;
};

bool Game::handleGameOver()
{
  // checkmate
  bool isCheckmate = false;
  if (isKingInCheck(activeColor, piecePlacement))
  {
    isCheckmate = true;
    for (size_t i = 0; i < piecePlacement.size(); ++i)
    {
      if (!isCheckmate)
      {
        break;
      }

      const BoardIndex boardIndex = i;
      const auto piece = piecePlacement[boardIndex];
      if (piece != ChessPiece::Empty && pieceColor(piece) == activeColor)
      {
        const auto moves = getPieceLegalMoves(boardIndex);
        for (auto index : moves)
        {
          // might need all the other handlers here
          auto tempPiecePlacement = piecePlacement;
          tempPiecePlacement[index] = piece;
          tempPiecePlacement[boardIndex] = ChessPiece::Empty;
          if (!isKingInCheck(activeColor, tempPiecePlacement))
          {
            isCheckmate = false;
            break;
          }
        }
      }
    }
  }
  if (isCheckmate)
  {
    std::string newMessage = !(activeColor == PieceColor::White) ? "white" : "black";
    newMessage += " won by checkmate";
    message = newMessage;
    isGameOver = true;
    return true;
  }

  // stalemate
  bool isStalemate = true;
  for (size_t i = 0; i < piecePlacement.size(); ++i)
  {
    const BoardIndex boardIndex = i;
    const auto piece = piecePlacement[i];
    if (piece != ChessPiece::Empty && pieceColor(piece) == activeColor)
    {
      const auto moves = getPieceLegalMoves(boardIndex);
      if (moves.size())
      {
        isStalemate = false;
        break;
      }
    }
  }
  if (isStalemate)
  {
    message = "stalemate";
    isGameOver = true;
    return true;
  }

  // 50 move-rule
  if (halfmoveClock == 100)
  {
    message = "draw by 50 move-rule";
    isGameOver = true;
    return true;
  }

  // repetition
  for (auto &p : positionCount)
  {
    if (p.second == 3)
    {
      message = "draw by repetition";
      isGameOver = true;
      return true;
    }
  }

  return false;
}

std::vector<BoardIndex> Game::getSamePieceIndexes(const BoardIndex fromIndex, const BoardIndex toIndex) const
{
  std::vector<BoardIndex> res;
  const auto fromPiece = piecePlacement[fromIndex];

  for (size_t i = 0; i < piecePlacement.size(); ++i)
  {
    const BoardIndex boardIndex = i;
    if (boardIndex != fromIndex && piecePlacement[boardIndex] == fromPiece)
    {
      const auto indexes = getPieceLegalMoves(boardIndex);
      const auto samePieceIt = std::find(indexes.cbegin(), indexes.cend(), toIndex);
      if (samePieceIt != indexes.cend())
      {
        res.emplace_back(boardIndex);
      }
    }
  }

  return res;
}

bool Game::isKingInCheck(const PieceColor color, const PiecePlacement &piecePlacement)
{
  const auto kingPiece = color == PieceColor::White ? ChessPiece::WhiteKing : ChessPiece::BlackKing;
  const auto kingIndex = std::find(piecePlacement.cbegin(), piecePlacement.cend(), kingPiece) - piecePlacement.cbegin();

  return isSquareUnderAttack(kingIndex, color, piecePlacement);
}

bool Game::isSquareUnderAttack(const BoardIndex index, const PieceColor defenderColor,
                               const PiecePlacement &piecePlacement)
{
  const auto isPieceInIndexesLambda = [&](const ChessPiece searchPiece, const std::vector<BoardIndex> &indexes)
  {
    for (auto &idx : indexes)
    {
      const auto currentPiece = piecePlacement[idx];
      if (currentPiece == ChessPiece::Empty || pieceColor(currentPiece) == defenderColor)
      {
        continue;
      }

      const char idxPieceChar = chessPieceToChar(currentPiece);
      const char pieceChar = chessPieceToChar(searchPiece);

      if (std::tolower(idxPieceChar) == std::tolower(pieceChar))
      {
        return true;
      }

      if (searchPiece == ChessPiece::BlackBishop || searchPiece == ChessPiece::WhiteBishop ||
          searchPiece == ChessPiece::BlackRook || searchPiece == ChessPiece::WhiteRook)
      {
        if (currentPiece == ChessPiece::BlackQueen || currentPiece == ChessPiece::WhiteQueen)
        {
          return true;
        }
      }
    }
    return false;
  };

  const bool isWhite = defenderColor == PieceColor::White;

  // pawn
  const auto pawn = isWhite ? ChessPiece::BlackPawn : ChessPiece::WhitePawn;
  const std::vector<std::pair<int, int>> pawnOffsets =
      (defenderColor == PieceColor::White)
          ? std::vector<std::pair<int, int>>{
                {1, 1},
                {-1, 1},
            }
          : std::vector<std::pair<int, int>>{
                {1, -1},
                {-1, -1},
            };
  const std::vector<BoardIndex> pawnIndexes = Piece::squareIndexes(index, defenderColor, pawnOffsets, piecePlacement);
  if (isPieceInIndexesLambda(pawn, pawnIndexes))
  {
    return true;
  }

  // knight
  const auto knight = isWhite ? ChessPiece::BlackKnight : ChessPiece::WhiteKnight;
  const std::vector<std::pair<int, int>> knightOffsets = {
      {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
  };
  const auto knightIndexes = Piece::squareIndexes(index, defenderColor, knightOffsets, piecePlacement);
  if (isPieceInIndexesLambda(knight, knightIndexes))
  {
    return true;
  }

  // bishop/queen
  const auto bishop = isWhite ? ChessPiece::BlackBishop : ChessPiece::WhiteBishop;
  const std::vector<std::pair<int, int>> diagOffsets = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
  const auto diagIndexes = Piece::linearSquareIndexes(index, defenderColor, diagOffsets, piecePlacement);
  if (isPieceInIndexesLambda(bishop, diagIndexes))
  {
    return true;
  }

  // rook/queen
  const auto rook = isWhite ? ChessPiece::BlackRook : ChessPiece::WhiteRook;
  const std::vector<std::pair<int, int>> horizVertOffsets = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
  const auto horizVertIndexes = Piece::linearSquareIndexes(index, defenderColor, horizVertOffsets, piecePlacement);
  if (isPieceInIndexesLambda(rook, horizVertIndexes))
  {
    return true;
  }

  // king
  const auto king = isWhite ? ChessPiece::BlackKing : ChessPiece::WhiteKing;
  const std::vector<std::pair<int, int>> kingOffsets = {
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1},
  };
  const auto kingIndexes = Piece::squareIndexes(index, defenderColor, kingOffsets, piecePlacement);
  if (isPieceInIndexesLambda(king, kingIndexes))
  {
    return true;
  }

  return false;
};
