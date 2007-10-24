#ifndef ALLOCATOR_GUARD
#define ALLOCATOR_GUARD

template<class T>
class Allocator {
public:
  Allocator():
    _buffers(),
    _buffer(0),
    _bufferEnd(0) {
  }

  ~Allocator() {
    clear();
  }

  void clear() throw() {
    for (typename list<T*>::iterator it = _buffers.begin();
	 it != _buffers.end(); ++it)
      doBuffer(*it);
    _buffers.clear();

    _buffer = 0;
    _bufferEnd = 0;
  }

  T* allocate() throw() {
    if (_buffer == _bufferEnd) {
      _buffer = doBuffer(0);
      _bufferEnd = _buffer + BufferSize;
      _buffers.push_back(_buffer);
    }

    return _buffer++;
  }

  static void clearStatic() throw() {
    doBuffer((T*)1);
  }

private:
  static const int BufferSize = 1000;

  // A hack to get static data working without problems.
  static T* doBuffer(T* buffer) {
    static vector<T*> buffers;

    if (buffer == (T*)1) {
      // TODO: check if 1 is really invalid as a pointer.
      for (typename vector<T*>::iterator it = buffers.begin();
	   it != buffers.end(); ++it)
	delete[] *it;
      buffers.clear();
      return 0;
    }
    else if (buffer == 0) {
      if (buffers.empty()) {
	return new T[BufferSize];
      }
      else {
	T* tmp = buffers.back();
	buffers.pop_back();
	return tmp;
      }
    } else {
      buffers.push_back(buffer);
      return 0;
    }
  }

  Allocator(const Allocator&); // not supported
  Allocator& operator=(const Allocator&); // not supported

  list<T*> _buffers;
  T* _buffer;
  T* _bufferEnd;
};

#endif
