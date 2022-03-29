#include "App.h"
#include "BackgroundWindowView.h"
#include "BoardView.h"
#include "FenUtils.h"
#include "LoadedPositionInfo.h"
#include "PieceView.h"
#include <array>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

#include "Piece.h"

std::set<int> moveOffsets = { -9, -8, -7, 1, 9, 8, 7, -1 };

class UpdateHandler : public IUpdateHandler
{
public:
  void Update() override
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    BackgroundWindowView::BeginWindow();
    BoardView::BeginBoard(paintedCellIds, targetedCellIds);

    static auto pos = FenUtils::PositionFromFen(FenUtils::StartFen);

    for (size_t i = 0; i < pos.cells.size(); ++i) {
      auto piece = pos.cells[i];
      if (piece) {
        auto p = BoardView::GetCellCursorPos(i);
        PieceView::BeginPiece(piece, p);
        PieceView::EndPiece();
      }
    }

    int cellId = BoardView::GetClickedCell();
    if (cellId != -1) {
      if (pos.cells[cellId] & pos.colorToMove) {
        if (paintedCellIds.count(cellId)) {
          paintedCellIds.clear();
        } else {
          paintedCellIds.clear();
          paintedCellIds.insert(cellId);
        }
      }
    }

    targetedCellIds.clear();
    for (auto offset : moveOffsets) {
      for (auto selected : paintedCellIds) {
        int cellId = selected + offset;
        if (cellId >= 0 && cellId <= 63) {
          bool aggressive = !!pos.cells[cellId];
          targetedCellIds[cellId] = aggressive;
        }
      }
    }

    BoardView::EndBoard();
    BackgroundWindowView::EndWindow();
  }

  std::set<int> paintedCellIds;
  std::map<int, bool> targetedCellIds;
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