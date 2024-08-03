#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <array>

struct Square
{
  int file;
  int rank;

  bool operator==(const Square &sqr) const
  {
    return file == sqr.file && rank == sqr.rank;
  }
};

int algebraic_to_index(const std::string &);

std::string index_to_algebraic(int);

std::array<char, 64> piece_placement_string_to_array(const std::string &);

std::string piece_placement_array_to_string(const std::array<char, 64> &);

Square index_to_file_rank(int);

#endif