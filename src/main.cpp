#include "App.h"
#include "BackgroundWindowView.h"
#include "BoardView.h"
#include "PieceView.h"
#include <array>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

#include "Piece.h"

std::string startFen =
  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

class LoadedPositionInfo
{
public:
  LoadedPositionInfo() { cells.fill(0); }

  std::array<int, 64> cells;
  bool whiteToMove = true;
  bool whiteCastleKingside = false;
  bool whiteCastleQueenside = false;
  bool blackCastleKingside = false;
  bool blackCastleQueenside = false;
};

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
        loadedPositionInfo.cells[rank * 8ull + file] = pieceType | pieceColour;
        file++;
      }
    }
  }

  loadedPositionInfo.whiteToMove = (sections.at(1) == "w");

  std::string castlingRights = (sections.size() > 2) ? sections.at(2) : "KQkq";
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

class UpdateHandler : public IUpdateHandler
{
public:
  void Update() override
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    BackgroundWindowView::BeginWindow();
    BoardView::BeginBoard();

    startFen =
      "rnbqkb1r/pp2pppp/3p1n2/8/3NP3/2N5/PPP2PPP/R1BQKB1R b KQkq - 2 5";

    static auto pos = PositionFromFen(startFen);

    for (size_t i = 0; i < pos.cells.size(); ++i) {
      auto piece = pos.cells[i];
      if (piece) {
        auto p = BoardView::GetCellCursorPos(i);
        PieceView::BeginPiece(piece, p);
        PieceView::EndPiece();
      }
    }

    BoardView::EndBoard();
    BackgroundWindowView::EndWindow();
  }
};

int main(int argc, char* argv[])
{
  try {
    App app = std::make_shared<UpdateHandler>();
    app.Run();
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}