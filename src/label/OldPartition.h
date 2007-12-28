#ifndef OLD_PARTITION_GUARD
#define OLD_PARTITION_GUARD

#include <vector>
class Term;

class OldPartition {
 public:
  OldPartition(int size);
  ~OldPartition();

  void join(int i, int j);

  int getSetCount(int minSize = 1, int position = 0) const;

  int getSetSize(int set, int position = 0) const;

  int getRoot(int i) const;

  void getSetTranslators(int number,
			 vector<Exponent>& compressor,
			 int position = 0) const;

  bool compress(Term& term,
		const vector<Exponent>& compressor) const;

 private:
  int* _partitions;
  int _size;
};

#endif
