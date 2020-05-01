#pragma once
#include <boost/dynamic_bitset.hpp>
#include <cassert>

#ifdef _DEBUG
// use vector of bool to allow vector debugging
typedef std::vector<bool> Bitset;
#else
// use dynamic bitset for speedup
typedef boost::dynamic_bitset<> Bitset;
#endif
