#include "Piece.h"
#include <array>
#include <cctype>
#include <cstring>

int Piece::GetPieceTypeFromSymbol(char symbol)
{
  symbol = tolower(symbol);
  std::array<int, 'r' + 1> pieceTypeFromSymbol;
  pieceTypeFromSymbol['k'] = Piece::King;
  pieceTypeFromSymbol['p'] = Piece::Pawn;
  pieceTypeFromSymbol['n'] = Piece::Knight;
  pieceTypeFromSymbol['b'] = Piece::Bishop;
  pieceTypeFromSymbol['r'] = Piece::Rook;
  pieceTypeFromSymbol['q'] = Piece::Queen;
  return pieceTypeFromSymbol[symbol];
}