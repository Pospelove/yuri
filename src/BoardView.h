#pragma once
#include <imgui.h>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <vector>

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

class BoardView
{
public:
  static void BeginBoard(
    const std::vector<std::shared_ptr<BoardCellEffect>>& effects);
  static void EndBoard();

  static const ImVec2& GetCellCursorPos(int cell);
  static int GetClickedCell();

private:
  static void RecalculateAppearanceK();
  static void DrawCell(int i, int j, ImVec2 boardStart);
  static void OnClick(int cellId);
};