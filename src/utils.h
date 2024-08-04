#ifndef UTILS_H
#define UTILS_H

#include <array>
#include <string>

struct FileRank
{
  int file;
  int rank;

  bool operator==(const FileRank &sqr) const
  {
    return file == sqr.file && rank == sqr.rank;
  }
};

bool is_chess_piece(const char);

char piece_color(const char);

int algebraic_to_index(const std::string &);

std::string index_to_algebraic(int);

std::array<char, 64> piece_placement_string_to_array(const std::string &);

std::string piece_placement_array_to_string(const std::array<char, 64> &);

FileRank index_to_file_rank(int);

int file_rank_to_index(FileRank);

#endif