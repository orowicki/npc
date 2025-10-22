#include "named_poset_collections.h"
#include <bitset>
#include <map>
#include <string>
#include <unordered_map>

using std::array;
using std::bitset;
using std::map, std::unordered_map;
using std::pair;
using std::string;

constexpr int N = 32;

using collection_t =
    unordered_map<long,
                  map<string, pair<array<bitset<N>, N>, array<bitset<N>, N>>>>;
