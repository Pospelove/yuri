#pragma once
#include <optional>

struct ImVec2;
struct ImVec4;

class BoardCellEffect
{
public:
  enum class CellState
  {
    Patient,
    Hovered,
    Clicked
  };

  virtual ~BoardCellEffect() = default;
  virtual void OnEndBoard(int cellId, const ImVec2& cellStart, float cellSize,
                          bool isWhiteCell) = 0;
  virtual std::optional<ImVec4> GetCellColor(int cellId, bool isWhiteCell,
                                             CellState cellState) = 0;
};