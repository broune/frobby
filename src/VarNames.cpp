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

void VarNames::addVar(const string& name) {
  ASSERT(name != "");
  ASSERT(!contains(name));
 
  _indexToName.push_back(new string(name));
  _nameToIndex[_indexToName.back()->c_str()] = _indexToName.size() - 1;

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

const string& VarNames::getName(size_t index) const {
  ASSERT(index < _indexToName.size());
  
  return *(_indexToName[index]);
}

size_t VarNames::getVarCount() const {
  return _indexToName.size();
}

void VarNames::clear() {
  _nameToIndex.clear();
  for (size_t i = 0; i < _indexToName.size(); ++i)
	delete _indexToName[i];
  _indexToName.clear();
}

bool VarNames::empty() const {
  return _indexToName.empty();
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
  fputc(')', file);
}
