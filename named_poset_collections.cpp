/* 
 * notes:
 * 
 * for collections most likely use:
 * using collection_t =
 * unordered_map<long, map<string, pair<array<bitset<N>, N>, array<bitset<N>, N>>>>
 *
 *
 * figure out static global init stuff to avoid init order problem 
 * (program has to work when linked with .o files swapped around)
 *
 * namespace cxx
 *
 * dont hold user's `char const *` -> convert them to cpp strings, to prevent
 * user's access to the collection's contents.
 *
*/


#include "named_poset_collections.h"

