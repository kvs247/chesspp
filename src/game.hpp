#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <fstream>
#include <iostream>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#include "constants.hpp"
#include "logger.hpp"
#include "piece.hpp"
#include "types.hpp"
#include "utils.hpp"

#define BLACK_IS_CPU true
#define WHITE_IS_CPU true
#define DISABLE_TURN_ORDER true
#define CPU_MOVE_DELAY_MS 250

class Game {
  friend class Piece;
  friend class Pawn;
  friend class Bishop;
  friend class Rook;
  friend class Queen;
  friend class King;

 public:
  Game(const PiecePlacement &, PieceColor, std::string,
       std::optional<BoardIndex>, int, int);
  Game();
  Game(std::string &);

  bool move();
  std::string getFenStr();
  PiecePlacement getPiecePlacement();
  void readMove(BoardIndex &, BoardIndex &) const;
  std::vector<BoardIndex> getPieceLegalMoves(const ChessPiece &,
                                             const BoardIndex) const;
  void handleEnPassant(const ChessPiece, const PieceColor, const BoardIndex,
                       const BoardIndex);  // could this be private?
  void handlePawnPromotion(const ChessPiece, const BoardIndex);
  std::pair<BoardIndex, BoardIndex> generateCpuMove(PieceColor) const;
  static bool isKingInCheck(const PieceColor, const PiecePlacement &);
  bool validateMove(BoardIndex, BoardIndex) const;

 private:
  PiecePlacement piecePlacement;
  PieceColor activeColor;
  std::string castlingAvailability;
  std::optional<BoardIndex> enPassantIndex;
  int halfmoveClock;
  int fullmoveClock;

  Pawn pawn;
  Knight knight;
  Bishop bishop;
  Rook rook;
  Queen queen;
  King king;
};

inline Game::Game(const PiecePlacement &pp, PieceColor ac, std::string ca,
                  std::optional<BoardIndex> ep, int hc, int fc)
    : piecePlacement(pp),
      activeColor(ac),
      castlingAvailability(ca),
      enPassantIndex(ep),
      halfmoveClock(hc),
      fullmoveClock(fc),
      pawn(*this),
      knight(*this),
      bishop(*this),
      rook(*this),
      queen(*this),
      king(*this) {}

inline Game::Game()
    : Game(startingPiecePlacement, PieceColor::White,
           startingCastlingAvailability, std::nullopt, 0, 0) {}

inline Game::Game(std::string &fen) : Game() {
  size_t pos = 0;
  std::string token;
  for (int tokenCount = 1; tokenCount != 7; ++tokenCount) {
    pos = fen.find(' ');
    token = fen.substr(0, pos);
    fen.erase(0, pos + 1);

    switch (tokenCount) {
      case 1:
        piecePlacement = piecePlacementStringToArray(token);
        break;
      case 2:
        activeColor = charToColor(token[0]);
        break;
      case 3:
        castlingAvailability = token;
        break;
      case 4:
        if (token == "-")
          enPassantIndex = std::nullopt;
        else
          enPassantIndex = algebraicToIndex(token);
        break;
      case 5:
        halfmoveClock = std::stoi(token);
        break;
      case 6:
        fullmoveClock = std::stoi(token);
        break;
      default:
        break;
    }
  }
}

inline std::string Game::getFenStr() {
  std::ostringstream fen;
  fen << piecePlacementArrayToString(piecePlacement) << " "
      << colorToChar(activeColor) << " " << castlingAvailability << " "
      << indexToAlgebraic(enPassantIndex) << " "
      << std::to_string(halfmoveClock) << " " << std::to_string(fullmoveClock);

  return fen.str();
}

inline PiecePlacement Game::getPiecePlacement() { return piecePlacement; }

