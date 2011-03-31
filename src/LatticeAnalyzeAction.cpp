/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 Bjarke Hammersholt Roune (www.broune.com)

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
#include "LatticeAnalyzeAction.h"

#include "SatBinomIdeal.h"
#include "IOFacade.h"
#include "Scanner.h"
#include "IOHandler.h"
#include "DataType.h"
#include "BigIdeal.h"
#include "MsmStrategy.h"
#include "TermTranslator.h"
#include "TranslatingTermConsumer.h"
#include "DebugStrategy.h"
#include "Matrix.h"
#include "ColumnPrinter.h"
#include "BigTermRecorder.h"
#include "SliceParams.h"
#include "SliceFacade.h"

#include <algorithm>
#include <set>
#include <sstream>
#include <limits>
#include <fstream>


#define CHECK(X)											\
  if (!(X)) {												\
    cout << "Check condition on line "						\
		 << __LINE__ << " not satisfied: "#X << endl;		\
    exit(1);												\
  }

/**
This file has stuff I'm using in my work with Scarf. It's just
whatever code gets the job done so we can figure out the structure of
these lattices. -Bjarke H. Roune

@todo: do not use cout

@todo: move functionality into proper classes away from this file. */
#include <iostream>

namespace {
  enum NeighborPlace {
	InPlane,
	UnderPlane,
	OverPlane,
	NoPlace
  };

  char getPlaceCode(NeighborPlace place) {
	switch (place) {
	case InPlane: return 'P';
	case UnderPlane: return 'U';
	case OverPlane: return 'O';
	case NoPlace: return ' ';
	default: return 'E';
	}
  }

  void printIndentedMatrix(const Matrix& matrix) {
    ColumnPrinter pr;
    pr.setPrefix("  ");
    print(pr, matrix);

    fputc('\n', stdout);
    print(stdout, pr);
    fputc('\n', stdout);
  }

  void printNullSpace(const Matrix& matrix) {
    Matrix nullSpaceBasis;
    nullSpace(nullSpaceBasis, matrix);
    transpose(nullSpaceBasis, nullSpaceBasis);

    fputs("The right null space is spanned by the rows of\n",
		  stdout);
    printIndentedMatrix(nullSpaceBasis);
  }

  /** A lattice with associated Grobner basis/neighbors. */
  class GrobLat {
  public:
    GrobLat(const Matrix& matrix, const SatBinomIdeal& ideal) {
      _ideal = ideal;
      _ideal.getMatrix(_y);

      // transpose in preparation for solve
      transpose(_y);
      transpose(_mat, matrix);

      solve(_h, _mat, _y);

      // un-transpose
      transpose(_mat);
      transpose(_y);
      transpose(_h);
    }

    size_t getNeighborCount() const {
      ASSERT(_h.getRowCount() == _y.getRowCount());
      return _y.getRowCount();
    }

    const Matrix& getYMatrix() const {
      return _y;
    }

    const Matrix& getHMatrix() const {
      return _h;
    }

    const Matrix& getMatrix() const {
      return _mat;
    }

    const SatBinomIdeal& getIdeal() const {
      return _ideal;
    }

	size_t getSum(size_t a, size_t b) const {
	  vector<mpq_class> sum(getHDim());
	  for (size_t i = 0; i < getHDim(); ++i)
		sum[i] = _h(a, i) + _h(b, i);
	  for (size_t row = 0; row < _h.getRowCount(); ++row) {
		bool match = true;
		for (size_t col = 0; col < _h.getColCount(); ++col)
		  if (sum[col] != _h(row, col))
			match = false;
		if (match)
		  return row;
	  }
	  return _h.getRowCount();
	}

    size_t getYDim() const {
      ASSERT(_y.getColCount() == _ideal.getVarCount());
      return _y.getColCount();
    }

    size_t getHDim() const {
      return _h.getColCount();
    }

    bool hasZeroEntryY() const {
      return _ideal.hasZeroEntry();
    }

	void getInitialIdeal(BigIdeal& ideal) const {
	  _ideal.getInitialIdeal(ideal);
	}

  private:
    Matrix _y; // rows are neighbors in y-space
    Matrix _h; // rows are neighbors in h-space
    Matrix _mat; // matrix that defines lattice
    SatBinomIdeal _ideal; // other representation of _y, necessary for now
  };

  struct Mlfb {
	mpq_class index;
	mpz_class dotDegree;
	vector<size_t> points;
	vector<size_t> edges;
	vector<size_t> edgeHitsFacet;
	vector<mpz_class> rhs;
	size_t planeCount;
	size_t id;

	bool hasPoint(size_t n) const {
	  for (size_t i = 0; i < points.size(); ++i)
		if (points[i] == n)
		  return true;
	  return false;
	}

	bool operator<(const Mlfb& mlfb) const {
	  if (planeCount > mlfb.planeCount)
		return true;
	  if (planeCount < mlfb.planeCount)
		return false;

	  return points.back() > mlfb.points.back();
	}

	size_t getHitsNeighbor(size_t index) const {
	  if (index < edgeHitsFacet.size()) {
		size_t hits = edgeHitsFacet[index];
		return hits == 0 ? 0 : points[hits - 1] + 1;
	  } else
		return numeric_limits<size_t>::max();
	}

	size_t getHitsFacet(size_t index) const {
	  ASSERT(index < edgeHitsFacet.size());
	  return edgeHitsFacet[index];
	}

	size_t getEdge(size_t index) const {
	  if (index < edges.size())
		return edges[index];
	  else
		return numeric_limits<size_t>::max();
	}

	bool isPivot() const {
	  return planeCount == 1 || planeCount == 3;
	}

	bool isFlat() const {
	  return planeCount == 4;
	}
  };

