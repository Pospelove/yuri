#include "BackgroundWindowView.h"

void BackgroundWindowView::BeginWindow()
{
  auto& size = ImGui::GetIO().DisplaySize;

  ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
  ImGui::SetNextWindowSize(
    ImVec2(static_cast<float>(size.x), static_cast<float>(size.y)),
    ImGuiCond_Always);

  int flags = 0;
  flags |= ImGuiWindowFlags_NoResize;
  flags |= ImGuiWindowFlags_NoMove;
  flags |= ImGuiWindowFlags_NoCollapse;
  flags |= ImGuiWindowFlags_NoBackground;
  flags |= ImGuiWindowFlags_NoTitleBar;
  ImGui::Begin("InvisibleWindow", nullptr, flags);
}

void BackgroundWindowView::EndWindow()
{
  ImGui::End();
}