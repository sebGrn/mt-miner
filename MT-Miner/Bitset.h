#pragma once

#include <cassert>

#ifdef _DEBUG
// use vector of bool to allow vector debugging
using Bitset = std::vector<bool>;
#else
// use dynamic bitset for speedup
using Bitset = boost::dynamic_bitset<>;
#endif