  size_t pushOutFacetPositive(size_t facetPushOut,
							  const vector<mpz_class>& rhs,
							  const GrobLat& lat) {
	size_t onFacet = numeric_limits<size_t>::max();
	mpq_class leastEntry;

	for (size_t n = 0; n < lat.getNeighborCount(); ++n) {
	  for (size_t i = 0; i < lat.getYDim(); ++i) {
		if (i == facetPushOut)
		  continue;
		if (lat.getYMatrix()(n, i) >= rhs[i])
		  goto notOnFacet;
	  }

	  if (onFacet == numeric_limits<size_t>::max() ||
		  leastEntry > lat.getYMatrix()(n, facetPushOut)) {
		leastEntry = lat.getYMatrix()(n, facetPushOut);
		onFacet = n;
	  }

	notOnFacet:;
	}
	return onFacet;
  }

  size_t pushOutFacetZero(const vector<mpz_class>& rhs, const GrobLat& lat) {
	size_t onFacet = numeric_limits<size_t>::max();
	mpq_class leastEntry;

	for (size_t n = 0; n < lat.getNeighborCount(); ++n) {
	  for (size_t i = 1; i < lat.getYDim(); ++i)
		if (-lat.getYMatrix()(n, i) >= rhs[i])
		  goto notOnFacet;

	  if (onFacet == numeric_limits<size_t>::max() ||
		  leastEntry > -lat.getYMatrix()(n, 0)) {
		leastEntry = -lat.getYMatrix()(n, 0);
		onFacet = n;
	  }

	notOnFacet:;
	}
	return onFacet;
  }

  void computeMlfbs(vector<Mlfb>& mlfbs, const GrobLat& lat) {
	BigIdeal initialIdeal;
	lat.getInitialIdeal(initialIdeal);

	BigTermRecorder recorder;
	SliceParams params;
	SliceFacade facade(params, initialIdeal, recorder);
	facade.computeIrreducibleDecomposition(true);
	auto_ptr<BigIdeal> rhsesOwner = recorder.releaseIdeal();
	BigIdeal& rhses = *rhsesOwner;
	ASSERT(recorder.empty());

	mlfbs.clear();
	mlfbs.resize(rhses.getGeneratorCount());

	for (size_t i = 0; i < mlfbs.size(); ++i) {
	  Mlfb& mlfb = mlfbs[i];
	  mlfb.id = i + 1;

	  for (size_t var = 0; var < lat.getYDim(); ++var)
		mlfb.rhs.push_back(rhses[i][var]);

	  for (size_t gen = 0; gen < initialIdeal.getGeneratorCount(); ++gen) {
		for (size_t var = 0; var < initialIdeal.getVarCount(); ++var)
		  if (initialIdeal[gen][var] > mlfb.rhs[var])
			goto skipIt;
		mlfb.points.push_back(gen);
	  skipIt:;
	  }

	  // order to have maxima along diagonal if possible.
	  if (mlfb.points.size() == lat.getYDim() - 1) {
		// -1 due to the missing 0.
		for (size_t i = 1; i < lat.getYDim(); ++i)
		  for (size_t p = 0; p < mlfb.points.size(); ++p)
			if (lat.getYMatrix()(mlfb.points[p], i) == mlfb.rhs[i])
			  swap(mlfb.points[i-1], mlfb.points[p]);
	  }

	  
	  // Compute MLFB index.
	  {
		Matrix mat(mlfb.points.size(), lat.getHDim());
		for (size_t point = 0; point < mlfb.points.size(); ++point)
		  for (size_t var = 0; var < lat.getHDim(); ++var)
			mat(point, var) = lat.getHMatrix()(mlfb.points[point], var);
		if (mlfb.points.size() == lat.getHDim())
		  mlfb.index = determinant(mat);
	  }
	}

	Matrix nullSpaceBasis;
    nullSpace(nullSpaceBasis, lat.getMatrix());
    transpose(nullSpaceBasis, nullSpaceBasis);
	// the basis is the rows of NullSpaceBasis at this point.

	for (size_t m = 0; m < mlfbs.size(); ++m) {
	  Mlfb& mlfb = mlfbs[m];

	  if (mlfb.points.size() != lat.getYDim() - 1)
		continue;

	  // Compute dot degree.
	  if (nullSpaceBasis.getRowCount() == 1 &&
		  mlfb.rhs.size() == nullSpaceBasis.getColCount()) {
		mlfb.dotDegree = 0;
		for (size_t r = 0; r < nullSpaceBasis.getRowCount(); ++r)
		  for (size_t c = 0; c < nullSpaceBasis.getColCount(); ++c)
			mlfb.dotDegree += nullSpaceBasis(r, c) * mlfb.rhs[c];
	  }

	  // Compute Scarf edges.
	  mlfb.edges.resize(lat.getYDim());
	  mlfb.edgeHitsFacet.resize(lat.getYDim());
	  for (size_t facetPushIn = 0; facetPushIn < lat.getYDim(); ++facetPushIn) {
		mpq_class secondLargest;
		size_t facetPushOut = numeric_limits<size_t>::max();

		for (size_t neigh = 0; neigh < lat.getYDim(); ++neigh) {
		  mpq_class entry = 0; // neigh == 0 represents zero.
		  if (neigh > 0)
			entry = lat.getYMatrix()(mlfb.points[neigh - 1], facetPushIn);

		  if (neigh == facetPushIn) {
			if (entry != mlfb.rhs[facetPushIn])
			  goto skipBecauseNotGeneric;
		  } else {
			if (entry == secondLargest &&
				facetPushOut != numeric_limits<size_t>::max())
			  goto skipBecauseNotGeneric;

			if (entry > secondLargest ||
				facetPushOut == numeric_limits<size_t>::max()) {
			  secondLargest = entry;
			  facetPushOut = neigh;
			}
		  }
		}

		// ----------------------------------------------
		// ** Case 1: facetPushIn > 0 and facetPushOut > 0
		//
		// We push in facetPushIn (discarding the non-zero neighbor
		// previously on that facet) and hit a non-zero neighbor
		// that is already on the MLFB. That neighbor now instead
		// lies on facetPushIn and we push out facetPushOut in the
		// straight forward way until it hits what will be the new
		// neighbor on facetPushOut.
		//
		// ----------------------------------------------
		// ** Case  2: facetPushIn > 0 and facetPushOut == 0
		//
		// We push in facetPushIn (discarding the non-zero neighbor
		// previously on that facet) and hit zero. Zero now instead
		// lies on facetPushIn and we push out facetPushOut. It will
		// be pushing into the area on the opposite side from the
		// half-set of neighbors that we are looking at, so to find
		// the replacement neighbor to put on facetPushOut
		// (i.e. facet zero) we need to consider the negative of the
		// neighbors we have. When that neighbor -v has been found,
		// we need to translate the whole body by +v so that zero
		// will once again lie on facet zero.
		//
		// ----------------------------------------------
		// ** case 3: facetPushIn == 0 and facetPushOut > 0
		//
		// We push in facetPushIn (discarding the zero previously on
		// that facet) and hit a non-zero neighbor v on
		// facetPushOut. Then v lies on facetPushIn (i.e. facet
		// zero), so for the MLFB to have zero on facet 0 we need to
		// translate it by -v. We then relax facetPushOut to find a
		// replacement neighbor as in Case 1.

		ASSERT(facetPushIn == 0 || secondLargest >= 0);
		// In all cases the neighbor we hit moves to facetPushIn.
		vector<mpz_class> rhs(mlfb.rhs);

		rhs[facetPushIn] = secondLargest;

		if (facetPushIn == 0) {
		  // Case 3: the neighbor hit moves to facet 0 so translate
		  // the body to make that neighbor zero.
		  for (size_t i = 0; i < lat.getYDim(); ++i)
			rhs[i] -= lat.getYMatrix()(mlfb.points[facetPushOut - 1], i);
		}

		if (facetPushOut > 0) {
		  // Case 1 or 3: push out in the usual way.
		  size_t newNeighbor = pushOutFacetPositive(facetPushOut, rhs, lat);
		  if (newNeighbor == numeric_limits<size_t>::max())
			goto skipBecauseNotGeneric;
		  rhs[facetPushOut] = lat.getYMatrix()(newNeighbor, facetPushOut);
		}

		if (facetPushOut == 0) {
		  // Case 2: push out into negative neighbors
		  size_t newNeighbor = pushOutFacetZero(rhs, lat);
		  if (newNeighbor == numeric_limits<size_t>::max())
			goto skipBecauseNotGeneric;
		  for (size_t i = 0; i < lat.getYDim(); ++i)
			rhs[i] += lat.getYMatrix()(newNeighbor, i);
		  rhs[0] = 0;
		}

		// Find the MLFB with the right hand side that we have
		// computed.
		for (size_t m = 0; m < mlfbs.size(); ++m) {
		  if (mlfbs[m].rhs == rhs) {
			mlfb.edges[facetPushIn] = m + 1;
			mlfb.edgeHitsFacet[facetPushIn] = facetPushOut;
			goto foundMatch;
		  }
		}
		goto skipBecauseNotGeneric;
	  foundMatch:;
	  }
	  continue;
	skipBecauseNotGeneric:
	  mlfb.edges.clear();
	  mlfb.edgeHitsFacet.clear();
	}
  }

