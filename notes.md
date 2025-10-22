# Notes

```
using collection_t = unordered_map<long, map<string, 
   pair<array<bitset<N>, N>, array<bitset<N>, N>>>>
```
##### example usage
```
long id, string name, size_t i, size_t j
collection_t[id][name].first[i][j] = true;
collection_t[id][name].second[i][j] = false;
```

explanation:
- id = a collections id (long 0 til long_max)
- name = a poset's inside that collection's name (string)
- first: only holds immediate relations, ex. adding $a\leq b$ and $b\leq c$, doesnt add $a\leq c$
- second: holds every relation that occurs based on transitivity, so if $a\leq b$ and $b\leq c$ in first then second has to contain `a<c`
- \[ i ]\[ j ] = true -> $i \leq j$

#### misc

dont hold user's `char const *` -> convert them to cpp strings, to prevent
user's access to the collection's contents.

namespace cxx

static global init problem (program has to work when linked with .o files swapped around)
