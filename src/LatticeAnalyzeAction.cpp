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

	void addLine(Neighbor neighbor,
				 NeighborPlace place = NoPlace,
				 size_t hits = (size_t)-1,
				 Mlfb* edge = 0) {
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

	  if (hits != (size_t)-1) {
		_pr[_hitsHeader] << "hits";
		if (hits == 0)
		  _pr[_hits] << "zero";
		else
		  _pr[_hits] << 'g' << hits;
	  }
	  _pr[_hitsHeader] << '\n';
	  _pr[_hits] << '\n';

	  if (edge != 0) {
		_pr[_edgeHeader] << "push to";
		_pr[_edge] << edge->getName();
	  }
	  _pr[_edgeHeader] << '\n';
	  _pr[_edge] << '\n';
	}

	void addZeroLine(NeighborPlace place = NoPlace,
					 size_t hits = (size_t)-1,
					 Mlfb* edge = 0) {
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

	  if (edge != 0) {
		_pr[_edgeHeader] << "push to";
		_pr[_edge] << edge->getName();
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
	cout << "The MLFB " << min->getName() << " has minimal rhs dot product.\n";
  }

  void printNeighbors(const GrobLat& lat) {
	NeighborPrinter pr(lat);
	pr.addZeroLine();
	for (size_t n = 0; n < lat.getNeighborCount(); ++n)
	  pr.addLine(lat.getNeighbor(n));

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
	  cout << "*** MLFB " << mlfb.getName() << " with rhs";
	  for (size_t var = 0; var < lat.getYDim(); ++var)
		cout << ' ' << mlfb.getRhs()[var];
	  cout << " contains the neighbors\n";

	  NeighborPrinter pr(lat);
	  pr.addZeroLine(InPlane, mlfb.getHitsNeighbor(0), mlfb.getEdge(0));
	  for (size_t i = 1; i < mlfb.getPointCount(); ++i) {
		pr.addLine(mlfb.getPoint(i),
				   plane.getPlace(mlfb.getPoint(i)),
				   mlfb.getHitsNeighbor(i),
				   mlfb.getEdge(i));
	  }
	  pr.print(cout);
	  cout << "Its index is " << mlfb.index << ", its rhs dot product is " <<
		mlfb.dotDegree << " and it has "
		   << plane.getType(mlfb) << " plane neighbors.\n\n";
	}
  }

  void prSeq(const vector<SeqPos>& seq) {
	cout << " Seq: ";
	for (size_t i = 0; i < seq.size(); ++i)
	  cout << (i > 0 ? "->" : "") << seq[i].mlfb->getName();
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

	cout << "The plane contains " << plane.nonMlfbTris.size()
		 << " non-MLFB double triangle pair(s):\n";
	NeighborPrinter pr(lat);
	for (size_t t = 0; t < plane.nonMlfbTris.size(); ++t) {
	  pr.addLine();
	  pr.addLine(plane.nonMlfbTris[t].a);
	  pr.addLine(plane.nonMlfbTris[t].b);
	  
	}
	pr.print(stdout);

	printMlfbs(mlfbs, plane, lat);

	const vector<const Mlfb*>& pivots = plane.pivots;

	CHECK(plane.nonMlfbTris.size() == 1);
	CHECK(pivots.size() >= 4);

	if (flatSeq.size() == 0) {
	  CHECK(plane.getTypeCount(4) == 0);
	  cout << "Plane has no flats so can't "
		"compute or make sense of sequences (yet).\n";
	} else {
	  cout << endl;

	  vector<vector<SeqPos> > leftSeqs;
	  vector<vector<SeqPos> > rightSeqs;

	  computePivotSeqs(leftSeqs, *(pivots[0]), plane);
	  computePivotSeqs(rightSeqs, *(pivots[2]), plane);

	  cout << "The left sequences are:\n";
	  for (size_t j = 0; j < leftSeqs.size(); ++j)
		prSeq(leftSeqs[j]);
	  cout << "The flat sequence is:\n";
	  prSeq(flatSeq);
	  cout << "The right sequences are:\n";
	  for (size_t j = 0; j < rightSeqs.size(); ++j)
		prSeq(rightSeqs[j]);

	  checkPivotSeqs(leftSeqs, plane, mlfbs, flatSeq);
	  checkPivotSeqs(rightSeqs, plane, mlfbs, flatSeq);
	  checkNonMlfbTris(lat, mlfbs, pivots, plane.nonMlfbTris, plane);
	  checkFlatSeq(flatSeq, lat, plane);
	}

	check0Graph(mlfbs);
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

  cerr << "** Computing double triangles" << endl;
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

  cout << "There are " << lat.getNeighborCount() << " neighbors excluding zero.\n";
  cout << "There are " << mlfbs.size() << " MLFBs.\n";
  cout << "There are " << paraMlfbCount << " parallelogram MLFBs.\n";
  cout << "There are " << planes.size()
	   << " distinct double triangle planes.\n";
  cout << "The sum of MLFB indexes is " << getIndexSum(mlfbs) << ".\n";

  printMinDotDegreeMlfb(mlfbs);

  printNeighbors(lat);

  vector<size_t> nonSums;
  computeNonSums(nonSums, lat);
  cout << "The " << nonSums.size() << " non-sum neighbors are:";
  for (size_t i = 0; i < nonSums.size(); ++i)
	cout << ' ' << 'g' << (i+1);
  cout << endl;

  for (size_t plane = 0; plane < planes.size(); ++plane) {
	cout << "\n\n*** Plane " << (plane + 1) << " of " << planes.size() << "\n\n";
	printPlane(mlfbs, planes[plane], lat);
	checkPlane(planes[plane], mlfbs);
  }
  checkPlanes(planes);

  printScarfGraph(mlfbs);
  printMathematica3D(mlfbs, lat);

  checkNonSums(nonSums, lat);
}

const char* LatticeAnalyzeAction::staticGetName() {
  return "latanal";
}