  class NeighborPrinter {
  public:
	NeighborPrinter(const GrobLat& lat):
	  _lat(lat) {

	  // "gXYZ:" label
	  _labelIndex = _pr.getColumnCount();
	  _pr.addColumn(false, " ");

	  // h space
	  _hHeader = _pr.getColumnCount();
	  _pr.addColumn(false, " ", "");
	  _hIndex = _pr.getColumnCount();
	  for (size_t i = 0; i < _lat.getHDim(); ++i)
		_pr.addColumn(false, i == 0 ? " " : "  ");

	  _comma = _pr.getColumnCount();
	  _pr.addColumn(false, "", " ");

	  // y space
	  _yHeader = _pr.getColumnCount();
	  _pr.addColumn(false, " ", "");
	  _yIndex = _pr.getColumnCount();
	  for (size_t i = 0; i < _lat.getYDim(); ++i)
		_pr.addColumn(false, i == 0 ? " " : "  ");

	  // hits
	  _hitsHeader = _pr.getColumnCount();
	  _pr.addColumn(false, "  ", "");
	  _hits = _pr.getColumnCount();
	  _pr.addColumn(false, " ", "");

	  // edges
	  _edgeHeader = _pr.getColumnCount();
	  _pr.addColumn(false, " ", "");
	  _edge = _pr.getColumnCount();
	  _pr.addColumn(false, " ", "");
	}

	void addLine(size_t neighbor,
				 NeighborPlace place = NoPlace,
				 size_t hits = (size_t)-1,
				 size_t edge = (size_t)-1) {
	  _pr[_labelIndex] << 'g' << (neighbor + 1) << ':';
	  if (place != NoPlace)
		_pr[_labelIndex] << ' ' << getPlaceCode(place);
	  _pr[_labelIndex] << '\n';

	  _pr[_yHeader] << "y=\n";
	  for (size_t i = 0; i < _lat.getYDim(); ++i)
		_pr[_yIndex + i] << _lat.getYMatrix()(neighbor, i) << '\n';
	  _pr[_comma] << ",\n";
	  _pr[_hHeader] << "h=\n";
	  for (size_t i = 0; i < _lat.getHDim(); ++i)
		_pr[_hIndex + i] << _lat.getHMatrix()(neighbor, i) << '\n';

	  if (hits != (size_t)-1) {
		_pr[_hitsHeader] << "hits";
		if (hits == 0)
		  _pr[_hits] << "zero";
		else
		  _pr[_hits] << 'g' << hits;
	  }
	  _pr[_hitsHeader] << '\n';
	  _pr[_hits] << '\n';

	  if (edge != (size_t)-1) {
		_pr[_edgeHeader] << "push to";
		_pr[_edge] << 'm' << edge;
	  }
	  _pr[_edgeHeader] << '\n';
	  _pr[_edge] << '\n';
	}

