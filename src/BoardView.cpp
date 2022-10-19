#include "BoardView.h"
#include <algorithm>
#include <array>
#include <vector>

namespace {
int GetCellId(int i, int j)
{
  return 64 - 1 - (j * 8ull + (8 - i - 1));
}

bool IsWhiteCell(int i, int j)
{
  return (i + j) % 2 == 0;
}

ImVec4 colorWhite{ 240.f / 256.f, 217.f / 256.f, 181.f / 256.f, 1 };
ImVec4 colorBlack{ 181.f / 256.f, 136.f / 256.f, 99.f / 256.f, 1 };

struct BoardViewData
{
  BoardViewData()
  {
    cellCursorPositions.fill({ 0, 0 });
    cellCursorPositionsScrollOffsetIncluded.fill({ 0, 0 });
    appearanceK.fill(0.f);
  }

  std::array<ImVec2, 64> cellCursorPositions,
    cellCursorPositionsScrollOffsetIncluded;
  std::array<float, 64> appearanceK;
  ImVec2 nullPosition = { 0, 0 };
  int clickedCellId = -1;
  float cellSize = 0.f;
  std::vector<std::shared_ptr<BoardCellEffect>> effects;
};
static BoardViewData g;
}

void BoardView::BeginBoard(
  const std::vector<std::shared_ptr<BoardCellEffect>>& effects, float& scale)
{
  g.effects = effects;

  RecalculateAppearanceK();

  float minWindowSize =
    std::min(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
  minWindowSize *= scale;

  g.cellSize = minWindowSize / 8;

  ImVec2 displayCenter = ImGui::GetIO().DisplaySize;
  displayCenter.x /= 2;
  displayCenter.y /= 2;

  ImVec2 boardSizePrediction = { g.cellSize * 8, g.cellSize * 8 };

  ImVec2 boardStart = displayCenter;
  boardStart.x -= boardSizePrediction.x / 2;
  boardStart.y -= boardSizePrediction.y / 2;

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      DrawCell(i, j, boardStart);
    }
  }
}

void BoardView::EndBoard()
{
  /* g.clickedCellId = -1;
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
  g.availableCellCircles.clear();*/

  for (auto& effect : g.effects) {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        int cellId = GetCellId(i, j);
        effect->OnEndBoard(cellId, GetCellCursorPos(cellId), g.cellSize,
                           IsWhiteCell(i, j));
      }
    }
  }
}

void BoardView::BeginSettings(float& scale)
{
  if (ImGui::Button("Reset")) {
    g.appearanceK.fill(0);
  }

  static bool enableAnimation = true;
  ImGui::Checkbox("Animation", &enableAnimation);
  if (!enableAnimation) {
    g.appearanceK.fill(1);
  }

  ImGui::SetNextItemWidth(100);
  ImGui::SliderFloat("Scale", &scale, 0.0f, 2.f);
}

void BoardView::EndSettings()
{
}

const ImVec2& BoardView::GetCellCursorPos(int cell)
{
  if (cell < 0 || static_cast<int>(g.cellCursorPositions.size()) <= cell) {
    return g.nullPosition;
  }

  float scrollOffset = ImGui::GetScrollY();

  g.cellCursorPositionsScrollOffsetIncluded[cell] =
    g.cellCursorPositions[cell];
  g.cellCursorPositionsScrollOffsetIncluded[cell].y -= scrollOffset;

  return g.cellCursorPositionsScrollOffsetIncluded[cell];
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

void BoardView::DrawCell(int i, int j, ImVec2 boardStart)
{
  const auto cellId = GetCellId(i, j);

  ImVec2 pieceStartCursorPos = {
    boardStart.x + static_cast<float>(i) * g.cellSize,
    boardStart.y + static_cast<float>(j) * g.cellSize
  };

  g.cellCursorPositions[cellId] = pieceStartCursorPos;

  // Hide pieces while board is loading
  if (g.appearanceK.back() < 0.99) {
    g.cellCursorPositions[cellId].y = -1000;
  }

  ImVec2 realCursorPos = pieceStartCursorPos;
  realCursorPos.x += (1 - g.appearanceK[cellId]) * g.cellSize / 2;
  realCursorPos.y += (1 - g.appearanceK[cellId]) * g.cellSize / 2;

  ImGui::SetCursorPos(realCursorPos);

  bool isWhiteCell = IsWhiteCell(i, j);
  ImVec4 color = isWhiteCell ? colorWhite : colorBlack;
  for (auto& effect : g.effects) {
    if (auto effectColor = effect->GetCellColor(
          cellId, isWhiteCell, BoardCellEffect::CellState::Patient)) {
      color = *effectColor;
      break;
    }
  }
  ImVec4 colorActive = color;
  for (auto& effect : g.effects) {
    if (auto effectColor = effect->GetCellColor(
          cellId, isWhiteCell, BoardCellEffect::CellState::Clicked)) {
      colorActive = *effectColor;
      break;
    }
  }
  ImVec4 colorHovered = color;
  for (auto& effect : g.effects) {
    if (auto effectColor = effect->GetCellColor(
          cellId, isWhiteCell, BoardCellEffect::CellState::Hovered)) {
      colorHovered = *effectColor;
      break;
    }
  }

  ImVec2 buttonSize = { g.cellSize * g.appearanceK[cellId],
                        g.cellSize * g.appearanceK[cellId] };

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

    /* if (targetedCellIds.count(static_cast<int>(cellId))) {
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
    }*/
  }
}

void BoardView::OnClick(int cellId)
{
  g.clickedCellId = cellId;
}