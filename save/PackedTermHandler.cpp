
class PackedTermHandler {
  class RankCompare;
  class LexComparator;

public:
  class ExternalTerm;

  class Term {
  public:
    unsigned int getIndex() const {
      return index;
    }

  private:
    friend class PackedTermHandler;
    friend class PackedTermHandler::RankCompare;
    friend class PackedTermHandler::ExternalTerm;

    int index;
    mutable const Term* lastDivider;
    unsigned int support;
    Exponent exponents[1];    
  };

  unsigned int getTermSizeInBytes() const {
    return sizeof(Exponent) * (_exponentWords + 3);
  }

  typedef Ranker<Term, LexComparator> Ranker;

  static bool vectorCompare(const vector<Exponent>& a,
			    const vector<Exponent>& b) {
    ASSERT(a.size() == b.size());
    for (int i = 0; i < (int)a.size(); ++i) {
      if (a[i] < b[i])
	return true;
      if (a[i] > b[i])
	return false;
    }
    return false;
  }

  PackedTermHandler(int dimension,
	      const vector<vector<Exponent> >& termsUnsorted):
    _rankers(0) {

    vector<vector<Exponent> > terms(termsUnsorted);
    std::sort(terms.begin(), terms.end(), vectorCompare);

    _size = terms.size();
    _dimension = dimension;
    _exponentWords = (_dimension + 3) >> 2; 

    _termMemory = (Term*)new char[terms.size() * getTermSizeInBytes()];
    for (int i = 0; i < (int)terms.size(); ++i) {
      Term* term = (Term*)indexToMemory(i);

      initializeExponents(term, terms[i].begin(), terms[i].end());
      term->support = computeSupport(term);
      term->lastDivider = 0;
      term->index = i;
    }

    computeRanks();
  }

  ~PackedTermHandler() {
    delete[] _rankers;
    delete[] _termMemory;
  }

  void fillArraySorted(const Term** array) {
    for (int i = 0; i < (int)size(); ++i)
      array[i] = indexToMemory(i);
  }

  Ranker::Predicate getLexPredicate(int position) {
    return _rankers[position].getPredicate();
  }

  int getDimension() const {
    return _dimension;
  }

  size_t size() const {
    return _size;
  }

  void getFirstLarger(const Term**& begin,
		      const Term** end,
		      Exponent largerThanThisE,
		      int position) const {
    unsigned int largerThanThis = (unsigned int)largerThanThisE;
    unsigned int mask = 0xFF000000 >> ((position & 3) << 3);
    largerThanThis = largerThanThis << ((3 - (position & 3)) << 3);
    position = position >> 2;
    /*
    if (begin == end)
      return;

//    cout << "at getFirstLarger begin=";
//    write(*begin, cout);
    cout << " largerThanThis=" << largerThanThis << " largerThanThisE=" << largerThanThisE;
    */
    while (begin != end &&
	   (((*begin)->exponents[position]) & mask) <= largerThanThis)
      ++begin;
  }

  void lcm(ExternalTerm& target,
	   const ExternalTerm& source1,
	   const Term* source2,
	   int position) {
    /*    cout << "doing lcm. target=";
    write(target._term, cout);
    cout << " source1=";
    write(source1._term, cout);
    cout << " source2=";
    write(source2, cout);
    cout << " position=" << position << endl;*/
    

    position = position >> 2;
    
    for (int i = position; i < _exponentWords; ++i) {
      Exponent e1 = source1._term->exponents[i];
      Exponent e2 = source2->exponents[i];

      Exponent exponent = 0xFF000000 & max(e1, e2);
      Exponent result = exponent;

      exponent = max(e1 & 0x00FF0000, e2 & 0x00FF0000);
      result |= exponent;

      exponent = max(e1 & 0x0000FF00, e2 & 0x0000FF00);
      result |= exponent;

      exponent = max(e1 & 0x000000FF, e2 & 0x000000FF);
      result |= exponent;

      target._term->exponents[i] = result;
    }
    
    /*
    
    for (int i = position; i < _dimension; ++i) {
      Exponent e1 = getExponent(source1._term, i);
      Exponent e2 = getExponent(source2, i);
      target.setExponent(i, e1 > e2 ? e1 : e2);
    }
    */

#ifdef PROFILE
    if (_dimension == 1000000) {
      combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();
      combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();
      combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();
      combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();
      combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();
    }
#endif

  }