inline bool Game::move() {
  BoardIndex fromIndex, toIndex;

  if (BLACK_IS_CPU && activeColor == PieceColor::Black) {
    std::tie(fromIndex, toIndex) = generateCpuMove(PieceColor::Black);
  } else if (WHITE_IS_CPU && activeColor == PieceColor::White) {
    std::tie(fromIndex, toIndex) = generateCpuMove(PieceColor::White);
  } else {
    readMove(fromIndex, toIndex);
  }

  if (validateMove(fromIndex, toIndex)) {
    auto fromPiece = piecePlacement[fromIndex];
    auto fromColor = pieceColor(fromPiece);
    piecePlacement[toIndex] = piecePlacement[fromIndex];
    piecePlacement[fromIndex] = ChessPiece::Empty;
    activeColor = !activeColor;
    handleEnPassant(fromPiece, fromColor, fromIndex, toIndex);
    handlePawnPromotion(fromPiece, toIndex);
    // logger.log(this->getFenStr());
    return true;
  } else {
    return false;
  };

  return false;
}

inline void Game::readMove(BoardIndex &fromIndex, BoardIndex &toIndex) const {
  std::string fromAlgebraic, toAlgebraic;

  std::cout << "from square: ";
  std::cin >> fromAlgebraic;
  std::cout << "\n";
  fromIndex = algebraicToIndex(fromAlgebraic);

  std::cout << "to square: ";
  std::cin >> toAlgebraic;
  std::cout << "\n";
  toIndex = algebraicToIndex(toAlgebraic);
}

inline std::vector<BoardIndex> Game::getPieceLegalMoves(
    const ChessPiece &piece, const BoardIndex index) const {
  std::vector<BoardIndex> indexes = {};
  switch (piece) {
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

inline void Game::handleEnPassant(const ChessPiece fromPiece,
                                  const PieceColor fromColor,
                                  const BoardIndex fromIndex,
                                  const BoardIndex toIndex) {
  bool isPawn = (fromPiece == ChessPiece::BlackPawn ||
                 fromPiece == ChessPiece::WhitePawn);
  if (toIndex == enPassantIndex)
    piecePlacement[toIndex + (fromColor == PieceColor::White ? +8 : -8)] =
        ChessPiece::Empty;

  if (isPawn && abs(fromIndex - toIndex) == 16)
    enPassantIndex = fromIndex + (fromColor == PieceColor::White ? -8 : +8);
  else
    enPassantIndex.reset();
}

inline void Game::handlePawnPromotion(const ChessPiece fromPiece,
                                      const BoardIndex toIndex) {
  auto [file, rank] = indexToFileRank(toIndex);
  if (fromPiece == ChessPiece::WhitePawn && rank == 8) {
    piecePlacement[toIndex] = ChessPiece::WhiteQueen;
  }
  if (fromPiece == ChessPiece::BlackPawn && rank == 1) {
    piecePlacement[toIndex] = ChessPiece::BlackQueen;
  }
}

inline bool Game::validateMove(BoardIndex fromIndex, BoardIndex toIndex) const {
  auto fromPiece = piecePlacement[fromIndex];
  auto toPiece = piecePlacement[toIndex];

  if (fromPiece == ChessPiece::Empty) {
    return false;
  }

  auto fromColor = pieceColor(fromPiece);
  if (toPiece != ChessPiece::Empty && fromColor == pieceColor(toPiece)) {
    return false;
  }

  auto indexes = getPieceLegalMoves(fromPiece, fromIndex);
  if (std::find(indexes.cbegin(), indexes.cend(), toIndex) == indexes.cend()) {
    return false;
  }

  PiecePlacement newPiecePlacement = piecePlacement;
  newPiecePlacement[toIndex] = piecePlacement[fromIndex];
  newPiecePlacement[fromIndex] = ChessPiece::Empty;
  if (isKingInCheck(fromColor, newPiecePlacement)) {
    return false;
  }

  return true;
}

inline std::pair<BoardIndex, BoardIndex> Game::generateCpuMove(
    PieceColor cpuColor) const {
  std::vector<int> cpuPiecesIdxs;
  cpuPiecesIdxs.reserve(32);

  for (size_t i = 0; i < piecePlacement.size(); ++i) {
    auto piece = piecePlacement[i];
    if (piece != ChessPiece::Empty &&
        pieceColor(piecePlacement[i]) == cpuColor) {
      cpuPiecesIdxs.push_back(i);
    }
  }

  std::random_device rd;
  std::mt19937 g(rd());

  std::shuffle(cpuPiecesIdxs.begin(), cpuPiecesIdxs.end(), g);

  BoardIndex resFromIndex, resToIndex;

  bool stop = false;
  for (auto fromIndex : cpuPiecesIdxs) {
    if (stop) {
      break;
    }

    auto fromPiece = piecePlacement[fromIndex];
    auto indexes = getPieceLegalMoves(fromPiece, fromIndex);
    if (!indexes.size()) {
      continue;
    }

    std::shuffle(indexes.begin(), indexes.end(), g);

    for (auto toIndex : indexes) {
      if (validateMove(fromIndex, toIndex)) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(CPU_MOVE_DELAY_MS));
        return {fromIndex, toIndex};
      }
    }
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(CPU_MOVE_DELAY_MS));
  logger.log("CHECKMATE");
  return {resFromIndex, resToIndex};  // getting here implies checkmate
};

