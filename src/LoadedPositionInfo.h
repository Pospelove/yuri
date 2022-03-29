#pragma once
#include "Piece.h"
#include <array>

class LoadedPositionInfo
{
public:
  LoadedPositionInfo() { cells.fill(0); }

  std::array<int, 64> cells;
  int colorToMove = Piece::White;
  bool whiteCastleKingside = false;
  bool whiteCastleQueenside = false;
  bool blackCastleKingside = false;
  bool blackCastleQueenside = false;
};