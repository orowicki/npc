/**
 * Implementation of the Named Poset Collections module.
 *
 * See named_poset_collections.h for the detailed description.
 */

#include "named_poset_collections.h"

#include <algorithm>
#include <array>
#include <bitset>
#include <cctype>
#include <climits>
#include <map>
#include <string>
#include <unordered_map>

using std::all_of;
using std::array;
using std::bitset;
using std::isalnum;
using std::map, std::unordered_map;
using std::next;
using std::string;

#ifndef N
#define N 32
#endif

namespace
{

using poset_t          = array<bitset<N>, N>;
using collection_t     = map<string, poset_t>;
using collection_map_t = unordered_map<long, collection_t>;

/* SIOF workaround */
collection_map_t &collections()
{
    static collection_map_t collections;
    return collections;
}

/* SIOF workaround */
long &nextID()
{
    static long nextID = 0;
    return nextID;
}

/**
 * Returns an iterator pointing to the collection corresponding to `id`.
 */
collection_map_t::iterator find_collection(const long id)
{
    return collections().find(id);
}

/**
 * Returns an iterator pointing to the poset `name` in the collection pointed to
 * by `col_it`.
 */
collection_t::iterator find_poset(const collection_map_t::iterator col_it,
                                  const string &name)
{
    return col_it->second.find(name);
}

bool collection_exists(const collection_map_t::iterator col_it)
{
    return col_it != collections().end();
}

bool poset_exists(const collection_map_t::iterator col_it,
                  const collection_t::iterator pos_it)
{
    return pos_it != col_it->second.end();
}

/**
 * Returns true if name is not empty and contains only the following characters:
 * a-z, A-Z, 0-9, _
 * Otherwise returns false.
 */
bool name_is_valid(const string &name)
{
    return !name.empty() && all_of(name.begin(), name.end(), [](char c) {
        return (isalnum(static_cast<unsigned char>(c)) || c == '_');
    });
}

/**
 * Initializes poset with {x, x} pairs to satisfy reflexivity.
 */
void initialize_poset(poset_t &poset)
{
    for (size_t i = 0; i < N; ++i)
        poset[i][i] = true;
}

/**
 * Returns true if {x, y} doesn't belong to the poset and there is no
 * element z such that {x, z} and {z, y} belong to the poset.
 * Otherwise returns false.
 */
bool relation_removal_is_valid(const collection_t::iterator pos_it,
                               const size_t x, const size_t y)
{
    const poset_t &poset = pos_it->second;

    if (!poset[x][y])
        return false;

    for (size_t z = 0; z < N; ++z) {
        if (poset[x][z] && poset[z][y] && z != x && z != y)
            return false;
    }

    return true;
}

/**
 * Checks whether the relation to be added already exists in the poset
 * or violates asymmetry. Returns false if the relation already exists
 * or would break asymmetry, true otherwise.
 */
bool relation_addition_is_valid(const collection_t::iterator pos_it,
                                const size_t x, const size_t y)
{
    const poset_t &poset = pos_it->second;
    return !poset[x][y] && !poset[y][x];
}

/**
 * Adds the relation {x, y} and updates the poset to satisfy transitivity.
 * Explanation:
 * Go over every element z that satisfies {z, x}.
 * Use bitset OR so that z is now in relation {z, ...} with every element that
 * y is in relation {y, ...} with.
 */
void update_transitive_closure(const collection_t::iterator pos_it,
                               const size_t x, const size_t y)
{
    poset_t &poset = pos_it->second;
    for (size_t z = 0; z < N; ++z) {
        if (poset[z][x])
            poset[z] |= poset[y];
    }
}

} // namespace