  bool someDivides(const Term** begin,
		   const Term** end,
		   const Term* term,
		   const Term*& divisorParam,
		   int originalPosition) {
    ASSERT(originalPosition != _dimension);

    //    cout << "dpos=" << originalPosition;

    unsigned int mask = 0x80808080 >> ((originalPosition & 3) << 3);
    int position = originalPosition >> 2;

    unsigned int support = term->support;
    const Exponent* exponents = term->exponents + position;
    const Exponent* exponentsEnd = term->exponents + _exponentWords;

    for (const Term** it = begin; it != end; ++it) {
      const Term* divisor = *it;

      if (supportsPrecludeDivision(divisor->support, support, originalPosition)) {
//	cout << " [support " << divisor->support << ' ' << support << ' ' << originalPosition << ']';
	
	goto DoesNotDivide;
      }

      {
      const Exponent* divIt = divisor->exponents + position;
      const Exponent* exp = exponents;

      if ((*exp - *divIt) & mask) {
	//	cout << "[first]";
	goto DoesNotDivide;
      }

      ++divIt;
      ++exp;
      
      for (; exp != exponentsEnd; ++divIt, ++exp)
	if ((*exp - *divIt) & 0x80808080)
	  goto DoesNotDivide;
      
      /*      write(*it, cout);
      cout << " divides ";
      write(term, cout);
      cout << endl;*/
      divisorParam = divisor;
      return true;
      }
    DoesNotDivide:;
      /*      write(*it, cout);
      cout << " does not divide ";
      write(term, cout);
      cout << endl;*/
      }
#ifdef PROFILE
    if (_dimension == 1000000) {
      combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();
      combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();
      combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();
      combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();
      combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();
    }
#endif
    return false;
    }

  bool someDivides(const Term** begin,
		   const Term** end,
		   const ExternalTerm& term,
		   const Term*& divisorParam,
		   int position) {
    return someDivides(begin, end, term._term, divisorParam, position);
  }

  bool divides(const Term* divisor, const ExternalTerm& divided, int position) {
    ASSERT(position != _dimension);

    /*    cout << "does ";
    write(divisor, cout);
    cout << " divide ";
    write(divided._term, cout);
    cout << "?" << endl;*/

    if ((divisor->support & ~(divided._term->support)) >> position)
      return false;
  
    unsigned int mask = 0x80808080 >> ((position & 3) << 3);
    position = position >> 2;

    const Exponent* divisorIt = divisor->exponents + position;
    const Exponent* dividedIt = divided._term->exponents + position;
    const Exponent* dividedEnd = divided._term->exponents + _exponentWords;

    if ((*dividedIt - *divisorIt) & mask)
      return false;

    ++dividedIt;
    ++divisorIt;

    for (; dividedIt != dividedEnd; ++dividedIt, ++divisorIt)
      if ((*dividedIt - *divisorIt) & 0x80808080)
        return false;
    //    cout << "yes" << endl;

#ifdef PROFILE
    if (_dimension == 1000000) {
      combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();
      combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();
      combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();
      combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();
      combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();combatInlining();
    }
#endif

    return true;
  }
  
  const Term* getLastDivider(const Term* term) {
    return term->lastDivider;
  }

  void setLastDivider(const Term* term, const Term* divider) {
    term->lastDivider = divider;
  }

  Exponent getExponent(const Term* term, int position) const {
    unsigned int adjustment = (3 - (position & 3)) << 3;
    position = position >> 2;

    return (term->exponents[position] >> adjustment) & 0x000000FF;
  }

  Exponent getWord(const Term* term, int position) const {
    return term->exponents[position];
  }

  int getWordCount() const {
    return _exponentWords;
  }

  

  bool write(const Term* term, ostream& out) const {
    out << '(' << getExponent(term, 0);
    for (int i = 1; i < _dimension; ++i)
      out << ", " << getExponent(term, i);
    out << ')';

    /*    out << "~[" << getWord(term, 0);
    for (int i = 1; i < _exponentWords; ++i)
      out << ", " << getWord(term, i);
      out << ']';*/
    return out;
  }

private:
  void initializeExponents(Term* term,
			   vector<Exponent>::iterator begin,
			   vector<Exponent>::iterator end) {
    Exponent* exponent = term->exponents;

    ASSERT(distance(begin,end) == _dimension);
    int variablesToGo = _dimension;

    while (variablesToGo > 0) {
      int value = 0;
      for (int i = 0; i < 4; ++i) {
	value <<= 8;
	if (variablesToGo == 0)
	  continue;

	if (begin != end) {
	  ASSERT(*begin <= 127);

	  value |= *begin;
	  ++begin;
	  --variablesToGo;
	}
      }
      *exponent = value;
      ++exponent;
    }
  }


