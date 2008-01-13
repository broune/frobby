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

  void addVar(const string& name);

  // Returns VarNames::UNKNOWN if name is not known.
  size_t getIndex(const string& name) const;
  size_t getIndex(const char* name) const;

  bool contains(const string& name) const;

  // The returned reference can become invalid next time addVar is
  // called.
  const string& getName(size_t index) const;

  size_t getVarCount() const;

  void clear();

  bool empty() const;

  static const size_t UNKNOWN;

  bool operator==(const VarNames& names) const;

  void print(FILE* file) const;

private:
  VarNameMap _nameToIndex;
  vector<string*> _indexToName;
};

#endif
