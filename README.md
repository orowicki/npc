# Named Poset Collections

Authors: Oskar Rowicki, Marcin Guszczin

## Overview
 
This module provides the user with the ability to:
- create collections identified by ID
- create named partially ordered sets within those collections
- add/remove/check relations of elements within the sets

The user defines the element count in sets via the constant N when compiling
the module, default is 32.
 
The module works both in C and in C++; in C++ the functions are within
namespace cxx.

For detailed api description see the [header](named_poset_collections.h).
