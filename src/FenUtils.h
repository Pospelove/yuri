#pragma once
#include "LoadedPositionInfo.h"
#include <sstream>
#include <string>
#include <vector>

class FenUtils
{
public:
  constexpr static auto StartFen =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  static LoadedPositionInfo PositionFromFen(std::string fen)
  {
    if (fen.empty()) {
      return LoadedPositionInfo();
    }

    LoadedPositionInfo loadedPositionInfo;
    std::vector<std::string> sections;

    std::istringstream ss(fen);
    std::string token;

    while (std::getline(ss, token, ' ')) {
      sections.push_back(token);
    }

    int file = 0;
    int rank = 7;

    for (char symbol : sections.at(0)) {
      if (symbol == '/') {
        file = 0;
        rank--;
      } else {
        if (isdigit(symbol)) {
          file += symbol - '0';
        } else {
          int pieceColour =
            (toupper(symbol) == symbol) ? Piece::White : Piece::Black;
          int pieceType = Piece::GetPieceTypeFromSymbol(symbol);
          loadedPositionInfo.cells[rank * 8ull + file] =
            pieceType | pieceColour;
          file++;
        }
      }
    }

    loadedPositionInfo.colorToMove =
      (sections.at(1) == "w") ? Piece::White : Piece::Black;

    std::string castlingRights =
      (sections.size() > 2) ? sections.at(2) : "KQkq";
    loadedPositionInfo.whiteCastleKingside =
      castlingRights.find_first_of("K") != std::string::npos;
    loadedPositionInfo.whiteCastleQueenside =
      castlingRights.find_first_of("Q") != std::string::npos;
    loadedPositionInfo.blackCastleKingside =
      castlingRights.find_first_of("k") != std::string::npos;
    loadedPositionInfo.blackCastleQueenside =
      castlingRights.find_first_of("q") != std::string::npos;

    /*if (sections.size() > 3) {
      std::string enPassantFileName;

      enPassantFileName.push_back(sections[3][0]);
      if (BoardRepresentation.fileNames.Contains(enPassantFileName)) {
        loadedPositionInfo.epFile =
          BoardRepresentation.fileNames.IndexOf(enPassantFileName) + 1;
      }
    }

    // Half-move clock
    if (sections.Length > 4) {
      int.TryParse(sections[4], out loadedPositionInfo.plyCount);
    }*/
    return loadedPositionInfo;
  }
};