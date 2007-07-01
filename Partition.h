#ifndef PARTITION_GUARD
#define PARTITION_GUARD

class ExternalTerm;

class Partition {
 public:
  Partition(int size);
  ~Partition();

  void join(int i, int j);

  int getSetCount(int minSize, int position) const;

  int getSetSize(int set, int position) const;

  int getRoot(int i) const;

  void getSetTranslators(int number,
			 vector<Exponent>& compressor,
			 vector<Exponent>& decompressor,
			 int position) const;

  bool compress(ExternalTerm& term,
		const vector<Exponent>& compressor) const;

  void print(ostream& out) const;

 private:
  int* _partitions;
  int _size;
};

#endif
