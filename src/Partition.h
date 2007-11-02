#ifndef PARTITION_GUARD
#define PARTITION_GUARD

class Term;

class Partition {
 public:
  Partition(int size);
  ~Partition();

  void join(int i, int j);

  int getSetCount(int minSize = 1) const;

  int getSetSize(int set) const;

  int getRoot(int i) const;

  void getProjection(int number, vector<Exponent>& projection) const;

  void print(ostream& out) const;

  void project(Term& to, const Exponent* from,
	       const vector<Exponent>& projection) const;
  void inverseProject(Term& to, const Exponent* from,
		      const vector<Exponent>& projection) const;


 private:
  int* _partitions;
  int _size;
};

#endif