	void addZeroLine(NeighborPlace place = NoPlace,
					 size_t hits = (size_t)-1,
					 size_t edge = (size_t)-1) {
	  _pr[_labelIndex] << "zero:";
	  if (place != NoPlace)
		_pr[_labelIndex] << ' ' << getPlaceCode(place);
	  _pr[_labelIndex] << '\n';

	  _pr[_yHeader] << "y=\n";
	  for (size_t i = 0; i < _lat.getYDim(); ++i)
		_pr[_yIndex + i] << 0 << '\n';
	  _pr[_comma] << ",\n";
	  _pr[_hHeader] << "h=\n";
	  for (size_t i = 0; i < _lat.getHDim(); ++i)
		_pr[_hIndex + i] << 0 << '\n';

	  if (hits != (size_t)-1) {
		_pr[_hitsHeader] << "hits";
		if (hits == 0)
		  _pr[_hits] << "zero";
		else
		  _pr[_hits] << 'g' << hits;
	  }
	  _pr[_hitsHeader] << '\n';
	  _pr[_hits] << '\n';

	  if (edge != (size_t)-1) {
		_pr[_edgeHeader] << "push to";
		_pr[_edge] << 'm' << edge;
	  }
	  _pr[_edgeHeader] << '\n';
	  _pr[_edge] << '\n';
	}

	void addLine() {
	  for (size_t i = 0; i < _pr.getColumnCount(); ++i)
		_pr[i] << '\n';
	}

	void print(FILE* out) {
	  ::print(out, _pr);
	}

	void print(ostream& out) {
	  out << _pr;
	}

  private:
	const GrobLat& _lat;
	ColumnPrinter _pr;
	size_t _labelIndex;
	size_t _comma;
	size_t _yHeader;
	size_t _yIndex;
	size_t _hHeader;
	size_t _hIndex;
	size_t _hitsHeader;
	size_t _hits;
	size_t _edgeHeader;
	size_t _edge;
  };

  void printMinDotDegreeMlfb(vector<Mlfb>& mlfbs) {
	Mlfb* min = &(mlfbs[0]);
	for (size_t i = 1; i < mlfbs.size(); ++i) {
	  Mlfb& mlfb = mlfbs[i];
	  if (mlfb.dotDegree < min->dotDegree)
		min = &mlfb;
	}
	cout << "The MLFB m" << min->id << " has minimal rhs dot product.\n";
  }

  void printNeighbors(const GrobLat& lat) {
	NeighborPrinter pr(lat);
	pr.addZeroLine();
	for (size_t n = 0; n < lat.getNeighborCount(); ++n)
	  pr.addLine(n);

	fprintf(stdout, "\nThe %u neighbors in y-space and h-space are\n",
			(unsigned int)lat.getNeighborCount());
	pr.print(stdout);
	fputc('\n', stdout);
  }

  struct Tri {
	size_t a;
	size_t b;
	size_t sumAB;
	bool fromFlat;
  };

  struct Plane {
	Matrix nullSpaceBasis;
	vector<Tri> nonMlfbTris;
	Matrix rowAB;

	vector<NeighborPlace> neighborPlace;
  };

  bool getNeighborOnFacetIsOnPlane(const Plane& plane,
								   const Mlfb& mlfb,
								   size_t facet) {
	ASSERT(facet < 4);
	if (facet == 0)
	  return true;
	return plane.neighborPlace[mlfb.points[facet - 1]] == InPlane;
  }
  
										
										

  struct SeqPos {
	size_t mlfb;
	size_t fixFacet1;
	size_t fixFacet2;
	size_t comingFromFacet;

	SeqPos getReverse() const {
	  size_t to;
	  for (to = 0; to < 4; ++to) {
		if (to != fixFacet1 && to != fixFacet2 && to != comingFromFacet)
		  break;
	  }
	  ASSERT(to != 4);
	  SeqPos reverse = *this;
	  reverse.comingFromFacet = to;
	  return reverse;
	}

	void order() {
	  if (fixFacet1 > fixFacet2)
		swap(fixFacet1, fixFacet2);
	}

	bool operator<(const SeqPos& pos) const {
	  if (mlfb < pos.mlfb)
		return true;
	  if (mlfb > pos.mlfb)
		return false;

	  if (fixFacet1 < pos.fixFacet1)
		return true;
	  if (fixFacet1 > pos.fixFacet1)
		return false;

	  if (fixFacet2 < pos.fixFacet2)
		return true;
	  if (fixFacet2 > pos.fixFacet2)
		return false;

	  if (comingFromFacet < pos.comingFromFacet)
		return true;
	  if (comingFromFacet > pos.comingFromFacet)
		return false;

	  return false;
	}
  };

  size_t mlfbIdToIndex(const vector<Mlfb>& mlfbs, size_t id) {
	for (size_t i = 0; i < mlfbs.size(); ++i)
	  if (mlfbs[i].id == id)
		return i;
	ASSERT(false);
	cout << "ERROR no MLFB with requested id." << endl;
	exit(1);
  }

