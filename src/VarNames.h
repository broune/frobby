#ifndef VAR_NAMES_GUARD
#define VAR_NAMES_GUARD

#include <map>

class VarNames {
public:
  VarNames();
  VarNames(size_t varCount);

  void addVar(const string& name);

  // Returns VarNames::UNKNOWN if name is not known.
  size_t getIndex(const string& name) const;

  bool contains(const string& name) const;

  // The returned reference can become invalid next time addVar is
  // called.
  const string& getName(size_t index) const;

  size_t getVarCount() const;

  void clear();

  bool empty() const;

  friend ostream& operator<<(ostream& out, const VarNames& names) {
    out << "VarNames(";
    const char* prefix = "";
    for (size_t i = 0; i < names.getVarCount(); ++i) {
      out << prefix << (i + 1) << "<->\"" << names.getName(i) << '"';
      prefix = ", ";
    }
    out << ")";

    return out;
  }

  static const size_t UNKNOWN;

  bool operator==(const VarNames& names) const {
    return _indexToName == names._indexToName;
  }

private:

  map<string, size_t> _nameToIndex;
  vector<string> _indexToName;
};

#endif
