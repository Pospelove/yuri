#pragma once

class PieceView
{
public:
  static void BeginPiece();
  static void EndPiece();

private:
  static void PreparePieceBitmaps();
  static void PreparePieceTextures();
  static unsigned int SurfaceToTexture(void* surface);
};