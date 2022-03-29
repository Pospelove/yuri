#include "PieceView.h"
#include "Piece.h"
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
  std::map<int, lunasvg::Bitmap> bitmaps;
  std::map<int, GLuint> textures;
  float lastCellSize = -1;
};
static PieceViewData g;
}

void PieceView::BeginPiece(int piece, const ImVec2& cursorPos)
{
  float minWindowSize =
    std::min(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
  minWindowSize *= 0.8f;
  auto cellSize = minWindowSize / 8;

  if (cellSize != g.lastCellSize) {
    g.lastCellSize = cellSize;
    for (auto p : g.textures) {
      glDeleteTextures(1, &p.second);
    }
    g.bitmaps.clear();
    g.textures.clear();
  }

  PreparePieceBitmaps();
  PreparePieceTextures();
  auto texture = g.textures[piece];

  auto pos2 = cursorPos;
  pos2.x += g.lastCellSize;
  pos2.y += g.lastCellSize;

  ImGui::GetForegroundDrawList()->AddImage(reinterpret_cast<void*>(texture),
                                           cursorPos, pos2);
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
    auto bitmap = document->renderToBitmap(g.lastCellSize, g.lastCellSize);

    if (!bitmap.valid()) {
      throw std::runtime_error("renderToBitmap failed for " + piecePath);
    }

    g.bitmaps[TexturePathToPieceCode(piecePath.data())] = bitmap;
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

int PieceView::TexturePathToPieceCode(const char* path)
{
  std::vector<char> buf;
  buf.resize(std::strlen(path) + 1, 0);
  std::strcpy(buf.data(), path);

  constexpr int removeChars = sizeof(".svg");
  buf[std::strlen(path) - removeChars + 1] = 0;

  std::string str(buf.data());
  auto pieceUpper = str.back();
  str.pop_back();
  auto color = str.back();
  str.clear();

  int pieceType = Piece::GetPieceTypeFromSymbol(pieceUpper);
  int pieceColor = color == 'b' ? Piece::Black : Piece::White;
  return pieceType | pieceColor;
}