  SeqPos nextInSeq(const vector<Mlfb>& mlfbs, SeqPos pos) {
	size_t pushIn;
	for (pushIn = 0;; ++pushIn) {
	  ASSERT(pushIn < 4);
	  if (pushIn != pos.fixFacet1 &&
		  pushIn != pos.fixFacet2 &&
		  pushIn != pos.comingFromFacet)
		break;
	}

	size_t hits = mlfbs[pos.mlfb].getHitsFacet(pushIn);
	ASSERT(hits != pushIn);

	SeqPos next = pos;
	next.mlfb = mlfbIdToIndex(mlfbs, mlfbs[pos.mlfb].getEdge(pushIn));
	next.comingFromFacet = hits;

	if (pos.fixFacet1 == hits)
	  next.fixFacet1 = pushIn;
	else if (pos.fixFacet2 == hits)
	  next.fixFacet2 = pushIn;

	next.order();
	return next;
  }

  void getSeq(size_t mlfb, size_t to, size_t fix1, size_t fix2, const vector<Mlfb>& mlfbs, vector<SeqPos>& seq) {
	seq.clear();
	size_t from;
	for (from = 0; from < 4; ++from) {
	  if (from == fix1 || from == fix2 || from == to)
		continue;
	  break;
	}
	ASSERT(from != 4);

	SeqPos pos;
	pos.mlfb = mlfb;
	pos.fixFacet1 = fix1;
	pos.fixFacet2 = fix2;
	pos.comingFromFacet = from;
	pos.order();

	seq.push_back(pos);
	do {
	  pos = nextInSeq(mlfbs, pos);
	  seq.push_back(pos);
	} while (mlfbs[pos.mlfb].planeCount != 1 &&
			 mlfbs[pos.mlfb].planeCount != 3);
  }

  void printSequences(const vector<Mlfb>& mlfbs,
					  const Plane& plane,
					  set<SeqPos>& seen,
					  size_t mlfb,
					  bool triplesRequired) {
	for (size_t fix1 = 0; fix1 < 4; ++fix1) {
	  for (size_t fix2 = 0; fix2 < fix1; ++fix2) {
		if (getNeighborOnFacetIsOnPlane(plane, mlfbs[mlfb], fix1) !=
			getNeighborOnFacetIsOnPlane(plane, mlfbs[mlfb], fix2))
		  continue;

		for (size_t from = 0; from < 4; ++from) {
		  if (from == fix1 || from == fix2)
			continue;

		  for (size_t to = 0; to < 4; ++to) {
			if (to == fix1 || to == fix2 || to == from)
			  continue;

			SeqPos pos;
			pos.mlfb = mlfb;
			pos.fixFacet1 = fix1;
			pos.fixFacet2 = fix2;
			pos.comingFromFacet = from;
			pos.order();
			if (seen.find(pos) != seen.end())
			  continue;
			if (seen.find(pos.getReverse()) != seen.end())
			  continue;

			cout << "loop:";

			bool first = true;
			bool stop = false;
			size_t length = 0;
			while (!stop) {
			  ++length;
			  if (seen.find(pos) != seen.end())
				stop = true;
			  seen.insert(pos);
			  if (!first)
				cout << "->";
			  first = false;
			  cout << 'm' << mlfbs[pos.mlfb].id;
			  if (mlfbs[pos.mlfb].planeCount == 1 ||
				  mlfbs[pos.mlfb].planeCount == 3)
				cout << '*';
			  if (mlfbs[pos.mlfb].planeCount == 4)
				cout << 'F';

			  cout << '('
				   << pos.fixFacet1 << pos.fixFacet2 << pos.comingFromFacet
				   << ')';
			  pos = nextInSeq(mlfbs, pos);
			}
			cout << " (length " << (length - 1) << ")\n";
		  }
		}
	  }
	}
  }

  void computePlanes(const GrobLat& lat, vector<Plane>& planes) {
	const SatBinomIdeal& ideal = lat.getIdeal();

	size_t varCount = lat.getYDim();
	size_t neighborCount = lat.getNeighborCount();
	vector<mpz_class> sum(varCount);
	for (size_t gen1 = 0; gen1 < neighborCount; ++gen1) {
	  for (size_t gen2 = gen1 + 1; gen2 < neighborCount; ++gen2) {
		const vector<mpz_class>& g1 = ideal.getGenerator(gen1);
		const vector<mpz_class>& g2 = ideal.getGenerator(gen2);

		// Set sum = g1 + g2.
		for (size_t var = 0; var < varCount; ++var)
		  sum[var] = g1[var] + g2[var];

		if (ideal.isPointFreeBody(g1, sum) && ideal.isPointFreeBody(g2, sum)) {
		  Tri tri;
		  tri.a = gen1;
		  tri.b = gen2;
		  tri.fromFlat = ideal.isPointFreeBody(g1, g2, sum);
		  Matrix rowAB(2, lat.getHDim());
		  copyRow(rowAB, 0, lat.getHMatrix(), gen1);
		  copyRow(rowAB, 1, lat.getHMatrix(), gen2);

		  for (size_t plane = 0; plane < planes.size(); ++plane) {
			if (hasSameRowSpace(rowAB, planes[plane].rowAB)) {
			  if (!tri.fromFlat)
				planes[plane].nonMlfbTris.push_back(tri);
			  goto done;
			}
		  }

		  {
			planes.resize(planes.size() + 1);
			Plane& plane = planes.back();
			plane.rowAB = rowAB;
			nullSpace(plane.nullSpaceBasis, rowAB);
			if (!tri.fromFlat)
			  plane.nonMlfbTris.push_back(tri);

			Matrix prod;
			product(prod, lat.getHMatrix(), plane.nullSpaceBasis);
			plane.neighborPlace.resize(lat.getNeighborCount());
			for (size_t gen = 0; gen < lat.getNeighborCount(); ++gen) {
			  mpq_class& value = prod(gen, 0);
			  NeighborPlace place = InPlane;
			  if (value < 0)
				place = UnderPlane;
			  else if (value > 0)
				place = OverPlane;
			  plane.neighborPlace[gen] = place;
			}

			transpose(plane.nullSpaceBasis);
		  }
		done:;
		}
	  }
	}
  }

