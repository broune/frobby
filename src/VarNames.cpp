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
#include "stdinc.h"
#include "VarNames.h"

#include "error.h"
#include "FrobbyStringStream.h"

#include <limits>

VarNames::VarNames() {
}

VarNames::VarNames(size_t varCount) {
  for (size_t i = 0; i < varCount; ++i) {
    FrobbyStringStream out;
    out << 'x' << (i + 1);
    addVar(out);
  }    
}

VarNames::VarNames(const VarNames& names) {
  *this = names;
}

VarNames::~VarNames() {
  clear();
}

bool VarNames::addVar(const string& name) {
  ASSERT(name != "");

  pair<VarNameMap::iterator, bool> p =_nameToIndex.insert
	(make_pair(name, _indexToName.size()));
  if (!p.second)
	return false;

  try {
	_indexToName.push_back(&(p.first->first)); // TODO: fix to vector of iters
  } catch (...) {
	_nameToIndex.erase(p.first);
	throw;
  }

  if (getVarCount() == getInvalidIndex())
	reportError("Too many variable names");

  return true;
}

bool VarNames::operator<(const VarNames& names) const {
  return lexicographical_compare(_indexToName.begin(),
								 _indexToName.end(),
								 names._indexToName.begin(),
								 names._indexToName.end(),
								 compareNames);
}

size_t VarNames::getIndex(const string& name) const {
  VarNameMap::const_iterator it = _nameToIndex.find(name);
  if (it == _nameToIndex.end())
    return getInvalidIndex();
  else	
    return it->second;
}

bool VarNames::contains(const string& name) const {
  return getIndex(name) != getInvalidIndex();
}

bool VarNames::namesAreDefault() const {
  VarNames names(getVarCount());
  return *this == names;
}

const string& VarNames::getName(size_t index) const {
  ASSERT(index < _indexToName.size());
  
  return *(_indexToName[index]);
}

size_t VarNames::getVarCount() const {
  return _indexToName.size();
}

void VarNames::clear() {
  _nameToIndex.clear();
  _indexToName.clear();
}

bool VarNames::empty() const {
  return _indexToName.empty();
}


VarNames& VarNames::operator=(const VarNames& names) {
  clear();

  _indexToName.reserve(names.getVarCount());

  for (size_t var = 0; var < names.getVarCount(); ++var)
	addVar(names.getName(var));
  return *this;
}

bool VarNames::operator==(const VarNames& names) const {
  if (getVarCount() != names.getVarCount())
	return false;

  for (size_t var = 0; var < getVarCount(); ++var)
	if (getName(var) != names.getName(var))
	  return false;

  return true;
}

void VarNames::swapVariables(size_t a, size_t b) {
  ASSERT(a < getVarCount());
  ASSERT(b < getVarCount());
  
  ASSERT(_nameToIndex[*_indexToName[a]] == a);
  ASSERT(_nameToIndex[*_indexToName[b]] == b);

  if (a == b)
	return;

  std::swap(_indexToName[a], _indexToName[b]);
  _nameToIndex[*_indexToName[a]] = a;
  _nameToIndex[*_indexToName[b]] = b;

  ASSERT(_nameToIndex[*_indexToName[a]] == a);
  ASSERT(_nameToIndex[*_indexToName[b]] == b);
}

void VarNames::toString(string& str) const {
  str.clear();
  for (size_t i = 0; i < getVarCount(); ++i) {
    if (i != 0)
	  str += ", ";
	str += getName(i);
  }
}

void VarNames::print(FILE* file) const {
  fputs("VarNames(", file);
  for (size_t i = 0; i < getVarCount(); ++i) {
    if (i != 0)
      fputs(", ", file);
    fprintf(file, "%lu<->\"%s\"", (unsigned long)i, getName(i).c_str());
  }
  fputs(")\n", file);
}

size_t VarNames::getInvalidIndex() {
  return numeric_limits<size_t>::max();
}

bool VarNames::compareNames(const string* a, const string* b) {
  return *a < *b;
}
