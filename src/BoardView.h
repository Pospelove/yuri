#pragma once
#include "BoardCellEffect.h"
#include <imgui.h>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <vector>

class BoardView
{
public:
  static void BeginBoard(
    const std::vector<std::shared_ptr<BoardCellEffect>>& effects, float &scale);
  static void EndBoard();

  static const ImVec2& GetCellCursorPos(int cell);
  static int GetClickedCell();

private:
  static void RecalculateAppearanceK();
  static void DrawCell(int i, int j, ImVec2 boardStart);
  static void OnClick(int cellId);
};