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

using poset_t      = array<bitset<N>, N>;
using collection_t = map<string, poset_t>;

/* SIOF workaround */
unordered_map<long, collection_t> &collections()
{
    static unordered_map<long, collection_t> collections;
    return collections;
}

/* SIOF workaround */
long &nextID()
{
    static long nextID = 0;
    return nextID;
}

bool collection_exists(const long id)
{
    return collections().find(id) != collections().end();
}

bool poset_exists(const long id, const string &name)
{
    return collections().at(id).find(name) != collections().at(id).end();
}

/**
 * Returns true if name is not empty and contains only the following characters:
 * a-z, A-Z, 0-9, _
 * Otherwise returns false.
 */
bool name_is_valid(const string &name)
{
    return (!name.empty() && all_of(name.begin(), name.end(), [](char c) {
        return (isalnum(static_cast<unsigned char>(c)) || c == '_');
    }));
}

/**
 * Initializes poset with {x, x} pairs to satisfy reflexivity.
 */
void initialize_poset(poset_t &poset)
{
    for (size_t i = 0; i < (size_t)N; ++i)
        poset[i][i] = true;
}

/**
 * Returns true if there is no element z such that {x, z} and {z, y} belong to
 * the poset. Otherwise returns false.
 */
bool relation_removal_is_valid(const long id, const string &name,
                               const size_t x, const size_t y)
{
    auto &poset = collections().at(id).at(name);
    for (size_t z = 0; z < (size_t)N; ++z) {
        if (poset[x][z] && poset[z][y] && z != x && z != y)
            return false;
    }

    return true;
}

/**
 * Checks whether the relation to be added already exists in the poset
 * or violates asymmetry.
 *
 * @return false if the relation already exists or would break asymmetry,
 *         true otherwise.
 */
bool relation_addition_is_valid(const long id, const string &name,
                                const size_t x, const size_t y)
{
    auto &poset = collections().at(id).at(name);
    return !poset[x][y] && !poset[y][x];
}

/**
 * Adds the relation {x, y} and updates the poset to satisfy transitivity.
 * Explanation:
 * Go over every element z that satisfies {z, x},
 * Use bitset OR so that z is now in relation {z, ...} with every element that y
 * is in relation {y, ...} with.
 */
void update_transitive_closure(const long id, const string &name,
                               const size_t x, const size_t y)
{
    auto &poset = collections().at(id).at(name);
    for (size_t z = 0; z < (size_t)N; ++z) {
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
    if (collection_exists(id))
        collections().erase(id);
}

bool npc_new_poset(long id, const char *name)
{
    if (!name)
        return false;
    const string name_string(name);

    if (collection_exists(id) && name_is_valid(name_string) &&
        !poset_exists(id, name_string)) {
        poset_t new_poset;
        initialize_poset(new_poset);
        collections().at(id).emplace(std::move(name_string),
                                     std::move(new_poset));
        return true;
    }

    return false;
}

void npc_delete_poset(long id, const char *name)
{
    if (!name)
        return;
    const string name_string(name);

    if (collection_exists(id) && poset_exists(id, name_string))
        collections().at(id).erase(name_string);
}

bool npc_copy_poset(long id, const char *name_dst, const char *name_src)
{
    if (!name_dst || !name_src)
        return false;

    const string name_dst_string(name_dst);
    const string name_src_string(name_src);

    if (collection_exists(id) && name_is_valid(name_dst_string) &&
        poset_exists(id, name_src_string)) {
        collections().at(id)[name_dst_string] =
            collections().at(id).at(name_src_string);
        return true;
    }

    return false;
}

char const *npc_first_poset(long id)
{
    if (collection_exists(id) && !collections().at(id).empty())
        return collections().at(id).begin()->first.c_str();

    return NULL;
}

char const *npc_next_poset(long id, char const *name)
{
    if (!name)
        return NULL;
    const string name_string(name);

    if (collection_exists(id) && poset_exists(id, name_string) &&
        next(collections().at(id).find(name_string)) !=
            collections().at(id).end())
        return next(collections().at(id).find(name_string))->first.c_str();

    return NULL;
}

bool npc_add_relation(long id, const char *name, size_t x, size_t y)
{
    if (!name)
        return false;
    const string name_string(name);

    if (collection_exists(id) && poset_exists(id, name_string) &&
        x < (size_t)N && y < (size_t)N &&
        relation_addition_is_valid(id, name_string, x, y)) {
        update_transitive_closure(id, name_string, x, y);
        return true;
    }

    return false;
}

bool npc_is_relation(long id, const char *name, size_t x, size_t y)
{
    if (!name)
        return false;
    const string name_string(name);

    if (collection_exists(id) && poset_exists(id, name_string) &&
        x < (size_t)N && y < size_t(N))
        return collections().at(id).at(name_string)[x][y];

    return false;
}

bool npc_remove_relation(long id, const char *name, size_t x, size_t y)
{
    if (!name)
        return false;
    const string name_string(name);

    if (collection_exists(id) && poset_exists(id, name_string) && x != y &&
        x < (size_t)N && y < (size_t)N &&
        relation_removal_is_valid(id, name_string, x, y) &&
        collections().at(id).at(name)[x][y]) {
        collections().at(id).at(name)[x][y] = false;
        return true;
    }

    return false;
}

size_t npc_size()
{
    return collections().size();
}

size_t npc_poset_size()
{
    return (size_t)N;
}

size_t npc_collection_size(long id)
{
    if (collection_exists(id))
        return collections().at(id).size();

    return 0;
}

} // namespace cxx
