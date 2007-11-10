#include "stdinc.h"
#include "VarNames.h"

#include <sstream>

const int VarNames::UNKNOWN = -1;

VarNames::VarNames() {
}

VarNames::VarNames(size_t varCount) {
  for (size_t i = 0; i < varCount; ++i) {
    ostringstream out;
    out << 'x' << (i + 1);
    addVar(out.str());
  }    
}

void VarNames::addVar(const string& name) {
  ASSERT(name != "");
  ASSERT(!contains(name));
  
  _nameToIndex[name] = _indexToName.size();
  _indexToName.push_back(name);
}

int VarNames::getIndex(const string& name) const {
  map<string, int>::const_iterator it = _nameToIndex.find(name);
  if (it == _nameToIndex.end())
    return -1;
  else
    return it->second;
}

bool VarNames::contains(const string& name) const {
  return getIndex(name) != UNKNOWN;
}

const string& VarNames::getName(int index) const {
  ASSERT(0 <= index &&
	 (size_t)index < _indexToName.size());
  
  return _indexToName[index];
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
