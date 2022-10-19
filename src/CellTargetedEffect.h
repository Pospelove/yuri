#pragma once
#include "BoardCellEffect.h"
#include <imgui.h>
#include <map>

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
    ImGui::GetWindowDrawList()->AddCircleFilled(center, radius, color32);
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