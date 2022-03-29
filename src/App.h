#pragma once
#include "IUpdateHandler.h"
#include <memory>

class App
{
public:
  App(std::shared_ptr<IUpdateHandler> updateHandler = nullptr);
  ~App();
  void Run();

private:
  void InitSdl();
  void InitOpenGlAttributes();
  void CreateSdlWindow();
  void CreateSdlGlContext();
  void InitGlad();
  void InitImgui();

  void HandleInputs();
  void StartImguiFrame();
  void FinishImguiFrame();

  struct Impl;
  std::shared_ptr<Impl> pImpl;
};