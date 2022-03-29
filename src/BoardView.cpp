#include "BoardView.h"
#include <algorithm>
#include <array>
#include <vector>

namespace {
ImVec4 colorWhite{ 240.f / 256.f, 217.f / 256.f, 181.f / 256.f, 1 };
ImVec4 colorBlack{ 181.f / 256.f, 136.f / 256.f, 99.f / 256.f, 1 };
ImVec4 selectedColorWhite =
  ImVec4{ 130.f / 256.f, 151.f / 256.f, 105.f / 256.f, 1.f };
ImVec4 selectedColorBlack =
  ImVec4{ 100.f / 256.f, 111.f / 256.f, 64.f / 256.f, 1.f };

struct AvailableCellCircle
{
  ImVec2 center;
  float radius = 0.f;
  ImU32 color = static_cast<ImU32>(-1);
  bool filled = true;
};

struct BoardViewData
{
  BoardViewData()
  {
    cellCursorPositions.fill({ 0, 0 });
    appearanceK.fill(0.f);
  }

  std::array<ImVec2, 64> cellCursorPositions;
  std::array<float, 64> appearanceK;
  ImVec2 nullPosition = { 0, 0 };
  int clickedCellId = -1;

  std::vector<AvailableCellCircle> availableCellCircles;
};
static BoardViewData g;
}

void BoardView::BeginBoard(const std::set<int>& paintedCellIds,
                           const std::map<int, bool>& targetedCellIds)
{
  if (ImGui::Button("Reset")) {
    g.appearanceK.fill(0);
  }

  static bool enableAnimation = true;
  ImGui::Checkbox("Animation", &enableAnimation);
  if (!enableAnimation) {
    g.appearanceK.fill(1);
  }

  RecalculateAppearanceK();

  float minWindowSize =
    std::min(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
  minWindowSize *= 0.8f;
  auto cellSize = minWindowSize / 8;

  ImVec2 displayCenter = ImGui::GetIO().DisplaySize;
  displayCenter.x /= 2;
  displayCenter.y /= 2;

  ImVec2 boardSizePrediction = { cellSize * 8, cellSize * 8 };

  ImVec2 boardStart = displayCenter;
  boardStart.x -= boardSizePrediction.x / 2;
  boardStart.y -= boardSizePrediction.y / 2;

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      DrawCell(i, j, boardStart, cellSize, paintedCellIds, targetedCellIds);
    }
  }
}

void BoardView::EndBoard()
{
  g.clickedCellId = -1;
  for (auto& circleInfo : g.availableCellCircles) {
    if (circleInfo.filled) {
      ImGui::GetForegroundDrawList()->AddCircleFilled(
        circleInfo.center, circleInfo.radius, circleInfo.color);
    } else {
      ImGui::GetForegroundDrawList()->AddCircle(
        circleInfo.center, circleInfo.radius * 0.95f, circleInfo.color, 0,
        circleInfo.radius * 0.1f);
    }
  }
  g.availableCellCircles.clear();
}

const ImVec2& BoardView::GetCellCursorPos(int cell)
{
  if (cell < 0 || static_cast<int>(g.cellCursorPositions.size()) <= cell) {
    return g.nullPosition;
  }
  return g.cellCursorPositions[cell];
}

int BoardView::GetClickedCell()
{
  return g.clickedCellId;
}

void BoardView::RecalculateAppearanceK()
{
  float minAppearanceKToStartNext = 0.1;

  for (size_t i = 0; i < g.appearanceK.size(); ++i) {
    auto& appearanceK = g.appearanceK[i];
    float boost = 0.02 + abs(appearanceK - 1) / 15;
    if (!i || g.appearanceK[i - 1] > minAppearanceKToStartNext) {
      appearanceK += boost;
    }
    appearanceK = std::min(appearanceK, 1.f);
  }

  if (ImGui::GetIO().MouseClicked[0]) {
    g.appearanceK.fill(1.f);
  }
}

void BoardView::DrawCell(int i, int j, ImVec2 boardStart, float cellSize,
                         const std::set<int>& paintedCellIds,
                         const std::map<int, bool>& targetedCellIds)
{
  const auto cellId =
    g.cellCursorPositions.size() - 1 - (j * 8ull + (8 - i - 1));

  ImVec2 pieceStartCursorPos = {
    boardStart.x + static_cast<float>(i) * cellSize,
    boardStart.y + static_cast<float>(j) * cellSize
  };

  g.cellCursorPositions[cellId] = pieceStartCursorPos;

  // Hide pieces while board is loading
  if (g.appearanceK.back() < 0.99) {
    g.cellCursorPositions[cellId].y = -1000;
  }

  ImVec2 realCursorPos = pieceStartCursorPos;
  realCursorPos.x += (1 - g.appearanceK[cellId]) * cellSize / 2;
  realCursorPos.y += (1 - g.appearanceK[cellId]) * cellSize / 2;

  ImGui::SetCursorPos(realCursorPos);

  bool isWhiteCell = ((i + j) % 2 == 0);
  ImVec4 color = isWhiteCell ? colorWhite : colorBlack;
  if (paintedCellIds.count(static_cast<int>(cellId))) {
    color = isWhiteCell ? selectedColorWhite : selectedColorBlack;
  }
  ImVec4 colorActive = color;
  ImVec4 colorHovered = color;
  if (targetedCellIds.count(static_cast<int>(cellId))) {
    colorActive = colorHovered =
      isWhiteCell ? selectedColorWhite : selectedColorBlack;
  }

  ImVec2 buttonSize = { cellSize * g.appearanceK[cellId],
                        cellSize * g.appearanceK[cellId] };

  if (buttonSize.x > 1) {
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, colorActive);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colorHovered);
    ImGui::Button("##!", buttonSize);
    if (ImGui::GetIO().MouseClicked[0] && ImGui::IsItemHovered()) {
      OnClick(cellId);
    }
    ImGui::PopStyleColor(3);
    char buf[32];
    sprintf(buf, "", static_cast<int>(cellId));
    ImGui::GetForegroundDrawList()->AddText(realCursorPos, -1, buf);

    if (targetedCellIds.count(static_cast<int>(cellId))) {
      ImVec2 center = realCursorPos;
      center.x += cellSize / 2;
      center.y += cellSize / 2;
      auto color = isWhiteCell ? selectedColorWhite : selectedColorBlack;
      auto color32 = ImGui::ColorConvertFloat4ToU32(color);

      bool aggressive = targetedCellIds.find(static_cast<int>(cellId))->second;
      if (aggressive) {
        if (!ImGui::IsItemHovered()) {
          g.availableCellCircles.push_back({});
          g.availableCellCircles.back().center = center;
          g.availableCellCircles.back().radius = cellSize * 0.5f;
          g.availableCellCircles.back().color = color32;
          g.availableCellCircles.back().filled = false;
        }
      } else {
        g.availableCellCircles.push_back({});
        g.availableCellCircles.back().center = center;
        g.availableCellCircles.back().radius = cellSize * 0.125f;
        g.availableCellCircles.back().color = color32;
      }
    }
  }
}

void BoardView::OnClick(int cellId)
{
  g.clickedCellId = cellId;
}