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
#include <map>

#include "LatticeAlgs.h"

/**
This file has stuff I'm using in my work with Scarf. It's just
whatever code gets the job done so we can figure out the structure of
these lattices. -Bjarke H. Roune

@todo: do not use cout

@todo: move functionality into proper classes away from this file. */
#include <iostream>

namespace {

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

	void addNeighbor(Neighbor neighbor) {
	  printNeighborOnly(neighbor);
	  _pr[_hitsHeader] << '\n';
	  _pr[_hits] << '\n';
	  _pr[_edgeHeader] << '\n';
	  _pr[_edge] << '\n';
	}

	void addMlfbPoint(Neighbor neighbor,
					  const Plane& plane,
					  Neighbor hits,
					  Mlfb& edge) {
	  printNeighborOnly(neighbor, plane.getPlace(neighbor));
	  _pr[_hitsHeader] << "hits\n";
	  _pr[_hits] << hits.getName() << '\n';
	  _pr[_edgeHeader] << "push to\n";
	  _pr[_edge] << edge.getName(plane) << '\n';
	}

	void addEmptyLine() {
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
	void printNeighborOnly(Neighbor neighbor, NeighborPlace place = NoPlace) {
	  _pr[_labelIndex] << neighbor.getName() << ':';
	  if (place != NoPlace)
		_pr[_labelIndex] << ' ' << getPlaceCode(place);
	  _pr[_labelIndex] << '\n';

	  _pr[_yHeader] << "y=\n";
	  for (size_t i = 0; i < neighbor.getYDim(); ++i)
		_pr[_yIndex + i] << neighbor.getY(i) << '\n';
	  _pr[_comma] << ",\n";
	  _pr[_hHeader] << "h=\n";
	  for (size_t i = 0; i < neighbor.getHDim(); ++i)
		_pr[_hIndex + i] << neighbor.getH(i) << '\n';
	}

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
	cout << "The MLFB " << min->getName() << " has minimal rhs dot product.\n";
  }

  void printNeighbors(const GrobLat& lat) {
	NeighborPrinter pr(lat);
	pr.addNeighbor(Neighbor(lat));
	for (size_t n = 0; n < lat.getNeighborCount(); ++n)
	  pr.addNeighbor(lat.getNeighbor(n));

	fprintf(stdout, "\nThe %u neighbors in y-space and h-space are\n",
			(unsigned int)lat.getNeighborCount());
	pr.print(stdout);
	fputc('\n', stdout);
  }

  class MlfbPtrCmp {
  public:
	MlfbPtrCmp(const Plane& plane): _plane(plane) {}
	bool operator()(const Mlfb* a, const Mlfb* b) {
	  size_t ta = _plane.getType(*a);
	  size_t tb = _plane.getType(*b);
	  if (ta == 1) ta = 3;
	  if (tb == 1) tb = 3;
	  if (ta != tb)
		return ta > tb;
	  else
		return a->getOffset() < b->getOffset();
	}

  private:
	const Plane& _plane;
  };

  void printMlfbs(vector<Mlfb>& mlfbs, const Plane& plane, const GrobLat& lat) {
	vector<Mlfb*> order;
	for (size_t i = 0; i < mlfbs.size(); ++i)
	  order.push_back(&(mlfbs[i]));
	sort(order.begin(), order.end(), MlfbPtrCmp(plane));

	cout << "\n\n";
	for (size_t i = 0; i < order.size(); ++i) {
	  Mlfb& mlfb = *(order[i]);
	  cout << "*** MLFB " << mlfb.getName(plane) << " with rhs";
	  for (size_t var = 0; var < lat.getYDim(); ++var)
		cout << ' ' << mlfb.getRhs()[var];
	  cout << " contains the neighbors\n";

	  NeighborPrinter pr(lat);
	  for (size_t j = 0; j < mlfb.getPointCount(); ++j) {
		pr.addMlfbPoint(mlfb.getPoint(j),
						plane,
						mlfb.getHitsNeighbor(j),
						*mlfb.getEdge(j));
	  }
	  pr.print(cout);
	  cout << "Its index is " << mlfb.index << ", its rhs dot product is " <<
		mlfb.dotDegree << " and it has "
		   << plane.getType(mlfb) << " plane neighbors.\n\n";
	}
  }

  void prSeq(const vector<SeqPos>& seq, const Plane& plane) {
	cout << " Seq: ";
	for (size_t i = 0; i < seq.size(); ++i)
	  cout << (i > 0 ? "->" : "") << seq[i].mlfb->getName(plane);
	cout << endl;
  }

