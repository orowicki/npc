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

#ifndef N
#define N 32
#endif

namespace {

using collection_t =
    map<string, pair<array<bitset<N>, N>, array<bitset<N>, N>>>;
using poset_t = pair<array<bitset<N>, N>, array<bitset<N>, N>>;

unordered_map<long, collection_t> collections;
long nextID = 0;

bool collection_exists(long id) {
  return collections.find(id) != collections.end();
}

bool poset_exists(long id, string name) {
  return collections[id].find(name) != collections[id].end();
}

// implementation needed
bool name_is_valid(string name) { return true; }

} // namespace

/*
 * Collection functions
 */

long npc_new_collection(void) {
  if (nextID == LONG_MAX)
    return -1;

  long id = nextID;
  collections.emplace(id, collection_t());
  nextID++;

  return id;
}

void npc_delete_collection(long id) {
  if (collection_exists(id))
    collections.erase(id);
}

/*
 * Poset functions
 */

bool npc_new_poset(long id, const char *name) {
  string name_string = string(name);
  if (collection_exists(id) && name_is_valid(name_string) &&
      !poset_exists(id, name_string)) {
    collections[id].emplace(name_string, poset_t());
    // initialize poset_t with the x = x, y = y.. relation <- implement

    return true;
  }

  return false;
}

/*
 * Size functions
 */

size_t npc_size() { return collections.size(); }

size_t npc_poset_size() { return (size_t)N; }

size_t npc_collection_size(long id) {
  if (collection_exists(id))
    return collections[id].size();

  return 0;
}