  void printMlfbs(vector<Mlfb>& mlfbs, const Plane& plane, const GrobLat& lat) {
	cout << "\n\n";
	for (size_t i = 0; i < mlfbs.size(); ++i) {
	  Mlfb& mlfb = mlfbs[i];
	  cout << "*** MLFB m" << mlfb.id << " with rhs";
	  for (size_t var = 0; var < lat.getYDim(); ++var)
		cout << ' ' << mlfb.rhs[var];
	  cout << " contains the neighbors\n";

	  NeighborPrinter pr(lat);
	  pr.addZeroLine(InPlane, mlfb.getHitsNeighbor(0), mlfb.getEdge(0));
	  for (size_t i = 0; i < mlfb.points.size(); ++i) {
		pr.addLine(mlfb.points[i],
				   plane.neighborPlace[mlfb.points[i]],
				   mlfb.getHitsNeighbor(i + 1),
				   mlfb.getEdge(i + 1));
	  }
	  pr.print(cout);
	  cout << "Its index is " << mlfb.index << ", its rhs dot product is " <<
		mlfb.dotDegree << " and it has "
		   << mlfb.planeCount << " plane neighbors.\n\n";
	}
  }

  void setupPlaneCountsAndOrder(vector<Mlfb>& mlfbs, const Plane& plane, map<size_t, size_t>& typeCounts) {
	typeCounts.clear();

	for (size_t i = 0; i < mlfbs.size(); ++i) {
	  Mlfb& mlfb = mlfbs[i];
	  mlfb.planeCount = 1; // counting zero
	  for (size_t p = 0; p < mlfb.points.size(); ++p)
		if (plane.neighborPlace[mlfb.points[p]] == InPlane)
		  mlfb.planeCount += 1;
	  typeCounts[mlfb.planeCount] += 1;
	}
	sort(mlfbs.begin(), mlfbs.end());
  }

  size_t getMlfbWithId(const vector<Mlfb>& mlfbs, size_t id) {
	for (size_t i = 0; i < mlfbs.size(); ++i)
	  if (mlfbs[i].id == id)
		return i;
	return mlfbs.size();
  }

  size_t getCentralEdgeFacet(const vector<Mlfb>& mlfbs, size_t me, size_t other) {
	size_t c = 4;
	for (size_t to = 0; to < 4; ++to) {
	  const size_t e = getMlfbWithId(mlfbs, mlfbs[me].getEdge(to));
	  if (mlfbs[e].isFlat() || (e != other && mlfbs[e].isPivot())) {
		CHECK(c == 4);
		c = to;
	  }
	}
	CHECK(c != 4);
	return c;
  }

  void prSeq(const vector<Mlfb>& mlfbs, const vector<SeqPos>& seq) {
	cout << " Seq: ";
	for (size_t i = 0; i < seq.size(); ++i)
	  cout << (i > 0 ? "->" : "") << 'm' << mlfbs[seq[i].mlfb].id;
	cout << endl;
  }

  bool disjointSeqs(const vector<SeqPos>& a, const vector<SeqPos>& b) {
	for (size_t i = 0; i < a.size(); ++i)
	  for (size_t j = 0; j < b.size(); ++j)
		if (a[i].mlfb == b[j].mlfb)
		  return false;
	return true;
  }

  void getFlatSeq(size_t start1, size_t back1,
				  size_t start2, size_t back2,
				  size_t end1, size_t end2,
				  bool goEven,
				  vector<SeqPos>& seq, const vector<Mlfb>& mlfbs) {
	size_t next1 = getMlfbWithId(mlfbs, mlfbs[start1].getEdge(back1));
	size_t next2 = getMlfbWithId(mlfbs, mlfbs[start2].getEdge(back2));
	size_t prev1 = start1;
	size_t prev2 = start2;

	while (next1 == next2) {
	  size_t m = next1;
	  CHECK(mlfbs[m].isFlat());

	  SeqPos pos;
	  pos.mlfb = m;
	  seq.push_back(pos);

	  bool set1 = false;
	  for (size_t i = 0; i < 4; ++i) {
		size_t n = getMlfbWithId(mlfbs, mlfbs[m].getEdge(i));
		if (n != prev1 && n != prev2) {
		  if (!set1) {
			set1 = true;
			next1 = n;
		  } else
			next2 = n;
		}
	  }

	  prev1 = prev2 = m;
	}

	CHECK(next1 == end1 || next1 == end2);
	CHECK(next2 == end1 || next2 == end2);
  }

  void check0Graph(vector<Mlfb>& mlfbs) {
	vector<bool> ok(mlfbs.size());
	bool sawFlat = false;
	for (size_t i =0 ; i < mlfbs.size(); ++i) {
	  ok[i] = (mlfbs[i].index == 0);
	  sawFlat = true;
	}
	if (!sawFlat)
	  return;

	while (true) {
	  bool done = true;
	  for (size_t i = 0; i < mlfbs.size(); ++i) {
		if (!ok[i]) {
		  size_t to = getMlfbWithId(mlfbs, mlfbs[i].getEdge(0));
		  if (ok[to]) {
			done = false;
			ok[i] = true;
		  }
		}
	  }
	  if (done)
		break;
	}

	for (size_t i = 0; i < mlfbs.size(); ++i) {
	  CHECK(ok[i]);
	}
  }