  void printPlane(vector<Mlfb>& mlfbs, const Plane& plane, const GrobLat& lat) {
	cout << "The plane's null space is spanned by the rows of\n";
	printIndentedMatrix(plane.nullSpaceBasis);
	cout << '\n';

	const vector<SeqPos>& flatSeq = plane.flatSeq;

	cout << "The flat sequence has " << plane.flatIntervalCount
		 << (plane.flatIntervalCount == 1 ? " interval.\n" : " intervals.\n");

	for (size_t i = plane.getMaxType(); i > 0; --i)
	  cout << "There are " << plane.getTypeCount(i)
		   << " MLFBs with " << i << " plane neighbors.\n";
	cout << '\n';

	const Tri& tri = plane.tri;
	cout << "The non-flat triangle of the plane is "
		 << "{zero,"
		 << tri.getA().getName() << ',' << tri.getSum().getName() << "},"
		 << "{zero,"
		 << tri.getB().getName() << ',' << tri.getSum().getName() << "}.";

	cout << "\nThe MLFBs containing {zero,"
		 << tri.getA().getName() << ',' << tri.getSum().getName() << "} are:";
	for (size_t i = 0; i < tri.getASideMlfbs().size(); ++i)
	  cout << ' ' << tri.getASideMlfbs()[i]->getName(plane);

	cout << "\nThe MLFBs containing {zero,"
		 << tri.getB().getName() << ',' << tri.getSum().getName() << "} are:";
	for (size_t i = 0; i < tri.getBSideMlfbs().size(); ++i)
	  cout << ' ' << tri.getBSideMlfbs()[i]->getName(plane);

	cout << "\nThe neighbors on the boundary of the body defined by {zero,"
		 << tri.getA().getName() << ','
		 << tri.getB().getName() << ','
		 << tri.getSum().getName() << "} are\n";
	{
	  NeighborPrinter pr(lat);
	  for (size_t i = 0; i < tri.getNeighborsOnBoundary().size(); ++i)
		pr.addNeighbor(tri.getNeighborsOnBoundary()[i]);
	  pr.print(stdout);
	}
	cout << "and those in the interior are\n";
	{
	  NeighborPrinter pr(lat);
	  for (size_t i = 0; i < tri.getNeighborsInInterior().size(); ++i)
		pr.addNeighbor(tri.getNeighborsInInterior()[i]);
	  pr.print(stdout);
	}


	printMlfbs(mlfbs, plane, lat);

	const vector<const Mlfb*>& pivots = plane.pivots;

	CHECK((pivots.size() % 2) == 0);

	CHECK(!(tri.getASideMlfbs().size() == 2 &&
			tri.getBSideMlfbs().size() == 2 &&
			pivots.size() == 4 &&
			flatSeq.size() > 0));

	if (flatSeq.size() == 0 || pivots.size() != 4) {
	  cout << "(Falling back to general framework as no flats or more than 4 pivots)\n\n";

	  vector<vector<SeqPos> > left;
	  vector<vector<SeqPos> > right;
	  computeSeqs(left, right, mlfbs, plane);

	  cout << "The left sequences are:\n";
	  for (size_t j = 0; j < left.size(); ++j)
		prSeq(left[j], plane);

	  cout << "The right sequences are:\n";
	  for (size_t j = 0; j < right.size(); ++j)
		prSeq(right[j], plane);

	  checkSeqs(left, right, plane, mlfbs);
	  checkMiddle(plane, mlfbs);
	  checkGraphOnPlane(plane, mlfbs);
	  checkDoubleTriangle(plane, mlfbs);
	} else {
	  cout << endl;

	  vector<vector<SeqPos> > leftSeqs;
	  vector<vector<SeqPos> > rightSeqs;

	  computePivotSeqs(leftSeqs, *(pivots[0]), plane);
	  computePivotSeqs(rightSeqs, *(pivots[2]), plane);

	  cout << "The left sequences are:\n";
	  for (size_t j = 0; j < leftSeqs.size(); ++j)
		prSeq(leftSeqs[j], plane);
	  cout << "The flat sequence is:\n";
	  prSeq(flatSeq, plane);
	  cout << "The right sequences are:\n";
	  for (size_t j = 0; j < rightSeqs.size(); ++j)
		prSeq(rightSeqs[j], plane);

	  checkPivotSeqs(leftSeqs, plane, mlfbs, flatSeq);
	  checkPivotSeqs(rightSeqs, plane, mlfbs, flatSeq);
	  checkPlaneTri(lat, mlfbs, pivots, plane);
	  checkFlatSeq(flatSeq, lat, plane);
	}
  }

