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

class Scanner;

// Use the GCC-specific hash_map class if compiling with GCC, and
// otherwise use a std::map, which is present in all compilers.
/** @todo It seems that Frobby comes with its own copy of
	hash_map. Always use that one here, or find out why it isn't used.
 */
#ifdef __GNUC__ // Only GCC defines this macro
#include "hash_map/hash_map"
class StringEquals {
 public:
  bool operator()(const char* a, const char* b) const {
	return strcmp(a, b) == 0;
  }
};
typedef __gnu_cxx::hash_map<string, size_t,
							__gnu_cxx::hash<string> > VarNameMap;
#else
#include <map>
class StringLessThan {
 public:
  bool operator()(const char* a, const char* b) const {
	return strcmp(a, b) < 0;
  }
};
typedef map<string, size_t> VarNameMap;
#endif

/** Defines the variables of a polynomial ring and facilities IO
	involving them.

	@todo Rename to Ring or a similar name.

	@todo make this class immutable and make a VarNamesBuilder.
	
	@todo make copies share the same memory and use reference counting
	to deallocate the shared memory.
	
	@todo make equality check for the same memory and thus return true very
	quickly if that is the case.
*/
class VarNames {
public:
  VarNames();
  VarNames(size_t varCount);
  VarNames(const VarNames& names);
  ~VarNames();

  /** Adds the variable and returns true if name is not already a variable.
	  Otherwise it returns false without adding the variable (again).
  */
  bool addVar(const string& name);

  /** As addvar, except it reports a syntax error if name is already a
	  variable.

	  @todo Move this somewhere more appropriate.
  */
  void addVarSyntaxCheckUnique(const Scanner& in, const string& name);

  /** This also depends on the order of the names. */
  bool operator<(const VarNames& names) const;

  /** Returns VarNames::getInvalidIndex() if name is not known. */
  size_t getIndex(const string& name) const;

  /** Returns true if name is the name of a variable. */
  bool contains(const string& name) const;

  /** Returns true if the names are x1, x2 and so on. */
  bool namesAreDefault() const;

  /** The returned reference can become invalid next time addVar is
	  called.
  */
  const string& getName(size_t index) const;
  
  /** Returns the current number of variables. */
  size_t getVarCount() const;

  /** Resets the number of variables to zero. */
  void clear();

  /** Returns true if the number of variables is zero. */
  bool empty() const;

  VarNames& operator=(const VarNames& names);
  bool operator==(const VarNames& names) const;
  bool operator!=(const VarNames& names) const;

  /** Swaps the variables with indexes a and b. */
  void swapVariables(size_t a, size_t b);

  void toString(string& str) const;
  void print(FILE* file) const; // For debug

  /** Returns a fixed variable offset that is always invalid. */
  static size_t getInvalidIndex();

private:
  static bool compareNames(const string* a, const string* b);

  VarNameMap _nameToIndex;
  vector<const string*> _indexToName;
};

#endif
