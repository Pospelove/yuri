#include "App.h"
#include <iostream>

#include "imgui.h"

#include "lunasvg.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <cmrc/cmrc.hpp>

#include <glad/glad.h>

#include <vector>

CMRC_DECLARE(assets);

class Handler : public IUpdateHandler
{
public:
  lunasvg::Bitmap bitmap;
  std::vector<uint8_t> png;
  GLuint texture = 0;

  Handler()
  {
    cmrc::file file = cmrc::assets::get_filesystem().open("assets/bN.svg");

    std::string svg;
    for (char ch : file) {
      svg += ch;
    }

    auto document = lunasvg::Document::loadFromData(svg.data());
    bitmap = document->renderToBitmap(128, 128);

    if (!bitmap.valid()) {
      throw std::runtime_error("renderToBitmap failed");
    }
  }
};

// ImGui::GetBackgroundDrawList()->AddImage((void*)texture, ImVec2(0, 0),
// ImVec2(64, 64));

#include "BackgroundWindowView.h"
#include "BoardView.h"
#include "PieceView.h"

class UpdateHandler : public IUpdateHandler
{
public:
  void Update() override
  {
    BackgroundWindowView::BeginWindow();
    BoardView::BeginBoard();

    PieceView::BeginPiece();
    PieceView::EndPiece();

    BoardView::EndBoard();
    BackgroundWindowView::EndWindow();
  }
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