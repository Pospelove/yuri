#include "PieceView.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <cmrc/cmrc.hpp>
#include <glad/glad.h>
#include <imgui.h>
#include <lunasvg.h>
#include <map>
#include <stdexcept>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

CMRC_DECLARE(assets);

namespace {
struct PieceViewData
{
  std::map<std::string, lunasvg::Bitmap> bitmaps;
  std::map<std::string, GLuint> textures;
};
static PieceViewData g;
}

void PieceView::BeginPiece()
{

  PreparePieceBitmaps();

  PreparePieceTextures();

  auto texture = g.textures["assets/bB.svg"];

  ImGui::GetBackgroundDrawList()->AddImage(reinterpret_cast<void*>(texture),
                                           ImVec2(0, 0), ImVec2(64, 64));
}

void PieceView::EndPiece()
{
}

void PieceView::PreparePieceBitmaps()
{
  if (!g.bitmaps.empty()) {
    return;
  }

  std::vector<std::string> paths;
  paths.push_back("assets/bB.svg");
  paths.push_back("assets/bK.svg");
  paths.push_back("assets/bN.svg");
  paths.push_back("assets/bP.svg");
  paths.push_back("assets/bQ.svg");
  paths.push_back("assets/bR.svg");
  paths.push_back("assets/wB.svg");
  paths.push_back("assets/wK.svg");
  paths.push_back("assets/wN.svg");
  paths.push_back("assets/wP.svg");
  paths.push_back("assets/wQ.svg");
  paths.push_back("assets/wR.svg");

  for (auto& piecePath : paths) {

    cmrc::file file = cmrc::assets::get_filesystem().open(piecePath.data());

    std::string svg;
    for (char ch : file) {
      svg += ch;
    }

    auto document = lunasvg::Document::loadFromData(svg.data());
    auto bitmap = document->renderToBitmap(128, 128);

    if (!bitmap.valid()) {
      throw std::runtime_error("renderToBitmap failed for " + piecePath);
    }

    g.bitmaps[piecePath] = bitmap;
  }
}

void PieceView::PreparePieceTextures()
{
  if (!g.textures.empty()) {
    return;
  }

  for (auto& pair : g.bitmaps) {
    auto& colorAndId = pair.first;
    auto& bitmap = pair.second;

    std::vector<uint8_t> png;

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

    g.textures[colorAndId] = SurfaceToTexture(temp);

    SDL_FreeSurface(temp);
  }
}

// https://github.com/Darkwood9612/Chess/blob/master/src/TextureStorage.cpp
unsigned int PieceView::SurfaceToTexture(void* surface_)
{
  auto surface = reinterpret_cast<SDL_Surface*>(surface_);

  unsigned int texture = NULL;

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