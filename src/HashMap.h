/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
#ifndef HASH_MAP_GUARD
#define HASH_MAP_GUARD

/** \file

 The purpose of this file is to provide a definition of a hash map
 whenever possible. This needs to be done separately since various
 compilers put their hash maps in different places. GCC even moves it
 around for different versions and issues warnings for some
 placements. This has been fixed by including GCCs hash_map with
 Frobby to get a consistent and warning-free place to get it from.

 If the current compiler is not recognized, we use a comparison-based
 map as a fall-back.
*/

template<class Key>
class FrobbyHash {};

// *********************************************************
#ifdef __GNUC__ // Only GCC defines this macro
#include "hash_map/hash_map"
#include <string>

template<>
class FrobbyHash<string> : public __gnu_cxx::hash<string> {
};

template<class Key, class Value>
class HashMap : public __gnu_cxx::hash_map<Key, Value,
  FrobbyHash<Key> > {
};

#else

// *********************************************************
#ifdef _MSC_VER // Only Microsoft C++ defines this macro
#include <hash_map>
#include <string>

template<class Key>
class HashWrapper : public stdext::hash_compare<Key, ::std::less<Key> >, FrobbyHash<Key> {
public:
  size_t operator()(const Key& key) const {
    return FrobbyHash<typename Key>::operator()(key);
  }

  bool operator()(const Key& a, const Key& b) const {
    return stdext::hash_compare<Key, ::std::less<Key> >::operator()(a, b);
  }
};

template<>
class HashWrapper<string> : public stdext::hash_compare<string, ::std::less<string> > {
};

template<class Key, class Value>
class HashMap : public stdext::hash_map<Key, Value, HashWrapper<Key> > {
};

// *********************************************************
#else // Fall-back for unknown compilers
#include <map>
template<class Key, class Value>
class HashMap : public std::map<Key, Value> {
};
#endif
#endif


#endif
