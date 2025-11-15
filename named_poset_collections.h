/**
 * Public interface for the Named Poset Collections module.
 *
 * Authors: Oskar Rowicki, Marcin Guszczin
 * Date: 31.10.2025
 *
 * This module provides the user with the ability to:
 * - create collections identified by ID
 * - create named partially ordered sets within those collections
 * - add/remove/check relations of elements within the sets
 *
 * The user defines the element count in sets via the constant N when compiling
 * the module, default is 32.
 *
 * The module works both in C and in C++; in C++ the functions are within
 * namespace cxx.
 */

#ifndef NAMED_POSET_COLLECTIONS_H
#define NAMED_POSET_COLLECTIONS_H

#include <cstddef>

#ifdef __cplusplus
namespace cxx
{
extern "C" {
#endif

/**
 * Creates a new empty collection, returns its ID.
 */
long npc_new_collection(void);

/**
 * Deletes the collection corresponding to `id`.
 */
void npc_delete_collection(long id);

/**
 * Creates a new named poset inside the collection corresponding to `id`,
 * initializes it with {x, x} pairs for reflexivity.
 * Returns true if it was successful.
 */
bool npc_new_poset(long id, char const *name);

/**
 * Deletes the poset named `name` from the collection corresponding to `id`.
 */
void npc_delete_poset(long id, char const *name);

/**
 * Creates a copy of the poset named `name_src` within the collection
 * corresponding to `id`, names it `name_dst`.
 * Returns true if successful.
 */
bool npc_copy_poset(long id, char const *name_dst, char const *name_src);

/**
 * Returns the name of the first poset in the collection corresponding to `id`.
 * Ordering is based on ASCII values of characters.
 */
char const *npc_first_poset(long id);

/**
 * Returns the name of the next poset after the poset named `name` in the
 * collection corresponding to `id`.
 */
char const *npc_next_poset(long id, char const *name);

/**
 * Adds the relation {x, y} to the poset named `name` in the collection
 * corresponding to `id`. Updates the poset to satisfy transitivity.
 * Returns true if successful.
 */
bool npc_add_relation(long id, char const *name, size_t x, size_t y);

/**
 * Returns true if the relation {x, y} exists within the poset named `name` in
 * the collection corresponding to `id`.
 */
bool npc_is_relation(long id, char const *name, size_t x, size_t y);

/**
 * Removes the relation {x, y} from the poset named `name` in the collection
 * corresponding to `id`, only if the removal doesn't affect transitivity.
 * Returns true if successful.
 */
bool npc_remove_relation(long id, char const *name, size_t x, size_t y);

/**
 * Returns the current collection count.
 */
size_t npc_size();

/**
 * Returns the element count inside posets.
 */
size_t npc_poset_size();

/**
 * Returns the poset count inside the collection corresponding to `id`.
 */
size_t npc_collection_size(long id);

#ifdef __cplusplus
} // extern "C"
} // namespace cxx
#endif

#endif // header-guard
