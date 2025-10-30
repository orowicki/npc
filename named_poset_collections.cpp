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
using col_iter_t   = collection_t::iterator;
using umap_iter_t  = unordered_map<long, collection_t>::iterator;

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

bool collection_exists(const long id, umap_iter_t &umap_i)
{
    umap_i = collections().find(id);
    return umap_i != collections().end();
}

bool poset_exists(const long id, const string &name, col_iter_t &col_i)
{   
    umap_iter_t umap_i;
    if (!collection_exists(id, umap_i))    
        return false;
    
    collection_t &col = umap_i->second;
    return (col_i = col.find(name)) != col.end();
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
    for (size_t i = 0; i < N; ++i)
        poset[i][i] = true;
}

/**
 * Returns true if there is no element z such that {x, z} and {z, y} belong to
 * the poset. Otherwise returns false.
 */
bool relation_removal_is_valid(const size_t x, const size_t y, poset_t &poset)
{
    for (size_t z = 0; z < (size_t)N; ++z) {
        if (poset[x][z] && poset[z][y] && z != x && z != y)
            return false;
    }

    return poset[x][y];
}

/**
 * Checks whether the relation to be added already exists in the poset
 * or violates asymmetry.
 *
 * @return false if the relation already exists or would break asymmetry,
 *         true otherwise.
 */
bool relation_addition_is_valid(const size_t x, const size_t y, poset_t &poset)
{
    return !poset[x][y] && !poset[y][x];
}

/**
 * Adds the relation {x, y} and updates the poset to satisfy transitivity.
 * Explanation:
 * Go over every element z that satisfies {z, x},
 * Use bitset OR so that z is now in relation {z, ...} with every element that y
 * is in relation {y, ...} with.
 */
void update_transitive_closure(const size_t x, const size_t y, poset_t &poset)
{
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
    umap_iter_t iter;
    if (collection_exists(id, iter))
        collections().erase(iter);
}

bool npc_new_poset(long id, const char *name)
{
    if (!name)
        return false;

    const string name_string(name);
    if (!name_is_valid(name_string))
        return false;

    if (!name_is_valid(name_string))
        return false;
    
    umap_iter_t umap_i;
    if (!collection_exists(id, umap_i)) 
        return false;

    collection_t &col = umap_i->second;
    if (col.find(name) == col.end()) {
        poset_t new_poset;
        initialize_poset(new_poset);
        col.emplace(std::move(name_string), std::move(new_poset));
        return true;
    }

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

    umap_iter_t umap_i;
    if (!collection_exists(id, umap_i)) 
        return;
    
    col_iter_t collection_i = umap_i->second.find(name);
    if (collection_i != umap_i->second.end())
        umap_i->second.erase(collection_i);
}

bool npc_copy_poset(long id, const char *name_dst, const char *name_src)
{
    if (!name_dst || !name_src)
        return false;

    const string name_dst_string(name_dst);
    const string name_src_string(name_src);
    if (!name_is_valid(name_dst_string))
        return false;

    /**
    * Checks only the name of the destination string.
    * There's no need to check the source string because
    * a poset with that name can never be added.
    */
    if (!name_is_valid(name_dst_string))
        return false;

    umap_iter_t umap_i;
    if (!collection_exists(id, umap_i))
        return false;

    collection_t &col = umap_i->second;
    col_iter_t const col_i = col.find(name_src_string);

    if (col_i != col.end()) {
        col[name_dst_string] = col_i->second;
        return true;
    }

    col_it->second[name_dst_string] = pos_it->second;

    return true;
}

char const *npc_first_poset(long id)
{
    umap_iter_t umap_i;
    if (!collection_exists(id, umap_i)) 
        return NULL;
    
    collection_t &c = umap_i->second;
    if (!c.empty())
        return c.begin()->first.c_str();

    return col_it->second.begin()->first.c_str();
}

char const *npc_next_poset(long id, char const *name)
{
    if (!name)
        return NULL;

    const string name_string(name);

    col_iter_t iter;
    if (!poset_exists(id, name_string, iter)) 
        return NULL;
        
    if (next(iter) != collections().at(id).end())
        return next(iter)->first.c_str();

    return next(pos_it)->first.c_str();
}

bool npc_add_relation(long id, const char *name, size_t x, size_t y)
{
    if (!name || x >= N || y >= N)
        return false;
     const string name_string(name);

    col_iter_t iter;
    if (!poset_exists(id, name_string, iter))
        return false;

    poset_t &poset = iter->second;
    if (relation_addition_is_valid(x, y, poset)) {
        update_transitive_closure(x, y, poset);
        return true;
    }

    auto pos_it = find_poset(col_it, name_string);
    if (!poset_exists(col_it, pos_it))
        return false;

    if (!relation_addition_is_valid(pos_it, x, y))
        return false;

    update_transitive_closure(pos_it, x, y);

    return true;
}

bool npc_is_relation(long id, const char *name, size_t x, size_t y)
{
    if (!name || x >= N || y >= N)
        return false;

    const string name_string(name);

    col_iter_t iter;
    if (poset_exists(id, name_string, iter))
        return iter->second[x][y];

    return pos_it->second[x][y];
}

bool npc_remove_relation(long id, const char *name, size_t x, size_t y)
{
    if (!name || x == y || x >= N || y >= N)
        return false;

    const string name_string(name);

    col_iter_t iter;
    if (!poset_exists(id, name_string, iter))
        return false;
    
    poset_t &poset = iter->second;
    if (relation_removal_is_valid(x, y, poset)) {
        poset[x][y] = false;
        return true;
    }

    auto pos_it = find_poset(col_it, name_string);
    if (!poset_exists(col_it, pos_it))
        return false;

    if (!relation_removal_is_valid(pos_it, x, y))
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
    umap_iter_t umap_i;
    if (collection_exists(id, umap_i))
        return umap_i->second.size();

    return col_it->second.size();
}

} // namespace cxx
