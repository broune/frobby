#ifndef PARTITION_GUARD
#define PARTITION_GUARD

class Term;

// TODO: this class needs a complete overhaul.
class Partition {
 public:
  Partition(int size);
  ~Partition();

  void join(int i, int j);

  int getSetCount(int minSize = 1, int position = 0) const;

  int getSetSize(int set, int position = 0) const;

  int getRoot(int i) const;

  void getSetTranslators(int number,
			 vector<Exponent>& compressor,
			 vector<Exponent>& decompressor,
			 int position = 0) const;

  bool compress(Term& term,
		const vector<Exponent>& compressor) const;

  void print(ostream& out) const;

  // project and inverseProject are for the slice algorithm, while
  // compress is for the old label algorithm code. They work
  // differently. The slice algorithm should really get its own
  // separate class.
  void project(Term& to, const Exponent* from,
	       const vector<Exponent>& compressor) const;
  void inverseProject(Term& to, const Exponent* from,
		      const vector<Exponent>& compressor) const;


 private:
  int* _partitions;
  int _size;
};

#endif
