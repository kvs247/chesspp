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
  void initScreen();
  void cleanupScreen();
  void draw(const Game &);

  Renderer() = delete;
  Renderer(Game &);
  ~Renderer();

private:
  Game &game;
  std::unique_ptr<FrameBuilder> frameBuilder;

  void render(const std::vector<std::string> &);
  winsize getWindowDimensions();
};
