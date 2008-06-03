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

#include <sstream>

const size_t VarNames::UNKNOWN = numeric_limits<size_t>::max();

VarNames::VarNames() {
}

VarNames::VarNames(size_t varCount) {
  for (size_t i = 0; i < varCount; ++i) {
    ostringstream out;
    out << 'x' << (i + 1);
    addVar(out.str());
  }    
}

VarNames::VarNames(const VarNames& names) {
  for (size_t var = 0; var < names.getVarCount(); ++var)
	addVar(names.getName(var));
}

VarNames::~VarNames() {
  clear();
}

void VarNames::addVar(const string& name) {
  ASSERT(name != "");
  ASSERT(!contains(name));

  _indexToName.push_back(new string(name));
  char* str = new char[name.size() + 1];
  strcpy(str, name.c_str());
  _nameToIndex[str] = _indexToName.size() - 1;

  if (getVarCount() == UNKNOWN) {
    fputs("ERROR: Too many variables names.\n", stderr);
    exit(1);
  }
}

size_t VarNames::getIndex(const string& name) const {
  return getIndex(name.c_str());
}

size_t VarNames::getIndex(const char* name) const {
  VarNameMap::const_iterator it = _nameToIndex.find(name);
  if (it == _nameToIndex.end())
    return UNKNOWN;
  else	
    return it->second;
}

bool VarNames::contains(const string& name) const {
  return getIndex(name) != UNKNOWN;
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
  vector<const char*> ptrs;
  for (VarNameMap::const_iterator it = _nameToIndex.begin();
	   it != _nameToIndex.end(); ++it)
	ptrs.push_back(it->first);
  _nameToIndex.clear();
  
  for (size_t i = 0; i < ptrs.size(); ++i)
	delete[] ptrs[i];
  
  for (size_t i = 0; i < _indexToName.size(); ++i)
	delete _indexToName[i];
  _indexToName.clear();
}

bool VarNames::empty() const {
  return _indexToName.empty();
}


VarNames& VarNames::operator=(const VarNames& names) {
  clear();
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

void VarNames::print(FILE* file) const {
  fputs("VarNames(", file);
  for (size_t i = 0; i < getVarCount(); ++i) {
    if (i != 0)
      fputs(", ", file);
    fprintf(file, "%lu<->\"%s\"", (unsigned long)i, getName(i).c_str());
  }
  fputs(")\n", file);
}
