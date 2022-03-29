#include "App.h"
#include <iostream>

#include "imgui.h"

#include "lunasvg.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <cmrc/cmrc.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <glad/glad.h>

#include <vector>

CMRC_DECLARE(assets);

// https://github.com/Darkwood9612/Chess/blob/master/src/TextureStorage.cpp
GLuint SurfaceToTexture(SDL_Surface* surface)
{
  unsigned int texture = NULL;
  if (texture != 0)
    glDeleteTextures(1, &texture);

  if (!surface)
    throw std::runtime_error("surface == nullptr");

  int Mode = GL_RGB;
  switch (surface->format->BytesPerPixel) {
    case 4:
      if (surface->format->Rmask == 0x000000ff)
        Mode = GL_RGBA;
      else
        Mode = GL_BGRA;
      break;

    case 3:
      if (surface->format->Rmask == 0x000000ff)
        Mode = GL_RGB;
      else
        Mode = GL_BGR;
      break;

    default:
      throw std::runtime_error("Error, image is not truecolor");
  }

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexImage2D(GL_TEXTURE_2D, 0, Mode, surface->w, surface->h, 0, Mode,
               GL_UNSIGNED_BYTE, surface->pixels);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return texture;
}

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

  void BeginInvisibleBackgroundWindow()
  {
    auto size = ImGui::GetIO().DisplaySize;

    // make controls widget width to be 1/3 of the main window width
    auto controlsWidth = size.x;

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(
      ImVec2(static_cast<float>(controlsWidth), static_cast<float>(size.y)),
      ImGuiCond_Always);

    int flags = 0;
    flags |= ImGuiWindowFlags_NoResize;
    flags |= ImGuiWindowFlags_NoMove;
    flags |= ImGuiWindowFlags_NoCollapse;
    flags |= ImGuiWindowFlags_NoBackground;
    flags |= ImGuiWindowFlags_NoTitleBar;
    ImGui::Begin("InvisibleWindow", nullptr, flags);
  }

  void Update() override
  {
    static bool onceFlag = false;
    if (!onceFlag) {
      onceFlag = true;

      stbi_write_png_to_func(
        [](void* context, void* data, int size) {
          auto& vec = *reinterpret_cast<std::vector<uint8_t>*>(context);
          vec.resize(size);
          std::memcpy(vec.data(), data, size);
        },
        &png, int(bitmap.width()), int(bitmap.height()), 4, bitmap.data(), 0);

      SDL_RWops* rw = SDL_RWFromMem(png.data(), png.size());
      SDL_Surface* temp = IMG_Load_RW(rw, 1);

      if (!temp) {
        std::string error = SDL_GetError();
        throw std::runtime_error(error);
      }

      texture = SurfaceToTexture(temp);
    }

    BeginInvisibleBackgroundWindow();
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
          ImGui::SetCursorPos(
            { boardStart.x + static_cast<float>(i) * cellSize,
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

    ImGui::GetBackgroundDrawList()->AddImage((void*)texture, ImVec2(0, 0),
                                             ImVec2(64, 64));

    ImGui::End();
  }
};

int main(int argc, char* argv[])
{
  try {
    App app = std::make_shared<Handler>();
    app.Run();
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}