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

class CellTargetedEffect : public BoardCellEffect
{
public:
  CellTargetedEffect(std::map<int, bool> cellIds_ = {})
    : cellIds(cellIds_)
  {
  }

  void OnEndBoard(int cellId, const ImVec2& cellStart, float cellSize,
                  bool isWhiteCell) override
  {
    if (!cellIds.count(cellId)) {
      return;
    }

    ImVec2 center = cellStart;
    center.x += cellSize / 2;
    center.y += cellSize / 2;
    auto color = isWhiteCell ? selectedColorWhite : selectedColorBlack;
    auto color32 = ImGui::ColorConvertFloat4ToU32(color);
    auto radius = cellSize * 0.125f;
    ImGui::GetForegroundDrawList()->AddCircleFilled(center, radius, color32);
  }

  std::optional<ImVec4> GetCellColor(int cellId, bool isWhiteCell,
                                     CellState cellState) override
  {
    return std::nullopt;
  }

private:
  const std::map<int, bool> cellIds;
  const ImVec4 selectedColorWhite =
    ImVec4{ 130.f / 256.f, 151.f / 256.f, 105.f / 256.f, 1.f };
  const ImVec4 selectedColorBlack =
    ImVec4{ 100.f / 256.f, 111.f / 256.f, 64.f / 256.f, 1.f };
};

class UpdateHandler : public IUpdateHandler
{
public:
  void Update() override
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    BackgroundWindowView::BeginWindow();
    BoardView::BeginBoard(effects);

    static auto pos = FenUtils::PositionFromFen(FenUtils::StartFen);

    for (size_t i = 0; i < pos.cells.size(); ++i) {
      auto piece = pos.cells[i];
      if (piece) {
        auto p = BoardView::GetCellCursorPos(i);
        PieceView::BeginPiece(piece, p);
        PieceView::EndPiece();
      }
    }

    /* int cellId = BoardView::GetClickedCell();
    if (cellId != -1) {
      if (pos.cells[cellId] & pos.colorToMove) {
        if (paintedCellIds.count(cellId)) {
          paintedCellIds.clear();
        } else {
          paintedCellIds.clear();
          paintedCellIds.insert(cellId);
        }
      }
    }*/

    effects.resize(1);

    int cellId = BoardView::GetClickedCell();
    if (cellId == -1) {
      effects[0] = std::make_shared<CellTargetedEffect>();
    } else {
      std::map<int, bool> map;
      map[cellId] = true;
      effects[0] = std::make_shared<CellTargetedEffect>(map);
    }

    targetedCellIds = { { 0, true }, { 2, true }, { 3, true } };

    // UpdateTargetedCellEffects();

    /* targetedCellIds.clear();
     for (auto offset : moveOffsets) {
       for (auto selected : paintedCellIds) {
         int cellId = selected + offset;
         if (cellId >= 0 && cellId <= 63) {
           bool aggressive = !!pos.cells[cellId];
           targetedCellIds[cellId] = aggressive;
         }
       }
       UpdateTargetedCellEffects();
     }*/

    BoardView::EndBoard();
    BackgroundWindowView::EndWindow();
  }

private:
  /* void UpdateTargetedCellEffects()
  {
    effects.resize(1);
    effects[0] = std::make_shared<CellTargetedEffect>(targetedCellIds);
    ImGui::Text("%d", targetedCellIds.size());
  }*/

  std::set<int> paintedCellIds;
  std::map<int, bool> targetedCellIds;

  std::vector<std::shared_ptr<BoardCellEffect>> effects;
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