  void printPlane(vector<Mlfb>& mlfbs, const Plane& plane, const GrobLat& lat) {
	map<size_t, size_t> typeCounts;
	setupPlaneCountsAndOrder(mlfbs, plane, typeCounts);

	cout << "The plane's null space is spanned by the rows of\n";
	printIndentedMatrix(plane.nullSpaceBasis);
	cout << '\n';

	for (size_t i = typeCounts.rbegin()->first; i > 0; --i)
	  cout << "There are " << typeCounts[i] << " MLFBs with " << i << " plane neighbors.\n";
	cout << '\n';

	cout << "The plane contains " << plane.nonMlfbTris.size()
		 << " non-MLFB double triangle pair(s):\n";
	NeighborPrinter pr(lat);
	for (size_t t = 0; t < plane.nonMlfbTris.size(); ++t) {
	  pr.addLine();
	  pr.addLine(plane.nonMlfbTris[t].a);
	  pr.addLine(plane.nonMlfbTris[t].b);
	  
	}
	pr.print(stdout);

	vector<size_t> pivots;
	for (size_t i = 0; i < mlfbs.size(); ++i)
	  if (mlfbs[i].isPivot())
		pivots.push_back(i);

	CHECK(plane.nonMlfbTris.size() == 1);

	if (typeCounts[0] == 0) {
	  cout << "skipping plane with no flats." << endl;
	  return;
	}

	CHECK(pivots.size() == 4);

	{
	  const Tri& tri = plane.nonMlfbTris[0];
	  size_t a = tri.a;
	  size_t b = tri.b;
	  size_t ab = lat.getSum(a, b);

	  bool found1 = false;
	  for (size_t i = 0; i < pivots.size(); ++i) {
		if (mlfbs[pivots[i]].hasPoint(a) && mlfbs[pivots[i]].hasPoint(ab)) {
		  CHECK(!found1);
		  found1 = true;
		  swap(pivots[0], pivots[i]);
		}
	  }
	  CHECK(found1);

	  bool found2 = false;
	  for (size_t i = 0; i < pivots.size(); ++i) {
		if (mlfbs[pivots[i]].hasPoint(b) && mlfbs[pivots[i]].hasPoint(ab)) {
		  CHECK(!found2);
		  found2 = true;
		  swap(pivots[1], pivots[i]);
		}
	  }
	  CHECK(found2);

	  if (mlfbs[pivots[0]].id > mlfbs[pivots[1]].id)
		swap(pivots[0], pivots[1]); // for determinism
	  if (mlfbs[pivots[2]].id > mlfbs[pivots[3]].id)
		swap(pivots[2], pivots[3]); // for determinism
	}
	cout << endl;

	for (size_t i = 0; i < 2; ++i) {
	  size_t a, b, c, d;
	  if (i == 0) {
		a = pivots[0];
		b = pivots[1];
		c = pivots[2];
		d = pivots[3];
	  } else {
		a = pivots[2];
		b = pivots[3];
		c = pivots[0];
		d = pivots[1];
	  }
	  size_t avoid = getCentralEdgeFacet(mlfbs, a, b);
	  size_t avoid2 = getCentralEdgeFacet(mlfbs, b, a);
	  CHECK(typeCounts[0] == 0 || mlfbs[a].getEdge(avoid) == mlfbs[b].getEdge(avoid2));

	  size_t x = (avoid <= 0 ? 1 : 0);
	  size_t y = (avoid <= 1 ? 2 : 1);
	  size_t z = (avoid <= 2 ? 3 : 2);

	  vector<SeqPos> seqs[4];
	  getSeq(a, x, y, z, mlfbs, seqs[0]);
	  getSeq(a, y, z, x, mlfbs, seqs[1]);
	  getSeq(a, z, x, y, mlfbs, seqs[2]);

	  cout << "The " << (i == 0 ? "left" : "right") << " sequences are:\n";
	  for (size_t j = 0; j < 3; ++j) {
		prSeq(mlfbs, seqs[j]);
		CHECK(seqs[j].front().mlfb == a);
		seqs[j].erase(seqs[j].begin());

		CHECK(seqs[j].back().mlfb == b);
		seqs[j].resize(seqs[j].size() - 1);
	  }

	  getFlatSeq(a, avoid,
				 b, avoid2,
				 c, d, i == 0,
				 seqs[3], mlfbs);

	  if (i == 0) {
		cout << "The flat sequence is:\n";
		prSeq(mlfbs, seqs[3]);
	  }

	  for (size_t i = 0; i < 4; ++i) {
		for (size_t j = 0; j < i; ++j) {
		  CHECK(disjointSeqs(seqs[i], seqs[j]));
		}
	  }
	  CHECK(seqs[0].size()+seqs[1].size()+seqs[2].size()+seqs[3].size()+4 == mlfbs.size());
	}

	check0Graph(mlfbs);

	/*
	cout << "\n\nmX(ABC) means we fix the neighbors on facet A and B while\n";
	cout << "we got to the current MLFB mX by pushing out facet C, so the\n";
	cout << "facet we will push in next is the remaining one. We track\n";
	cout << "where the neighbors go ignoring translation of the MLFB\n";
	cout << "so at the next step the facets ABC can have changed. We do not consider\n";
	cout << "two states m1(ABC) and m1(DEF) to be equal if AB != DE. If only C\n";
	cout << "and D differ its the same path in the opposite direction\n"
		 << "which we make sure not to list in both directions. We only list paths\n"
		 << "such that A and B are both on the thin plane or both off it.\n"
	  "3-1's and 1-3's are indicated by a * while flats are indicated by an F.\n";

	cout << "\nThe paths in the graph starting at 1-3's or 3-1's are:\n";

	set<SeqPos> seen;
	for (size_t i = 0; i < mlfbs.size(); ++i) {
	  if (mlfbs[i].planeCount != 1 && mlfbs[i].planeCount != 3)
		continue;
	  printSequences(mlfbs, plane, seen, i, true);
	}

	cout << "The remaining paths are:\n";
	for (size_t i = 0; i < mlfbs.size(); ++i)
	  printSequences(mlfbs, plane, seen, i, false);
	*/
	printMlfbs(mlfbs, plane, lat);
  }

  const char* getEdgePos(size_t index) {
	switch (index) {
	case 0: return "sw";
	case 1: return "se";
	case 2: return "ne";
	case 3: return "nw";
	default: ASSERT(false);
	  return "ERROR";
	}
  }

