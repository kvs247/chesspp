#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>

#include "../config.hpp"
#include "../game.hpp"
#include "../logger.hpp"
#include "../types.hpp"
#include "../utils.hpp"
#include "frameBuilder.hpp"
#include "renderer.hpp"

Renderer::Renderer(Game &g) : game(g)
{
  initScreen();
  frameBuilder = std::make_unique<FrameBuilder>(game);
}

Renderer::~Renderer() { cleanupScreen(); }

void Renderer::initScreen()
{
  // enter alternate screen buffer (for full-screen terminal applications)
  std::cout << "\033[?1049h";
  // clear terminal
  std::cout << "\033[2J";
  // hide cursor
  std::cout << "\033[?25l";
}

void Renderer::cleanupScreen()
{
  const auto window = frameBuilder->getWindowDimensions();

  // move cursor to bottom of content
  std::cout << "\033[" << (window.ws_row + 1) << ";1H";
  // show cursor
  std::cout << "\033[?25h";

  std::cout << std::flush;
}

void Renderer::renderFrame()
{
  const auto outputLines = frameBuilder->buildFrame();

  std::stringstream frame;

  frame << "\033[H"; // reset cursor

  // position and write each line
  for (size_t i = 0; i < outputLines.size(); ++i)
  {
    frame << "\033[" << (i + 1) << ";1H";
    frame << outputLines[i] << "\033[K";
  }

  std::cout << frame.str() << std::flush;
}