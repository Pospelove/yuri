#pragma once
#include <imgui.h>

class PieceView
{
public:
  static void BeginPiece(int piece, const ImVec2& cursorPos, float scale);
  static void EndPiece();

private:
  static void PreparePieceBitmaps();
  static void PreparePieceTextures();
  static unsigned int SurfaceToTexture(void* surface);
  static int TexturePathToPieceCode(const char* path);
};