namespace cxx
{

long npc_new_collection(void)
{
    if (nextID() == LONG_MAX)
        return -1;

    long id = nextID();
    collections().emplace(id, collection_t());
    ++nextID();

    return id;
}

void npc_delete_collection(long id)
{
    const auto col_it = find_collection(id);
    if (collection_exists(col_it))
        collections().erase(col_it);
}

bool npc_new_poset(long id, const char *name)
{
    if (!name)
        return false;

    const string name_string(name);
    if (!name_is_valid(name_string))
        return false;

    const auto col_it = find_collection(id);
    if (!collection_exists(col_it))
        return false;

    const auto pos_it = find_poset(col_it, name_string);
    if (poset_exists(col_it, pos_it))
        return false;

    poset_t new_poset;
    initialize_poset(new_poset);
    col_it->second.emplace(std::move(name_string), std::move(new_poset));

    return true;
}

void npc_delete_poset(long id, const char *name)
{
    if (!name)
        return;

    const string name_string(name);

    const auto col_it = find_collection(id);
    if (!collection_exists(col_it))
        return;

    const auto pos_it = find_poset(col_it, name_string);
    if (!poset_exists(col_it, pos_it))
        return;

    col_it->second.erase(pos_it);
}

bool npc_copy_poset(long id, const char *name_dst, const char *name_src)
{
    if (!name_dst || !name_src)
        return false;

    const string name_dst_string(name_dst);
    const string name_src_string(name_src);

    /**
     * Don't need to validate name_src_string, because
     * a poset with an invalid name can't exist.
     */
    if (!name_is_valid(name_dst_string))
        return false;

    const auto col_it = find_collection(id);
    if (!collection_exists(col_it))
        return false;

    const auto pos_it = find_poset(col_it, name_src_string);
    if (!poset_exists(col_it, pos_it))
        return false;

    col_it->second[name_dst_string] = pos_it->second;

    return true;
}

char const *npc_first_poset(long id)
{
    const auto col_it = find_collection(id);
    if (!collection_exists(col_it) || col_it->second.empty())
        return NULL;

    return col_it->second.begin()->first.c_str();
}

char const *npc_next_poset(long id, char const *name)
{
    if (!name)
        return NULL;

    const string name_string(name);

    const auto col_it = find_collection(id);
    if (!collection_exists(col_it))
        return NULL;

    const auto pos_it = find_poset(col_it, name_string);
    if (!poset_exists(col_it, pos_it) || next(pos_it) == col_it->second.end())
        return NULL;

    return next(pos_it)->first.c_str();
}

bool npc_add_relation(long id, const char *name, size_t x, size_t y)
{
    if (!name || x >= N || y >= N || x == y)
        return false;

    const string name_string(name);

    const auto col_it = find_collection(id);
    if (!collection_exists(col_it))
        return false;

    const auto pos_it = find_poset(col_it, name_string);
    if (!poset_exists(col_it, pos_it) ||
        !relation_addition_is_valid(pos_it, x, y))
        return false;

    update_transitive_closure(pos_it, x, y);

    return true;
}

bool npc_is_relation(long id, const char *name, size_t x, size_t y)
{
    if (!name || x >= N || y >= N)
        return false;

    const string name_string(name);

    const auto col_it = find_collection(id);
    if (!collection_exists(col_it))
        return false;

    const auto pos_it = find_poset(col_it, name_string);
    if (!poset_exists(col_it, pos_it))
        return false;

    return pos_it->second[x][y];
}

bool npc_remove_relation(long id, const char *name, size_t x, size_t y)
{
    if (!name || x >= N || y >= N || x == y)
        return false;

    const string name_string(name);

    const auto col_it = find_collection(id);
    if (!collection_exists(col_it))
        return false;

    const auto pos_it = find_poset(col_it, name_string);
    if (!poset_exists(col_it, pos_it) ||
        !relation_removal_is_valid(pos_it, x, y))
        return false;

    pos_it->second[x][y] = false;

    return true;
}

size_t npc_size()
{
    return collections().size();
}

size_t npc_poset_size()
{
    return N;
}

size_t npc_collection_size(long id)
{
    const auto col_it = find_collection(id);

    if (!collection_exists(col_it))
        return 0;

    return col_it->second.size();
}

} // namespace cxx