inline bool Game::isKingInCheck(const PieceColor color,
                                const PiecePlacement &piecePlacement) {
  ChessPiece kingPiece = color == PieceColor::White ? ChessPiece::WhiteKing
                                                    : ChessPiece::BlackKing;
  auto kingIter =
      std::find(piecePlacement.cbegin(), piecePlacement.cend(), kingPiece);
  BoardIndex kingIndex(kingIter - piecePlacement.cbegin());

  auto isPieceInIndexesLambda = [color, piecePlacement](
                                    ChessPiece searchPiece,
                                    std::vector<BoardIndex> &indexes) {
    for (auto &idx : indexes) {
      auto currentPiece = piecePlacement[idx];
      if (currentPiece == ChessPiece::Empty ||
          pieceColor(currentPiece) == color) {
        continue;
      }

      char idxPieceChar = chessPieceToChar(currentPiece);
      char pieceChar = chessPieceToChar(searchPiece);

      if (std::tolower(idxPieceChar) == std::tolower(pieceChar)) {
        return true;
      }

      if (searchPiece == ChessPiece::BlackBishop ||
          searchPiece == ChessPiece::WhiteBishop ||
          searchPiece == ChessPiece::BlackRook ||
          searchPiece == ChessPiece::WhiteRook) {
        if (currentPiece == ChessPiece::BlackQueen ||
            currentPiece == ChessPiece::WhiteQueen) {
          return true;
        }
      }
    }
    return false;
  };

  // pawn
  const std::vector<std::pair<int, int>> pawnOffsets =
      (color == PieceColor::White)
          ? std::vector<std::pair<int, int>>{
                {1, 1},
                {-1, 1},
            }
          : std::vector<std::pair<int, int>>{
                {1, -1},
                {-1, -1},
            };
  std::vector<BoardIndex> pawnIndexes =
      Piece::squareIndexes(kingIndex, pawnOffsets);
  if (isPieceInIndexesLambda(ChessPiece::BlackPawn, pawnIndexes)) {
    return true;
  }

  // knight
  const std::vector<std::pair<int, int>> knightOffsets = {
      {1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
  };
  auto knightIndexes = Piece::squareIndexes(kingIndex, knightOffsets);
  if (isPieceInIndexesLambda(ChessPiece::BlackKnight, knightIndexes)) {
    return true;
  }

  // bishop/queen
  const std::vector<std::pair<int, int>> diagOffsets = {
      {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
  auto diagIndexes =
      Piece::linearSquareIndexes(kingIndex, diagOffsets, piecePlacement);
  if (isPieceInIndexesLambda(ChessPiece::BlackBishop, diagIndexes)) {
    return true;
  }

  // rook/queen
  const std::vector<std::pair<int, int>> horizVertOffsets = {
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}};
  auto horizVertIndexes =
      Piece::linearSquareIndexes(kingIndex, horizVertOffsets, piecePlacement);
  if (isPieceInIndexesLambda(ChessPiece::BlackRook, horizVertIndexes)) {
    return true;
  }

  // king
  const std::vector<std::pair<int, int>> kingOffsets = {
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1},
  };
  auto kingIndexes = Piece::squareIndexes(kingIndex, kingOffsets);
  if (isPieceInIndexesLambda(ChessPiece::BlackKing, kingIndexes)) {
    return true;
  }

  return false;
}
