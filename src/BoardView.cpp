#include "BoardView.h"
#include <algorithm>

void BoardView::BeginBoard()
{
  auto cellSize = 128.f;

  float minWindowSize =
    std::min(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
  minWindowSize *= 0.8f;
  cellSize = minWindowSize / 8;

  ImVec2 displayCenter = ImGui::GetIO().DisplaySize;
  displayCenter.x /= 2;
  displayCenter.y /= 2;

  ImVec2 boardSizePrediction = { cellSize * 8, cellSize * 8 };

  ImVec2 boardStart = displayCenter;
  boardStart.x -= boardSizePrediction.x / 2;
  boardStart.y -= boardSizePrediction.y / 2;

  for (int i = 0; i < 8; i++) {
    bool flag = false;
    for (int j = 0; j < 8; j++) {
      ImVec4 colorWhite{ 240.f / 256.f, 217.f / 256.f, 181.f / 256.f, 1 };
      ImVec4 colorBlack{ 181.f / 256.f, 136.f / 256.f, 99.f / 256.f, 1 };
      ImGui::SetCursorPos({ boardStart.x + static_cast<float>(i) * cellSize,
                            boardStart.y + static_cast<float>(j) * cellSize });

      ImVec4 color = ((i + j) % 2 == 0) ? colorWhite : colorBlack;
      ImVec4 colorActive = color;
      colorActive.w *= 0.75f;
      ImVec4 colorHovered = color;
      colorHovered.w *= 0.75f;

      ImGui::PushStyleColor(ImGuiCol_Button, color);
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, colorActive);
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colorHovered);
      ImGui::Button(" ", { cellSize, cellSize });
      ImGui::PopStyleColor(3);
      flag = true;
    }
  }
}

void BoardView::EndBoard()
{
}