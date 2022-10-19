#include "App.h"
#include "BackgroundWindowView.h"
#include "BoardView.h"
#include "CellTargetedEffect.h"
#include "FenUtils.h"
#include "LoadedPositionInfo.h"
#include "PieceView.h"
#include <array>
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

#include "Piece.h"

std::set<int> moveOffsets = { -9, -8, -7, 1, 9, 8, 7, -1 };

class UpdateHandler : public IUpdateHandler
{
public:
  void Update() override
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    ImVec4 c = ImGui::GetStyleColorVec4(ImGuiCol_TitleBgActive);
    ImGui::PushStyleColor(ImGuiCol_TitleBg, c);
    ImGui::Begin("Settings");
    BoardView::BeginSettings(scale);
    BoardView::EndSettings();
    if (ImGui::IsMouseReleased(0)) {
      ImGui::SetWindowFocus();
    }
    ImGui::End();
    ImGui::PopStyleColor();

    BackgroundWindowView::BeginWindow();
    BoardView::BeginBoard(effects, scale);

    static LoadedPositionInfo pos =
      FenUtils::PositionFromFen(FenUtils::StartFen);

    for (size_t i = 0; i < pos.cells.size(); ++i) {
      auto piece = pos.cells[i];
      if (piece) {
        auto p = BoardView::GetCellCursorPos(i);
        PieceView::BeginPiece(piece, p, scale);
        PieceView::EndPiece();
      }
    }

    effects.resize(1);

    int cellId = BoardView::GetClickedCell();
    if (cellId == -1) {
      effects[0] = std::make_shared<CellTargetedEffect>();
    } else {
      std::map<int, bool> map;
      map[cellId] = true;
      effects[0] = std::make_shared<CellTargetedEffect>(map);
    }

    BoardView::EndBoard();
    BackgroundWindowView::EndWindow();
  }

private:
  std::vector<std::shared_ptr<BoardCellEffect>> effects;
  float scale = 0.8f;
};

int main(int argc, char* argv[])
{
  try {
    App app = std::make_shared<UpdateHandler>();
    app.Run();
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}