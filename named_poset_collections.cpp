#include "named_poset_collections.h"
#include <algorithm>
#include <array>
#include <bitset>
#include <cctype>
#include <map>
#include <string>
#include <unordered_map>

using std::all_of;
using std::array;
using std::bitset;
using std::isalnum;
using std::map, std::unordered_map;
using std::pair;
using std::string;

#ifndef N
#define N 32
#endif

namespace {

using collection_t =
    map<string, pair<array<bitset<N>, N>, array<bitset<N>, N>>>;
using poset_t = pair<array<bitset<N>, N>, array<bitset<N>, N>>;

unordered_map<long, collection_t> &collections() {
  static unordered_map<long, collection_t> collections;
  return collections;
}

long &nextID() {
  static long nextID = 0;
  return nextID;
}

bool collection_exists(const long id) {
  return collections().find(id) != collections().end();
}

bool poset_exists(const long id, const string &name) {
  return collections()[id].find(name) != collections()[id].end();
}

bool name_is_valid(const string &name) {
  return !name.empty() && all_of(name.begin(), name.end(), [](char c) {
    return (isalnum(static_cast<unsigned char>(c)) || c == '_');
  });
}

void initialize_poset(poset_t &poset) {
  for (int i = 0; i < N; i++) {
    poset.first[i][i] = true;
    poset.second[i][i] = true;
  }
}

} // namespace

/*
 * Collection functions
 */

long npc_new_collection(void) {
  if (nextID() == LONG_MAX)
    return -1;

  long id = nextID();
  collections().emplace(id, collection_t());
  nextID()++;

  return id;
}

void npc_delete_collection(long id) {
  if (collection_exists(id))
    collections().erase(id);
}

/*
 * Poset functions
 */

bool npc_new_poset(long id, const char *name) {
  const string name_string = string(name);
  if (collection_exists(id) && name_is_valid(name_string) &&
      !poset_exists(id, name_string)) {

    poset_t new_poset;
    initialize_poset(new_poset);
    collections()[id].emplace(name_string, new_poset);
    return true;
  }

  return false;
}

/*
 * Size functions
 */

size_t npc_size() { return collections().size(); }

size_t npc_poset_size() { return (size_t)N; }

size_t npc_collection_size(long id) {
  if (collection_exists(id))
    return collections()[id].size();

  return 0;
}
