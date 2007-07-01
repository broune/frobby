#ifndef VAR_NAMES_GUARD
#define VAR_NAMES_GUARD

#include <sstream>

#include <map>

class VarNames {
public:
  VarNames() {
  }

  VarNames(size_t varCount) {
    for (size_t i = 0; i < varCount; ++i) {
      ostringstream out;
      out << 'x' << (i + 1);
      addVar(out.str());
    }    
  }

  void addVar(const string& name) {
    ASSERT(name != "");
    ASSERT(!contains(name));

    _nameToIndex[name] = _indexToName.size();
    _indexToName.push_back(name);
  }

  // Returns VarNames::UNKNOWN if name is not known.
  int getIndex(const string& name) const {
    map<string, int>::const_iterator it =
      _nameToIndex.find(name);
    if (it == _nameToIndex.end())
      return -1;
    else
      return it->second;
  }

  // The returned reference can become invalid next time addVar is
  // called.
  const string& getName(int index) const {
    ASSERT(0 <= index &&
	   (size_t)index < _indexToName.size());

    return _indexToName[index];
  }

  size_t getVarCount() const {
    return _indexToName.size();
  }

  void clear() {
    _nameToIndex.clear();
    _indexToName.clear();
  }

  bool empty() const {
    return _indexToName.empty();
  }

  friend ostream& operator<<(ostream& out, const VarNames& names) {
    out << "VarNames(";
    const char* prefix = "";
    for (unsigned int i = 0; i < names.getVarCount(); ++i) {
      out << prefix << (i + 1) << "<->\"" << names.getName(i) << '"';
      prefix = ", ";
    }
    out << ")";

    return out;
  }

  static const int UNKNOWN = -1;

private:
  bool contains(const string& name) const {
    return _nameToIndex.find(name) != _nameToIndex.end();
  }

  map<string, int> _nameToIndex;
  vector<string> _indexToName;
};

#endif
