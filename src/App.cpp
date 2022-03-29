#include "App.h"

#include <SDL.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

struct App::Impl
{
  int windowWidth = 1280, windowHeight = 720, windowWidthMinimum = 500,
      windowHeightMinimum = 300;
  const char* windowName = "Yuri";
  ImVec4 background = ImVec4(35 / 255.0f, 35 / 255.0f, 35 / 255.0f, 1.00f);

  SDL_Window* window = nullptr;
  SDL_GLContext glContext = nullptr;
  const char* glslVersion = "";
  bool loop = true;

  std::shared_ptr<IUpdateHandler> updateHandler;
};

App::App(std::shared_ptr<IUpdateHandler> updateHandler)
{
  pImpl = std::make_shared<Impl>();
  pImpl->updateHandler = updateHandler;

  InitSdl();
  InitOpenGlAttributes();
  CreateSdlWindow();
  CreateSdlGlContext();
  InitGlad();
  InitImgui();
}

App::~App()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(pImpl->glContext);
  SDL_DestroyWindow(pImpl->window);
  SDL_Quit();
}

void App::Run()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  while (pImpl->loop) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    HandleInputs();
    StartImguiFrame();
    if (pImpl->updateHandler) {
      pImpl->updateHandler->Update();
    }
    FinishImguiFrame();
  }
}

void App::InitSdl()
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::string error = SDL_GetError();
    throw std::runtime_error(error);
  }
}

void App::InitOpenGlAttributes()
{
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                      SDL_GL_CONTEXT_PROFILE_CORE);

#if WIN32
  pImpl->glslVersion = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
#  error                                                                      \
    "Please review GL attributes https://decovar.dev/blog/2019/05/26/sdl-imgui/"
#endif
}

void App::CreateSdlWindow()
{
  auto flags = static_cast<SDL_WindowFlags>(
    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  pImpl->window = SDL_CreateWindow(pImpl->windowName, SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED, pImpl->windowWidth,
                                   pImpl->windowHeight, flags);

  // limit to which minimum size user can resize the window
  SDL_SetWindowMinimumSize(pImpl->window, pImpl->windowWidthMinimum,
                           pImpl->windowHeightMinimum);
}

void App::CreateSdlGlContext()
{
  pImpl->glContext = SDL_GL_CreateContext(pImpl->window);
  SDL_GL_MakeCurrent(pImpl->window, pImpl->glContext);

  // enable VSync
  SDL_GL_SetSwapInterval(1);
}

void App::InitGlad()
{
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    throw std::runtime_error("Couldn't initialize glad");
  }
  glViewport(0, 0, pImpl->windowWidth, pImpl->windowHeight);
}

void App::InitImgui()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForOpenGL(pImpl->window, pImpl->glContext);
  ImGui_ImplOpenGL3_Init(pImpl->glslVersion);

  auto& background = pImpl->background;
  glClearColor(background.x, background.y, background.z, background.w);
}

void App::HandleInputs()
{
  SDL_Event event;
  while (SDL_PollEvent(&event)) {

    ImGui_ImplSDL2_ProcessEvent(&event);

    switch (event.type) {
      case SDL_QUIT:
        pImpl->loop = false;
        break;

      case SDL_WINDOWEVENT:
        switch (event.window.event) {
          case SDL_WINDOWEVENT_RESIZED:
            pImpl->windowWidth = event.window.data1;
            pImpl->windowHeight = event.window.data2;
            glViewport(0, 0, pImpl->windowWidth, pImpl->windowHeight);
            break;
        }
        break;

      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            pImpl->loop = false;
            break;
        }
        break;
    }
  }
}

void App::StartImguiFrame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(pImpl->window);
  ImGui::NewFrame();
}

void App::FinishImguiFrame()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  SDL_GL_SwapWindow(pImpl->window);
}