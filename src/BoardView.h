#pragma once
#include <imgui.h>
#include <map>
#include <set>

class BoardView
{
public:
  static void BeginBoard(const std::set<int>& paintedCellIds,
                         const std::map<int, bool>& targetedCellIds);
  static void EndBoard();

  static const ImVec2& GetCellCursorPos(int cell);
  static int GetClickedCell();

private:
  static void RecalculateAppearanceK();
  static void DrawCell(int i, int j, ImVec2 boardStart, float cellSize,
                       const std::set<int>& paintedCellIds,
                       const std::map<int, bool>& targetedCellIds);
  static void OnClick(int cellId);
};