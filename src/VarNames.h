/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

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
#ifndef VAR_NAMES_GUARD
#define VAR_NAMES_GUARD

#include <vector>
#include <string>

// Use the GCC-provided hash_map class if compiling with GCC, and
// otherwise use a std::map, which is present in all compilers.
#ifdef __GNUC__ // Only GCC defines this macro
#include <ext/hash_map>
class StringEquals {
 public:
  bool operator()(const char* a, const char* b) const {
	return strcmp(a, b) == 0;
  }
};
typedef __gnu_cxx::hash_map<const char*, size_t,
							__gnu_cxx::hash<const char*>,
							StringEquals> VarNameMap;
#else
#include <map>
class StringLessThan {
 public:
  bool operator()(const char* a, const char* b) const {
	return strcmp(a, b) < 0;
  }
};
typedef map<const char*, size_t, StringLessThan> VarNameMap;
#endif


class VarNames {
public:
  VarNames();
  VarNames(size_t varCount);
  VarNames(const VarNames& names);
  ~VarNames();

  bool addVar(const string& name);

  // This also depends on the order of the names.
  bool operator<(const VarNames& names) const;

  // Returns VarNames::UNKNOWN if name is not known.
  size_t getIndex(const string& name) const;
  size_t getIndex(const char* name) const;

  bool contains(const string& name) const;

  // Returns true if the names are x1, x2 and so on.
  bool namesAreDefault() const;

  // The returned reference can become invalid next time addVar is
  // called.
  const string& getName(size_t index) const;
  
  size_t getVarCount() const;

  void clear();

  bool empty() const;

  static const size_t UNKNOWN;

  VarNames& operator=(const VarNames& names);
  bool operator==(const VarNames& names) const;

  void swapVariables(size_t a, size_t b);

  void print(FILE* file) const;

private:
  static bool compareNames(const string* a, const string* b);

  VarNameMap _nameToIndex;
  vector<string*> _indexToName;
};

#endif
