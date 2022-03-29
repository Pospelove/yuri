#pragma once
#include <imgui.h>

class BoardView
{
public:
  static void BeginBoard();
  static void EndBoard();

  static const ImVec2 &GetCellCursorPos(int cell);
};