  void printScarfGraph(const vector<Mlfb>& mlfbs) {
	ofstream out("graph.dot");
	out << "graph G {\n";
	for (size_t m = 0; m < mlfbs.size(); ++m) {
	  const Mlfb& mlfb = mlfbs[m];
	  out << "  m" << mlfb.id << "[label=\"";
	  out << "m" << mlfb.id << "\\nindex " << mlfb.index;
	  out << "\", shape=box];\n";

	  for (size_t e = 0; e < mlfb.edges.size(); ++e) {
		size_t hits = mlfb.edgeHitsFacet[e];
		if (mlfb.id < mlfb.edges[e])
		  continue;

		out << "   m" << mlfb.id << " -- m" << mlfb.edges[e] << " [";
		out << "headport=" << getEdgePos(hits) << ", ";
		out << "tailport=" << getEdgePos(e) << "];\n";
	  }
	}
	out << "}\n";
  }

  void printMathematica3D(vector<Mlfb>& mlfbs, const GrobLat& lat) {
	ofstream out("ma3d");
	const Matrix& mat = lat.getHMatrix();
	out << "a={\n";
	for (size_t m = 0; m < mlfbs.size(); ++m) {
	  out << " Graphics3D[{RGBColor[";
	  for (size_t i = 0; i < 3; ++i) {
		if (i > 0)
		  out << ',';
		out << "0." << (rand() % 10000);
	  }
	  out << "],Polygon[{";

	  Mlfb& mlfb = mlfbs[m];
	  for (size_t p = 0; p < mlfb.points.size(); ++p) {
		if (p > 0)
		  out << ',';
		out << '{';
		for (size_t i = 0; i < lat.getHDim(); ++i) {
		  if (i > 0)
			out << ',';
		  out << mat(mlfb.points[p], i);
		}
		out << '}';
	  }
	  out << "}]}],\n";
	}
	out << " Graphics3D[Point[{0,0,0}]]\n};\ng=Show[{a},Boxed->False];\n";
  }

  mpq_class getIndexSum(const vector<Mlfb>& mlfbs) {
	mpq_class sum;
	for (size_t m = 0; m < mlfbs.size(); ++m)
	  sum += mlfbs[m].index;
	return sum;
  }
}

	  LatticeAnalyzeAction::LatticeAnalyzeAction():
	  Action
	  (staticGetName(),
	   "Display information about the input ideal.",
	   "This action is not ready for general use.\n\n"
	   "Display information about input Grobner basis of lattice.",
	   false),

	  _io(DataType::getSatBinomIdealType(), DataType::getMonomialIdealType()) {
	}

	void LatticeAnalyzeAction::obtainParameters(vector<Parameter*>& parameters) {
	  _io.obtainParameters(parameters);
	  Action::obtainParameters(parameters);
	}

	bool LatticeAnalyzeAction::displayAction() const {
	  return false;
	}

	void LatticeAnalyzeAction::perform() {
	  cerr << "** Reading input " << endl;

	  Scanner in(_io.getInputFormat(), stdin);
	  _io.autoDetectInputFormat(in);
	  _io.validateFormats();

	  IOFacade ioFacade(_printActions);
	  SatBinomIdeal ideal;
	  ioFacade.readSatBinomIdeal(in, ideal);

	  Matrix matrix;
	  {
		SatBinomIdeal matrixIdeal;
		ioFacade.readSatBinomIdeal(in, matrixIdeal);
		matrixIdeal.getMatrix(matrix);
	  }

	  cerr << "** Computing matrix and its nullspace" << endl;
	  cout << "Analysis of the "
		   << matrix.getRowCount() << " by " << matrix.getColCount()
		   << " matrix\n";
	  printIndentedMatrix(matrix);
	  printNullSpace(matrix);

	  cerr << "** Computing h-space vectors" << endl;
	  GrobLat lat(matrix, ideal);

	  if (lat.hasZeroEntryY()) {
		cout << "matrix not generic." << endl;
		exit(2);
	  }

	  cerr << "** Computing MLFBs" << endl;
	  vector<Mlfb> mlfbs;
	  computeMlfbs(mlfbs, lat);
	  stable_sort(mlfbs.begin(), mlfbs.end());

	  cerr << "** Computing double triangles" << endl;
	  vector<Plane> planes;
	  computePlanes(lat, planes);

	  size_t paraMlfbCount = 0;
	  for (size_t mlfb = 0; mlfb < mlfbs.size(); ++mlfb) {
		if (mlfbs[mlfb].index == 0)
		  ++paraMlfbCount;
	  }

	  cerr << "** Producing output" << endl;

	  if (lat.hasZeroEntryY())
		cout << "A neighbor has a zero entry.\n";
	  else
		cout << "No neighbor has a zero entry.\n";

	  cout << "There are " << lat.getNeighborCount() << " neighbors excluding zero.\n";
	  cout << "There are " << mlfbs.size() << " MLFBs.\n";
	  cout << "There are " << paraMlfbCount << " parallelogram MLFBs.\n";
	  cout << "There are " << planes.size()
		   << " distinct double triangle planes.\n";
	  cout << "The sum of MLFB indexes is " << getIndexSum(mlfbs) << ".\n";

	  printMinDotDegreeMlfb(mlfbs);

	  printNeighbors(lat);

	  for (size_t plane = 0; plane < planes.size(); ++plane) {
		cout << "\n\n*** Plane " << (plane + 1) << " of " << planes.size() << "\n\n";
		printPlane(mlfbs, planes[plane], lat);
	  }

	  printScarfGraph(mlfbs);
	  printMathematica3D(mlfbs, lat);
	}

	const char* LatticeAnalyzeAction::staticGetName() {
	  return "latanal";
	}