  void printInteriorNeighborGraph(const GrobLat& lat,
								  const vector<Mlfb>& mlfbs,
                                  const vector<Plane>& planes) {
    ofstream out("interior.dot");
    out << "digraph G {\n" << flush;
    for (size_t gen1 = 0; gen1 < lat.getNeighborCount(); ++gen1) {
      Neighbor from = lat.getNeighbor(gen1);
      out << ' ' << from.getName() << ";\n";
      for (size_t gen2 = 0; gen2 < lat.getNeighborCount(); ++gen2) {
        if (gen1 == gen2)
          continue;
        Neighbor to = lat.getNeighbor(gen2);
        Neighbor sum = lat.getSum(from, to);
        if (!sum.isValid() || !lat.isInterior(to, sum))
          continue;
        out << "  " << from.getName() << " -> " << sum.getName()
			<< " [label=\"" << to.getName();
		for (size_t m = 0; m < mlfbs.size(); ++m) {
		  const Mlfb& mlfb = mlfbs[m];
		  if (mlfb.hasPoint(to) && mlfb.hasPoint(sum)) {
            out << ' ' << mlfb.getName();
          }
        }
        if (lat.isPointFreeBody(from, sum)) {
          for (size_t p = 0; p < planes.size(); ++p) {
            const Plane& plane = planes[p];
            if (plane.inPlane(sum) && plane.inPlane(to) && plane.inPlane(from))
              out << " p" << (p + 1);
          }
          out << "\",style=dotted,arrowhead=empty";
        } else
          out << "\"";
        out << "];\n";
      }
    }
    out << "}\n";
  }

  void printScarfGraph(const vector<Mlfb>& mlfbs) {
	ofstream out("graph.dot");
	out << "graph G {\n";
	for (size_t m = 0; m < mlfbs.size(); ++m) {
	  const Mlfb& mlfb = mlfbs[m];
	  out << "  " << mlfb.getName() << "[label=\"";
	  out << mlfb.getName() << "\\nindex " << mlfb.index;
	  out << "\", shape=box];\n";

	  for (size_t e = 0; e < mlfb.edges.size(); ++e) {
		size_t hits = mlfb.edgeHitsFacet[e];
		if (mlfb.getOffset() < mlfb.edges[e]->getOffset())
		  continue;

		out << "   " << mlfb.getName()
			<< " -- " << mlfb.edges[e]->getName() << " [";
		out << "headport=" << getEdgePos(hits) << ", ";
		out << "tailport=" << getEdgePos(e) << "];\n";
	  }
	}
	out << "}\n";
  }

  void printMathematica3D(vector<Mlfb>& mlfbs, const GrobLat& lat) {
	ofstream out("ma3d");
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
	  for (size_t p = 1; p < mlfb.getPointCount(); ++p) {
		if (p > 1)
		  out << ',';
		out << '{';
		for (size_t i = 0; i < lat.getHDim(); ++i) {
		  if (i > 1)
			out << ',';
		  out << mlfb.getPoint(p).getH(i);
		}
		out << '}';
	  }
	  out << "}]}],\n";
	}
	out << " Graphics3D[Point[{0,0,0}]]\n};\ng=Show[{a},Boxed->False];\n";
  }

  void printThinPlanes(const vector<TriPlane>& thinPlanes) {
	Matrix matrix(thinPlanes.size(), 3);

	for (size_t p = 0; p < thinPlanes.size(); ++p)
	  copyRow(matrix, p, thinPlanes[p].getNormal(), 0);

	cout << "The " << thinPlanes.size() << " thin planes' normals are the rows of" << endl;
	cout << matrix;
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

  cerr << "** Computing thin planes" << endl;
  vector<TriPlane> thinPlanes;
  getThinPlanes(thinPlanes, lat);

  cerr << "** Computing double triangle planes and associated structures" << endl;
  vector<Plane> planes;
  computePlanes(planes, lat, mlfbs);

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

  mpq_class indexSum = getIndexSum(mlfbs);

  cout << "There are " << lat.getNeighborCount() << " neighbors excluding zero.\n";
  cout << "There are " << mlfbs.size() << " MLFBs.\n";
  cout << "There are " << paraMlfbCount << " parallelogram MLFBs.\n";
  cout << "There are " << planes.size()
	   << " distinct double triangle planes.\n";
  cout << "The sum of MLFB indexes is " << indexSum << ".\n";

  CHECK(indexSum == 6 || indexSum == -6);

  printMinDotDegreeMlfb(mlfbs);

  printNeighbors(lat);

  const vector<Neighbor>& nonSums = lat.getNonSums();
  cout << "The " << nonSums.size() << " non-sum neighbors are:";
  for (size_t i = 0; i < nonSums.size(); ++i)
	cout << ' ' << nonSums[i].getName();
  cout << endl;

  printThinPlanes(thinPlanes);

  for (size_t plane = 0; plane < planes.size(); ++plane) {
	cout << "\n\n*** Plane " << (plane + 1)
		 << " of " << planes.size() << "\n\n";
	printPlane(mlfbs, planes[plane], lat);
	checkPlane(planes[plane], mlfbs);
  }
  checkMlfbs(mlfbs, lat);
  checkDoubleTrianglePlanes(planes, lat, mlfbs);

  checkPlanes(thinPlanes, planes);

  printInteriorNeighborGraph(lat, mlfbs, planes);
  printScarfGraph(mlfbs);
  printMathematica3D(mlfbs, lat);

  checkNonSums(lat);
  checkGraph(mlfbs);
}

const char* LatticeAnalyzeAction::staticGetName() {
  return "latanal";
}
