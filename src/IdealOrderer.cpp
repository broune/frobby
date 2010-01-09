/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2010 University of Aarhus
   Contact Bjarke Hammersholt Roune for license information (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
#include "stdinc.h"
#include "IdealOrderer.h"

#include "Ideal.h"
#include "TermPredicate.h"
#include "IdealOrderer.h"
#include "NameFactory.h"
#include "TermExtra.h"
#include "ElementDeleter.h"

#include <algorithm>
#include <iterator>
#include <map>

IdealOrderer::~IdealOrderer() {
}

namespace {
  template<class Pred>
  class IdealSorter : public IdealOrderer {
  public:
	static const char* staticGetName() {
	  return Pred::staticGetName();
	}

  private:
	virtual void doOrder(Ideal& ideal) const {
	  Pred pred;
	  pred.setVarCount(ideal.getVarCount());
	  stable_sort(ideal.begin(), ideal.end(), pred);
	}
  };

  class RandomOrderer : public IdealOrderer {
  public:
	static const char* staticGetName() {return "random";}
  private:
	void doOrder(Ideal& ideal) const {
	  random_shuffle(ideal.begin(), ideal.end());
	}
  };

  class TotalDegreeComparator : public TermPredicate {
  public:
	static const char* staticGetName() {return "tdeg";}
  private:
	virtual bool doPredicate(const Exponent* a,
							 const Exponent* b) const {
	  totalDegree(_degA, a, getVarCount());
	  totalDegree(_degB, b, getVarCount());
	  return a < b;
	}
	mutable mpz_class _degA; // member to avoid repeated allocation
	mutable mpz_class _degB;
  };

  class MedianComparator : public TermPredicate {
  public:
	static const char* staticGetName() {return "median";}
  private:
	virtual bool doPredicate(const Exponent* a,
							 const Exponent* b) const {
	  return median(a, getVarCount()) < median(b, getVarCount());
	}
  };

  class MedianPositiveComparator : public TermPredicate {
  public:
	static const char* staticGetName() {return "posMedian";}
  private:
	virtual bool doPredicate(const Exponent* a,
							 const Exponent* b) const {
	  return medianPositive(a, getVarCount()) <
		medianPositive(b, getVarCount());
	}
  };

  class MinimumPositiveComparator : public TermPredicate {
  public:
	static const char* staticGetName() {return "minPos";}
  private:
	virtual bool doPredicate(const Exponent* a,
							 const Exponent* b) const {
	  return minimumPositive(a, getVarCount()) <
		minimumPositive(b, getVarCount());
	}
  };

  class MaximumComparator : public TermPredicate {
  public:
	static const char* staticGetName() {return "max";}
  private:
	virtual bool doPredicate(const Exponent* a,
							 const Exponent* b) const {
	  return maximum(a, getVarCount()) < maximum(b, getVarCount());
	}
  };

  class SupportComparator : public TermPredicate {
  public:
	static const char* staticGetName() {return "support";}
  private:
	virtual bool doPredicate(const Exponent* a,
							 const Exponent* b) const {
	  return Term::getSizeOfSupport(a, getVarCount()) <
		Term::getSizeOfSupport(b, getVarCount());
	}
  };

  class StrongGenericityOrderer : public IdealOrderer {
  public:
	static const char* staticGetName() {return "strongGenericity";}

  protected:
	void orderGenericity(Ideal& ideal, bool strong) const {
	  // Using a map here is the prettier of several ugly
	  // alternaties. Only trade more ugly for efficiency if this
	  // method turns up as a significant consumer of time in a
	  // profiler.
	  UnGenMap ungenericity;

	  // Make ungenericity[gen] be the number of other generators that
	  // shares a positive exponent with gen.
	  Term tmp(ideal.getVarCount());
	  for (cit a = ideal.begin(); a != ideal.end(); ++a) {
		for (cit b = a + 1; b != ideal.end(); ++b) {
		  if (Term::sharesNonZeroExponent(*a, *b, ideal.getVarCount())) {
			if (!strong) {
			  tmp.lcm(*a, *b);
			  if (ideal.strictlyContains(tmp))
				continue;
			}
			++ungenericity[*a];
			++ungenericity[*b];
		  }
		}
	  }

	  Pred pred(ungenericity);
	  stable_sort(ideal.begin(), ideal.end(), pred);
	}

  private:
	typedef Ideal::const_iterator cit;
	typedef map<const Exponent*, size_t> UnGenMap;

	virtual void doOrder(Ideal& ideal) const {
	  orderGenericity(ideal, true);
	}

	class Pred {
	public:
	  Pred(UnGenMap& ungenericity): _ungenericity(ungenericity) {}

	  bool operator()(const Exponent* a, const Exponent* b) const {
		return _ungenericity[a] < _ungenericity[b];
	  }

	private:
	  UnGenMap& _ungenericity;
	};
  };

  class NullOrderer : public IdealOrderer {
  public:
	static const char* staticGetName() {return "null";}
  private:
	virtual void doOrder(Ideal& ideal) const {}
  };

  /** @todo: do this more elegantly than with inheritance. */
  class WeakGenericityOrderer : public StrongGenericityOrderer {
  public:
	static const char* staticGetName() {return "weakGenericity";}
  private:
	virtual void doOrder(Ideal& ideal) const {
	  orderGenericity(ideal, false);
	}
  };

  class CompositeOrderer : public IdealOrderer {
  public:
	CompositeOrderer(): _orderersDeleter(_orderers) {}

	void refineOrderingWith(auto_ptr<IdealOrderer> orderer) {
	  exceptionSafePushBack(_orderers, orderer);
	}

  private:
	typedef vector<IdealOrderer*> Container;
	typedef Container::const_reverse_iterator rev_cit;

	virtual void doOrder(Ideal& ideal) const {
	  // This works because orderes that do not define a total order
	  // such that they can be meaningfully refined use a stable
	  // sorting algorithm.
	  rev_cit rbegin(_orderers.end());
	  rev_cit rend(_orderers.begin());
	  for (rev_cit it = rbegin; it != rend; ++it)
		(*it)->order(ideal);
	}

	Container _orderers;
	ElementDeleter<Container> _orderersDeleter;
  };

  typedef NameFactory<IdealOrderer> OrdererFactory;
  OrdererFactory getOrdererFactory() {
	OrdererFactory factory("ordering of terms");

	nameFactoryRegister<RandomOrderer>(factory);
	nameFactoryRegister<NullOrderer>(factory);
	nameFactoryRegister<IdealSorter<LexComparator> >(factory);
	nameFactoryRegister<IdealSorter<ReverseLexComparator> >(factory);
	nameFactoryRegister<IdealSorter<TotalDegreeComparator> >(factory);
	nameFactoryRegister<IdealSorter<MedianComparator> >(factory);
	nameFactoryRegister<IdealSorter<MedianPositiveComparator> >(factory);
	nameFactoryRegister<IdealSorter<MinimumPositiveComparator> >(factory);
	nameFactoryRegister<IdealSorter<MaximumComparator> >(factory);
	nameFactoryRegister<IdealSorter<SupportComparator> >(factory);
	nameFactoryRegister<StrongGenericityOrderer>(factory);
	nameFactoryRegister<WeakGenericityOrderer>(factory);

	return factory;
  }
}

auto_ptr<IdealOrderer> createIdealOrderer(const string& prefix) {
  OrdererFactory factory = getOrdererFactory();
  if (prefix.find('_') == string::npos)
	return createWithPrefix(factory, prefix);

  auto_ptr<CompositeOrderer> composite(new CompositeOrderer());
  size_t pos = 0;
  while (true) {
	size_t nextUnderscore = prefix.find('_', pos);
	string subPrefix = prefix.substr(pos, nextUnderscore - pos);
	composite->refineOrderingWith(createWithPrefix(factory, subPrefix));

	if (nextUnderscore == string::npos)
	  break;
	pos = nextUnderscore + 1;
  }
  return auto_ptr<IdealOrderer>(composite.release());
}
