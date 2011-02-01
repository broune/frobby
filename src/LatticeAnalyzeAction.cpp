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

/** @todo: do not use cout

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
	vector<size_t> points;
	vector<size_t> edges;
	vector<size_t> edgeHitsFacet;
	vector<mpz_class> rhs;
	size_t planeCount;
	size_t id;

	bool operator<(const Mlfb& mlfb) const {
	  if (planeCount > mlfb.planeCount)
		return true;
	  if (planeCount < mlfb.planeCount)
		return false;

	  return points.back() > mlfb.points.back();
	}

	size_t getEdge(size_t index) {
	  if (index < edges.size())
		return edges[index];
	  else
		return numeric_limits<size_t>::max();
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
	  Matrix mat(mlfb.points.size(), lat.getHDim());
	  for (size_t point = 0; point < mlfb.points.size(); ++point)
		for (size_t var = 0; var < lat.getHDim(); ++var)
		  mat(point, var) = lat.getHMatrix()(mlfb.points[point], var);
	  if (mlfb.points.size() == lat.getHDim())
		mlfb.index = determinant(mat);
	}

	// Compute Scarf edges.
	for (size_t m = 0; m < mlfbs.size(); ++m) {
	  Mlfb& mlfb = mlfbs[m];
	  if (mlfb.points.size() != lat.getYDim() - 1)
		continue;

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

	  // edges
	  _edgeHeader = _pr.getColumnCount();
	  _pr.addColumn(false, "  ", "");
	  _edge = _pr.getColumnCount();
	  _pr.addColumn(false, " ", "");
	}

	void addLine(size_t neighbor,
				 NeighborPlace place = NoPlace,
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

	  if (edge != (size_t)-1) {
		_pr[_edgeHeader] << "push to";
		_pr[_edge] << 'm' << edge;
	  }
	  _pr[_edgeHeader] << '\n';
	  _pr[_edge] << '\n';
	}

	void addZeroLine(NeighborPlace place = NoPlace,
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
	size_t _edgeHeader;
	size_t _edge;
  };

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
  };

  struct Plane {
	Matrix nullSpaceBasis;
	vector<Tri> tris;
	Matrix rowAB;

	vector<NeighborPlace> neighborPlace;
  };

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
		  Matrix rowAB(2, lat.getHDim());
		  copyRow(rowAB, 0, lat.getHMatrix(), gen1);
		  copyRow(rowAB, 1, lat.getHMatrix(), gen2);

		  for (size_t plane = 0; plane < planes.size(); ++plane) {
			ASSERT(!planes[plane].tris.empty());
			if (hasSameRowSpace(rowAB, planes[plane].rowAB)) {
			  planes[plane].tris.push_back(tri);
			  goto done;
			}
		  }

		  {
			planes.resize(planes.size() + 1);
			Plane& plane = planes.back();
			plane.rowAB = rowAB;
			nullSpace(plane.nullSpaceBasis, rowAB);
			plane.tris.push_back(tri);

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
	  pr.addZeroLine(InPlane, mlfb.getEdge(0));
	  for (size_t i = 0; i < mlfb.points.size(); ++i) {
		pr.addLine(mlfb.points[i],
				   plane.neighborPlace[mlfb.points[i]],
				   mlfb.getEdge(i + 1));
	  }
	  pr.print(cout);
	  cout << "Its index is " << mlfb.index << " and it has "
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

  void printPlane(vector<Mlfb>& mlfbs, const Plane& plane, const GrobLat& lat) {
	map<size_t, size_t> typeCounts;
	setupPlaneCountsAndOrder(mlfbs, plane, typeCounts);

	cout << "The plane's null space is spanned by the rows of\n";
	printIndentedMatrix(plane.nullSpaceBasis);
	cout << '\n';

	for (size_t i = typeCounts.rbegin()->first; i > 0; --i)
	  cout << "There are " << typeCounts[i] << " MLFBs with " << i << " plane neighbors.\n";
	cout << '\n';

	cout << "The plane contains " << plane.tris.size() << " double triangle pairs:\n";
	NeighborPrinter pr(lat);
	for (size_t t = 0; t < plane.tris.size(); ++t) {
	  pr.addLine();
	  pr.addLine(plane.tris[t].a);
	  pr.addLine(plane.tris[t].b);
	}
	pr.print(stdout);

	printMlfbs(mlfbs, plane, lat);
  }
  /*
  const char* getEdgePos(size_t index) {
	switch (index) {
	case 0: return "s";
	case 1: return "e";
	case 2: return "n";
	case 3: return "w";
	default: ASSERT(false);
	  return "ERROR";
	}
  }
*/
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
