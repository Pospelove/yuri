#pragma once

class Piece
{
public:
  constexpr static int None = 0;
  constexpr static int King = 1;
  constexpr static int Pawn = 2;
  constexpr static int Knight = 3;
  constexpr static int Bishop = 5;
  constexpr static int Rook = 6;
  constexpr static int Queen = 7;

  constexpr static int White = 8;
  constexpr static int Black = 16;

  static int GetPieceTypeFromSymbol(char symbol);
};