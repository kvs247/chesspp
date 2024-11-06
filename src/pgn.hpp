#include <exception>
#include <fstream>
#include <string>
#include <vector>

void writePgn(const std::vector<std::string> &moveList)
{
  const std::string filePath = "out/game.pgn";
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