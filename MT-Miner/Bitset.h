#pragma once
#include <boost/dynamic_bitset.hpp>
#include <cassert>

#ifdef _DEBUG
using Bitset = std::vector<bool>;
#else
using Bitset = boost::dynamic_bitset<>;
#endif
