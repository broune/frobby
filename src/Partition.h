#ifndef PARTITION_GUARD
#define PARTITION_GUARD

class Term;

class Partition {
 public:
  Partition();
  ~Partition();

  void reset(size_t size);

  void join(size_t i, size_t j);

  size_t getSize() const;

  size_t getSetCount(size_t minSize = 1) const;

  size_t getSetSize(size_t set) const;

  size_t getRoot(size_t i) const;

  size_t getSizeOfClassOf(size_t i) const;

  void print(ostream& out) const;

 private:
  int* _partitions;
  size_t _size;
  size_t _capacity;
};

#endif