  unsigned int computeSupport(const Term* term) const {
    unsigned int support = 0;
    for (int i = _exponentWords - 1; i >= 0; --i) {
      // hex 7F is binary 0111111, so adding 7F to 0aaaaaaa will give
      // 1bbbbbbb exactly if any a is different from 0. So doing this
      // and anding with hex 80 (which is 10000000) tests for zero.

      unsigned int spreadSubSupport = term->exponents[i] + 0x7F7F7F7F;
      unsigned int subSupport =
	((spreadSubSupport & 0x80000000) >> 31) |
	((spreadSubSupport & 0x00800000) >> 22) |
	((spreadSubSupport & 0x00008000) >> 13) |
	((spreadSubSupport & 0x00000080) >> 4);

      // subSupport now contains the support of exponents[i] in
      // the lower 4 bits.

      if (i == _exponentWords - 1) {
	int bits = ((_dimension - 1) & 3) + 1;
	support = (support << bits) | (subSupport >> (4 - bits));
      } else
	support = (support << 4) | subSupport;
    }
    /*    cout << hex << "the support of";
    write(term, cout);
    cout << " is " << support << endl;*/
    return support;
  }

  static unsigned int supportsPrecludeDivision(unsigned int divisor,
					       unsigned int divided,
					       int position) {
    return (divisor & ~divided) >> position;
  }
  
  class LexComparator {
  public:
    LexComparator(PackedTermHandler& handler, int position):
      _handler(handler),
      _position(position) {
    }

    bool operator()(const Term* a, const Term* b) const {
      /*      _handler.write(a, cout);
      cout << "<";
      _handler.write(b,cout);
      cout << " for position=" << _position << "? ";*/

      unsigned int mask = 0xFFFFFFFF >> ((_position & 3) << 3);

      for (int i = _position >> 2; i < _handler.getWordCount(); ++i) {
	Exponent aEntry = _handler.getWord(a, i) & mask;
	Exponent bEntry = _handler.getWord(b, i) & mask;
	mask = 0xFFFFFFFF;
	
	// Yes, this actually does lex comparison on the exponents
	// contained within.
	if (aEntry < bEntry) {
	  //	  cout << "true" << endl;
	  return true;
	}
	if (aEntry > bEntry) {
	  //	  cout << "false" << endl;
	  return false;
	}
      }
      //      cout << "false" << endl;
      return false;    
    }

  private:
    PackedTermHandler& _handler;
    int _position;
  };

  void computeRanks() {
    const Term** buffer = new const Term*[_size];
    fillArraySorted(buffer);

    _rankers = new Ranker[_dimension];
    for (int position = 0; position < _dimension; ++position)
      _rankers[position].init(buffer, buffer + _size,
			      LexComparator(*this, position));

    delete[] buffer;
  }

  int indexOf(const Term* term) const {
    return term->index;
  }

  const Term* indexToMemory(int index) const {
    return (const Term*)((char*)_termMemory + index * getTermSizeInBytes());
  }

  void combatInlining() const {
    ((PackedTermHandler*)this)->computeRanks();
  }

  void setExponent(Term* term, int position, Exponent value) const {
    //    cout << "(setting " << value << ")";
    value = value << ((3 - (position & 3)) << 3);
    unsigned int mask = 0xFF000000 >> ((position & 3) << 3);
    position = position >> 2;

    term->exponents[position] = (term->exponents[position] & ~mask) | value;
  }

  int _dimension;
  int _exponentWords;
  Ranker* _rankers;
  Term* _termMemory;
  size_t _size;
  
  
public:
  friend class ExternalTerm;

  class ExternalTerm {
    friend class PackedTermHandler;

  public:
    ExternalTerm(const ExternalTerm& term):
      _handler(term._handler) {
      _term = (Term*)new char[_handler.getTermSizeInBytes()];
      _term->support = term._term->support;
      copy(term._term->exponents, term._term->exponents + _handler.getWordCount(),
	   _term->exponents);
    }

    ExternalTerm(const PackedTermHandler& handler):
      _handler(handler) {
      _term = (Term*)new char[_handler.getTermSizeInBytes()];
      _term->support = 0;
      fill_n(_term->exponents, _handler.getWordCount(), 0);
    }

    ~ExternalTerm() {
      delete[] _term;
    }

    const Exponent* begin(int position) const {
      return _term->exponents + position;
    }

    const Exponent* end() const {
      return _term->exponents + _handler.getDimension();
    }

    bool write(ostream& out) const {
      return _handler.write(_term, out);
    }

    Exponent operator[](int position) const {
      return _handler.getExponent(_term, position);
    }

    void setExponent(int position, Exponent value) const {
      _handler.setExponent(_term, position, value);
    }
  
    unsigned int getSupport() const {
      return _term->support;
    }

    void updateSupport() {
      _term->support = _handler.computeSupport(_term);
    }

  private:
    Term* _term;
    const PackedTermHandler& _handler;
  };  
};
/*
typedef PackedTermHandler TermHandler;
typedef PackedTermHandler::Term Term;
typedef PackedTermHandler::ExternalTerm ExternalTerm;
*/
ostream& operator<<(ostream& out, const PackedTermHandler::ExternalTerm& term) {
  term.write(out);
  return out;
}
