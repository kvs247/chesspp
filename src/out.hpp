#include <exception>
#include <fstream>
#include <string>
#include <vector>

void writeMoveList(std::vector<std::string> &moveList)
{
  std::string filePath = "out/moveList.txt";
  std::fstream f(filePath);
  if (!f.is_open())
  {
    throw std::runtime_error("Error opening " + filePath);
  }

  for (auto &s : moveList)
  {
    f << s << "\n";
  }
  f.close();
}