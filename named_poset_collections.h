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

#include <stddef.h>

#ifdef __cplusplus
namespace cxx
{
extern "C" {
#endif

long npc_new_collection(void);
void npc_delete_collection(long id);

bool npc_new_poset(long id, char const *name);
void npc_delete_poset(long id, char const *name);
bool npc_copy_poset(long id, char const *name_dst, char const *name_src);

char const *npc_first_poset(long id);
char const *npc_next_poset(long id, char const *name);

bool npc_add_relation(long id, char const *name, size_t x, size_t y);
bool npc_is_relation(long id, char const *name, size_t x, size_t y);
bool npc_remove_relation(long id, char const *name, size_t x, size_t y);

size_t npc_size();
size_t npc_poset_size();
size_t npc_collection_size(long id);

#ifdef __cplusplus
} // extern "C"
} // namespace cxx
#endif

#endif // header-guard
