#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <array>

int square_to_index(const std::string& square);

std::string index_to_square(int index);

std::array<char, 64> piece_placement_string_to_array(const std::string &);

#endif