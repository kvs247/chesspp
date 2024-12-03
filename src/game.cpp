#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <fstream>
#include <iterator>
#include <optional>
#include <random>
#include <set>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "config.hpp"
#include "constants.hpp"
#include "game.hpp"
#include "logger.hpp"
#include "moveInput.hpp"
#include "piece.hpp"
#include "positionHash.hpp"
#include "timeControl.hpp"
#include "types.hpp"
#include "utils.hpp"

Game::GameState Game::GameState::fromFEN(const std::string &fen)
{
  GameState res{};
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
  fen << piecePlacementArrayToString(state.piecePlacement) << " " << colorToChar(state.activeColor) << " "
      << castlingAvailabilityToString(state.castlingAvailability) << " " << indexToAlgebraic(state.enPassantIndex)
      << " " << std::to_string(state.halfmoveClock) << " " << std::to_string(state.fullmoveClock);

  return fen.str();
}

// constructors
Game::Game() : Game(GameState::newGameState()) {}

Game::Game(const std::string &fen) : Game(GameState::fromFEN(fen)) {}

Game::Game(const GameState &gs)
    : renderer(*this), state(
                           {gs.piecePlacement,
                            gs.activeColor,
                            gs.castlingAvailability,
                            gs.enPassantIndex,
                            gs.halfmoveClock,
                            gs.fullmoveClock}),
      modalState(ModalState::NONE), pawn(*this), knight(*this), bishop(*this), rook(*this), queen(*this), king(*this),
      randomGenerator(std::random_device{}())
{
  incrementPositionCount();
  timer.start();
  timer.startPlayerTimer(whiteTime);
}

// public methods

bool Game::isWhiteMove() const { return state.activeColor == PieceColor::White; }

bool Game::processNextMove()
{
  BoardIndex fromIndex, toIndex;
  const auto inputResult = moveInput.handleGetInput();
  if (inputResult.has_value())
  {
    std::tie(fromIndex, toIndex) = inputResult.value();
  }
  else
  {
    handleGameOver();
    return false;
  }

  if (validateMove(fromIndex, toIndex))
  {
    const auto fromPiece = state.piecePlacement[fromIndex];
    const auto fromColor = getPieceColor(fromPiece);
    const auto toPiece = state.piecePlacement[toIndex];
    const auto samePieceIndexes = getSamePieceIndexes(fromIndex, toIndex);

    const auto castlingString = handleCastling(fromIndex, toIndex);
    const auto isEnPassantCapture = handleEnPassant(fromIndex, toIndex);
    const auto promotionPiece = handlePawnPromotion(fromPiece, toIndex);

    updateHalfMoveClock(fromPiece, toPiece);

    const auto newToPiece = promotionPiece != ChessPiece::Empty ? promotionPiece : fromPiece;
    state.piecePlacement[toIndex] = newToPiece;
    state.piecePlacement[fromIndex] = ChessPiece::Empty;
    state.activeColor = !state.activeColor;

    incrementPositionCount();

    if (handleGameOver())
    {
      logger.log("GAME OVER");
    }

    const auto isOpponentInCheck = isKingInCheck(!fromColor, state.piecePlacement);
    const MoveListItem moveListItem = {
        fromIndex,
        fromPiece,
        toIndex,
        toPiece,
        samePieceIndexes,
        promotionPiece,
        castlingString,
        isOpponentInCheck,
        isEnPassantCapture};
    moveList.push_back(moveListItem);

    if (isWhiteMove())
    {
      timer.stopPlayerTimer(blackTime);
      timer.startPlayerTimer(whiteTime);
    }
    else
    {
      timer.stopPlayerTimer(whiteTime);
      timer.startPlayerTimer(blackTime);
    }

    return true;
  };

  message = "Illegal move.";
  return false;
}

