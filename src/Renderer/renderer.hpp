#pragma once

#include <memory>
#include <stddef.h>
#include <string>
#include <sys/ioctl.h>
#include <vector>

#include "../game.hpp"
#include "../timeControl.hpp"
#include "../types.hpp"
#include "frameBuilder.hpp"

class Renderer
{
public:
  void initScreen();
  void cleanupScreen();
  void draw(const Game &);

  Renderer();
  ~Renderer();

private:
  std::unique_ptr<FrameBuilder> frameBuilder;

  void render(const std::vector<std::string> &);
  winsize getWindowDimensions();
};
