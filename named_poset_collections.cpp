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
using std::next;
using std::pair;
using std::string;

#ifndef N
#define N 32
#endif

namespace {

using poset_t = array<bitset<N>, N>;
using collection_t = map<string, poset_t>;

// avoids static init order problem
unordered_map<long, collection_t> &collections() {
  static unordered_map<long, collection_t> collections;
  return collections;
}

// avoids static init order problem
long &nextID() {
  static long nextID = 0;
  return nextID;
}

bool collection_exists(const long id) {
  return collections().find(id) != collections().end();
}

bool poset_exists(const long id, const string &name) {
  return collections().at(id).find(name) != collections().at(id).end();
}
// verifies that name contains only a-z, A-Z, 0-9, _
bool name_is_valid(const string &name) {
  return !name.empty() && all_of(name.begin(), name.end(), [](char c) {
    return (isalnum(static_cast<unsigned char>(c)) || c == '_');
  });
}
// adds {x, x} pairs to the poset's relation
void initialize_poset(poset_t &poset) {
  for (int i = 0; i < N; i++) {
    poset[i][i] = true;
  }
}

} // namespace

/**
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

/**
 * Poset functions
 */

bool npc_new_poset(long id, const char *name) {
  const string name_string = string(name);
  if (collection_exists(id) && name_is_valid(name_string) &&
      !poset_exists(id, name_string)) {

    poset_t new_poset;
    initialize_poset(new_poset);
    collections().at(id).emplace(std::move(name_string), std::move(new_poset));
    return true;
  }

  return false;
}

void npc_delete_poset(long id, const char *name) {
  const string name_string = string(name);
  if (collection_exists(id) && poset_exists(id, name_string)) {
    collections().at(id).erase(name_string);
  }
}

bool npc_copy_poset(long id, const char *name_dst, const char *name_src) {
  const string name_dst_string(name_dst);
  const string name_src_string(name_src);

  if (collection_exists(id) && name_is_valid(name_dst_string) &&
      poset_exists(id, name_src_string)) {
    collections().at(id).emplace(std::move(name_dst_string),
                                 collections().at(id).at(name_src_string));
    return true;
  }
  return false;
}

char const *npc_first_poset(long id) {
  if (collection_exists(id) && !collections().at(id).empty())
    return collections().at(id).begin()->first.c_str();

  return NULL;
}

char const *npc_next_poset(long id, char const *name) {
  string name_string(name);
  if (collection_exists(id) && poset_exists(id, name_string) &&
      next(collections().at(id).find(name_string)) !=
          collections().at(id).end()) {

    return next(collections().at(id).find(name_string))->first.c_str();
  }

  return NULL;
}

/**
 * Relation functions
 */

bool npc_is_relation(long id, const char *name, size_t x, size_t y) {
  string name_string = string(name);
  if (collection_exists(id) && poset_exists(id, name_string) && x < (size_t)N &&
      y < size_t(N)) {
    return collections().at(id).at(name_string)[x][y];
  }
  return false;
}

/**
 * Size functions
 */

size_t npc_size() { return collections().size(); }

size_t npc_poset_size() { return (size_t)N; }

size_t npc_collection_size(long id) {
  if (collection_exists(id))
    return collections().at(id).size();

  return 0;
}
