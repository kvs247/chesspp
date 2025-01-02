#pragma once

#include <fstream>
#include <stddef.h>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef OUT_PGN_FILE_PATH
#define OUT_PGN_FILE_PATH "out/game.pgn"
#endif

void writePgn(const std::vector<std::string> &moveList)
{
  const std::string filePath = OUT_PGN_FILE_PATH;
  std::ofstream f(filePath, std::ios::out | std::ios::trunc);
  if (!f.is_open())
  {
    throw std::runtime_error("Error opening " + filePath);
  }

  size_t count = 1;
  for (size_t i = 0; i < moveList.size(); i += 2)
  {
    f << count << ". " << moveList[i];
    if (i + 1 < moveList.size())
    {
      f << " " << moveList[i + 1];
    }
    f << " ";
    ++count;
  }

  f.close();
}