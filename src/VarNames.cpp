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

void VarNames::addVar(const string& name) {
  ASSERT(name != "");
  ASSERT(!contains(name));
 
  _nameToIndex[name] = _indexToName.size();
  _indexToName.push_back(name);

  if (getVarCount() == UNKNOWN) {
    cerr << "ERROR: Too many variables names." << endl;
    exit(1);
  }
}

size_t VarNames::getIndex(const string& name) const {
  map<string, size_t>::const_iterator it = _nameToIndex.find(name);
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