std::vector<BoardIndex> Game::getPieceLegalMoves(const BoardIndex index) const
{
  const auto piece = state.piecePlacement[index];
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
  const auto fromPiece = state.piecePlacement[fromIndex];
  if (fromPiece == ChessPiece::Empty)
  {
    return false;
  }

  const auto fromColor = getPieceColor(fromPiece);
  if (!config.disableTurnOrder && fromColor != state.activeColor)
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

std::pair<BoardIndex, BoardIndex> Game::generateCpuMove(const PieceColor cpuColor)
{
  std::vector<int> cpuPiecesIdxs;
  cpuPiecesIdxs.reserve(32);

  for (size_t i = 0; i < state.piecePlacement.size(); ++i)
  {
    auto piece = state.piecePlacement[i];
    if (piece != ChessPiece::Empty && getPieceColor(state.piecePlacement[i]) == cpuColor)
    {
      cpuPiecesIdxs.push_back(i);
    }
  }

  std::shuffle(cpuPiecesIdxs.begin(), cpuPiecesIdxs.end(), randomGenerator);

  BoardIndex resFromIndex, resToIndex;

  for (auto fromIndex : cpuPiecesIdxs)
  {
    auto indexes = getPieceLegalMoves(fromIndex);
    if (!indexes.size())
    {
      continue;
    }

    std::shuffle(indexes.begin(), indexes.end(), randomGenerator);

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
    state.halfmoveClock = 0;
  }
  else
  {
    state.halfmoveClock += 1;
  }
}

bool Game::handleEnPassant(const BoardIndex fromIndex, const BoardIndex toIndex)
{
  const auto fromPiece = state.piecePlacement[fromIndex];
  if (fromPiece == ChessPiece::Empty)
  {
    throw std::invalid_argument("handleEnPassant(): no piece at given index");
  }

  const auto fromColor = getPieceColor(fromPiece);

  // capture
  if (toIndex == state.enPassantIndex)
  {
    state.piecePlacement[toIndex + (fromColor == PieceColor::White ? +8 : -8)] = ChessPiece::Empty;
    return true;
  }

  // create en passant opportunity
  const bool isPawn = (fromPiece == ChessPiece::BlackPawn || fromPiece == ChessPiece::WhitePawn);
  if (isPawn && abs(fromIndex - toIndex) == 16)
  {
    state.enPassantIndex = fromIndex + (fromColor == PieceColor::White ? -8 : +8);
  }
  else
  {
    state.enPassantIndex.reset();
  }

  return false;
}

ChessPiece Game::handlePawnPromotion(const ChessPiece fromPiece, const BoardIndex toIndex)
{
  static const std::set<char> validChars{'q', 'r', 'b', 'n'};

  const auto promotionPieceCharToChessPiece = [](char c, PieceColor playerColor)
  {
    if (playerColor == PieceColor::White)
    {
      c = std::toupper(c);
    }
    return charToChessPiece(c);
  };

  const auto collectPromotionPieceChar = [&]()
  {
    char promotionPieceChar;
    while (validChars.find(promotionPieceChar) == validChars.cend())
    {
      const auto input = moveInput.collectUserInput("Enter promotion piece: ", 1);
      promotionPieceChar = input.value().front();
    }
    return promotionPieceChar;
  };

  const auto getRandomPromotionPieceChar = [&]()
  {
    static const auto validCharsVec = std::vector(validChars.cbegin(), validChars.cend());
    return validCharsVec[std::uniform_int_distribution<size_t>(0, validCharsVec.size() - 1)(randomGenerator)];
  };

  const auto [file, rank] = indexToFileRank(toIndex);
  if (fromPiece == ChessPiece::WhitePawn && rank == 8)
  {
    const auto promotionPieceChar = config.whiteIsCpu ? getRandomPromotionPieceChar() : collectPromotionPieceChar();
    const auto promotionPiece = promotionPieceCharToChessPiece(promotionPieceChar, PieceColor::White);
    state.piecePlacement[toIndex] = promotionPiece;
    return promotionPiece;
  }
  if (fromPiece == ChessPiece::BlackPawn && rank == 1)
  {
    const auto promotionPieceChar = config.blackIsCpu ? getRandomPromotionPieceChar() : collectPromotionPieceChar();
    const auto promotionPiece = promotionPieceCharToChessPiece(promotionPieceChar, PieceColor::Black);
    state.piecePlacement[toIndex] = promotionPiece;
    return promotionPiece;
  }

  return ChessPiece::Empty;
}

std::string Game::handleCastling(const BoardIndex fromIndex, const BoardIndex toIndex)
{
  const auto piece = state.piecePlacement[fromIndex];
  if (piece == ChessPiece::Empty)
  {
    throw std::invalid_argument("handleCastling(): no piece at given index");
  }

  std::string res;

  if (piece == ChessPiece::WhiteKing)
  {
    state.castlingAvailability.whiteShort = false;
    state.castlingAvailability.whiteLong = false;

    if (fromIndex == 60 && toIndex == 62)
    {
      state.piecePlacement[63] = ChessPiece::Empty;
      state.piecePlacement[61] = ChessPiece::WhiteRook;
      res = shortCastleString;
    }

    if (fromIndex == 60 && toIndex == 58)
    {
      state.piecePlacement[56] = ChessPiece::Empty;
      state.piecePlacement[59] = ChessPiece::WhiteRook;
      res = longCastleString;
    }
  }

  if (piece == ChessPiece::BlackKing)
  {
    state.castlingAvailability.blackShort = false;
    state.castlingAvailability.blackLong = false;

    if (fromIndex == 4 && toIndex == 6)
    {
      state.piecePlacement[7] = ChessPiece::Empty;
      state.piecePlacement[5] = ChessPiece::BlackRook;
      res = shortCastleString;
    }

    if (fromIndex == 4 && toIndex == 2)
    {
      state.piecePlacement[0] = ChessPiece::Empty;
      state.piecePlacement[3] = ChessPiece::BlackRook;
      res = longCastleString;
    }
  }

  if (fromIndex == 63 && piece == ChessPiece::WhiteRook)
  {
    state.castlingAvailability.whiteShort = false;
  }

  if (fromIndex == 56 && piece == ChessPiece::WhiteRook)
  {
    state.castlingAvailability.whiteLong = false;
  }

  if (fromIndex == 7 && piece == ChessPiece::BlackRook)
  {
    state.castlingAvailability.blackShort = false;
  }

  if (fromIndex == 0 && piece == ChessPiece::BlackRook)
  {
    state.castlingAvailability.blackLong = false;
  }

  return res;
};

bool Game::handleGameOver()
{
  // checkmate
  bool isCheckmate = false;
  if (isKingInCheck(state.activeColor, state.piecePlacement))
  {
    isCheckmate = true;
    for (size_t i = 0; i < state.piecePlacement.size(); ++i)
    {
      if (!isCheckmate)
      {
        break;
      }

      const BoardIndex boardIndex = i;
      const auto piece = state.piecePlacement[boardIndex];
      if (piece != ChessPiece::Empty && getPieceColor(piece) == state.activeColor)
      {
        const auto moves = getPieceLegalMoves(boardIndex);
        for (auto index : moves)
        {
          // might need all the other handlers here
          auto tempPiecePlacement = state.piecePlacement;
          tempPiecePlacement[index] = piece;
          tempPiecePlacement[boardIndex] = ChessPiece::Empty;
          if (!isKingInCheck(state.activeColor, tempPiecePlacement))
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
    std::string newMessage = !(state.activeColor == PieceColor::White) ? "white" : "black";
    newMessage += " won by checkmate";
    message = newMessage;
    isGameOver = true;
    return true;
  }

  // setup for insufficient material
  using PieceVector = std::vector<ChessPiece>;
  PieceVector whitePieces;
  std::copy_if(
      state.piecePlacement.cbegin(),
      state.piecePlacement.cend(),
      std::back_inserter(whitePieces),
      [](ChessPiece piece) { return piece != ChessPiece::Empty && getPieceColor(piece) == PieceColor::White; });

  PieceVector blackPieces;
  std::copy_if(
      state.piecePlacement.cbegin(),
      state.piecePlacement.cend(),
      std::back_inserter(blackPieces),
      [](ChessPiece piece) { return piece != ChessPiece::Empty && getPieceColor(piece) == PieceColor::Black; });

  // timeout
  const auto canCheckmate = [&](PieceVector pieces)
  {
    if (pieces.size() > 3)
    {
      return true;
    }

    std::set<char> piecesSet;
    for (const auto &p : pieces)
    {
      piecesSet.insert(std::tolower(chessPieceToChar(p)));
    }

    if ((piecesSet == std::set<char>{'k'}) || (piecesSet == std::set<char>{'k', 'n'}) ||
        (piecesSet == std::set<char>{'k', 'b'}) || (piecesSet == std::set<char>{'k', 'n', 'n'}))
    {
      return false;
    }

    return true;
  };

  if (whiteTime.isEnabled && whiteTime.isOutOfTime())
  {
    if (canCheckmate(blackPieces))
    {
      message = "black won on time";
    }
    else
    {
      message = "draw by timeout vs insufficient material";
    }
    isGameOver = true;
    return true;
  }
  if (blackTime.isEnabled && blackTime.isOutOfTime())
  {
    if (canCheckmate(whitePieces))
    {
      message = "white won on time";
    }
    else
    {
      message = "draw by timeout vs insufficient material";
    }
    isGameOver = true;
    return true;
  }

  // stalemate
  bool isStalemate = true;
  for (size_t i = 0; i < state.piecePlacement.size(); ++i)
  {
    const BoardIndex boardIndex = i;
    const auto piece = state.piecePlacement[i];
    if (piece != ChessPiece::Empty && getPieceColor(piece) == state.activeColor)
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
  if (state.halfmoveClock == 100)
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

  // insufficient material
  auto isKingVersusKing = [&]() -> bool { return (whitePieces.size() == 1) && (blackPieces.size() == 1); };

  auto isKingMinorPieceVersusKing = [&]() -> bool
  {
    auto test = [&](PieceVector v1, PieceVector v2) -> bool
    {
      if (v1.size() == 1 && v2.size() == 2)
      {
        char pieceA = std::tolower(chessPieceToChar(v2[0]));
        char pieceB = std::tolower(chessPieceToChar(v2[1]));
        if (pieceA == 'n' || pieceA == 'b' || pieceB == 'n' || pieceB == 'b')
        {
          return true;
        }
      }
      return false;
    };

    return test(whitePieces, blackPieces) || test(blackPieces, whitePieces);
  };

  auto isKingTwoKnightsVersusKing = [&]() -> bool
  {
    auto test = [&](PieceVector v1, PieceVector v2) -> bool
    {
      if (v1.size() == 1 && v2.size() == 3)
      {
        char pieceA = std::tolower(chessPieceToChar(v2[0]));
        char pieceB = std::tolower(chessPieceToChar(v2[1]));
        char pieceC = std::tolower(chessPieceToChar(v2[2]));
        if (pieceA == 'k')
        {
          return pieceB == 'n' && pieceC == 'n';
        }
        if (pieceB == 'k')
        {
          return pieceA == 'n' && pieceC == 'n';
        }
        if (pieceC == 'k')
        {
          return pieceA == 'n' && pieceB == 'n';
        }
      }
      return false;
    };

    return test(whitePieces, blackPieces) || test(blackPieces, whitePieces);
  };

  auto isKingMinorPieceVersusKingMinorPiece = [&]() -> bool
  {
    auto test = [&](PieceVector v1, PieceVector v2)
    {
      if (v1.size() == 2 && v2.size() == 2)
      {
        char piece1A = std::tolower(chessPieceToChar(v1[0]));
        char piece1B = std::tolower(chessPieceToChar(v1[1]));
        char piece2A = std::tolower(chessPieceToChar(v2[0]));
        char piece2B = std::tolower(chessPieceToChar(v2[1]));

        char nonKingPiece1 = piece1A != 'k' ? piece1A : piece1B;
        char nonKingPiece2 = piece2A != 'k' ? piece2A : piece2B;

        return (nonKingPiece1 == 'b' || nonKingPiece1 == 'n') && (nonKingPiece2 == 'b' || nonKingPiece2 == 'n');
      }
      return false;
    };

    return test(whitePieces, blackPieces) || test(blackPieces, whitePieces);
  };

  if (isKingVersusKing() || isKingMinorPieceVersusKing() || isKingTwoKnightsVersusKing() ||
      isKingMinorPieceVersusKingMinorPiece())
  {
    message = "draw by insufficient material";
    isGameOver = true;
    return true;
  }

  return false;
}

void Game::incrementPositionCount()
{
  Position pos{state.piecePlacement, state.castlingAvailability, state.enPassantIndex};
  positionCount[pos] += 1;
}

std::vector<BoardIndex> Game::getSamePieceIndexes(const BoardIndex fromIndex, const BoardIndex toIndex) const
{
  std::vector<BoardIndex> res;
  const auto fromPiece = state.piecePlacement[fromIndex];

  for (size_t i = 0; i < state.piecePlacement.size(); ++i)
  {
    const BoardIndex boardIndex = i;
    if (boardIndex != fromIndex && state.piecePlacement[boardIndex] == fromPiece)
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

bool Game::isSquareUnderAttack(
    const BoardIndex index,
    const PieceColor defenderColor,
    const PiecePlacement &piecePlacement)
{
  const auto isPieceInIndexesLambda = [&](const ChessPiece searchPiece, const std::vector<BoardIndex> &indexes)
  {
    for (auto &idx : indexes)
    {
      const auto currentPiece = piecePlacement[idx];
      if (currentPiece == ChessPiece::Empty || getPieceColor(currentPiece) == defenderColor)
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
      {1, 2},
      {1, -2},
      {-1, 2},
      {-1, -2},
      {2, 1},
      {2, -1},
      {-2, 1},
      {-2, -1},
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
      {1, 0},
      {-1, 0},
      {0, 1},
      {0, -1},
      {1, 1},
      {1, -1},
      {-1, 1},
      {-1, -1},
  };
  const auto kingIndexes = Piece::squareIndexes(index, defenderColor, kingOffsets, piecePlacement);
  if (isPieceInIndexesLambda(king, kingIndexes))
  {
    return true;
  }

  return false;
};
