#pragma once

#include <memory>
#include <stddef.h>
#include <string>
#include <sys/ioctl.h>
#include <vector>

#include "../game.hpp"
#include "../timeControl.hpp"
#include "../types.hpp"

class FrameBuilder;

class Renderer
{
public:
  Renderer() = delete;
  Renderer(Game &);
  ~Renderer();

  void initScreen();
  void cleanupScreen();
  void renderFrame();

private:
  Game &game;
  std::unique_ptr<FrameBuilder> frameBuilder;
};
