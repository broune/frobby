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

  private:
    Matrix _y; // rows are neighbors in y-space
    Matrix _h; // rows are neighbors in h-space
    Matrix _mat; // matrix that defines lattice
    SatBinomIdeal _ideal;
  };

  void printNeighbors(const GrobLat& lat) {
    ColumnPrinter pr;

    pr.addColumn(true, " ");
    for (size_t i = 0; i < lat.getNeighborCount(); ++i)
      pr[0] << 'g' << i << ":\n";

    pr.addColumn(true, " y=");
    print(pr, lat.getYMatrix());

    pr.addColumn(true, ",  h=");
    print(pr, lat.getHMatrix());

    fprintf(stdout, "The %u neighbors in y-space and h-space are\n",
	    (unsigned int)lat.getNeighborCount());
    print(stdout, pr);
    fputc('\n', stdout);
  }

  struct Tri {
    size_t a;
    size_t b;
    Matrix rowAB;
  };

  void makeTrianglesAndSums(const GrobLat& lat,
			    set<string>& sums,
			    vector<vector<Tri> >& tris) {
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

	ostringstream sumStr;
	sumStr << "sum:";
	for (size_t var = 0; var < varCount; ++var)
	  sumStr << ' ' << sum[var];
	sumStr << '\n';
	sums.insert(sumStr.str());
	if (ideal.isPointFreeBody(g1, sum) && ideal.isPointFreeBody(g2, sum)) {
	  Tri tri;
	  tri.a = gen1;
	  tri.b = gen2;
	  tri.rowAB.resize(2, lat.getHDim());
	  copyRow(tri.rowAB, 0, lat.getHMatrix(), gen1);
	  copyRow(tri.rowAB, 1, lat.getHMatrix(), gen2);

	  for (size_t plane = 0; plane < tris.size(); ++plane) {
	    ASSERT(!tris[plane].empty());
	    if (hasSameRowSpace(tri.rowAB, tris[plane][0].rowAB)) {
	      tris[plane].push_back(tri);
	      goto done;
	    }
	  }
	  tris.resize(tris.size() + 1);
	  tris.back().push_back(tri);
	done:;
	}
      }
    }
  }

  void printTriangles(const GrobLat& lat) {
    set<string> sums;
    vector<mpz_class> sum(lat.getYDim());
    vector<vector<Tri> > tris;

    makeTrianglesAndSums(lat, sums, tris);

    size_t triCount = 0;
    for (size_t plane = 0; plane < tris.size(); ++plane)
      triCount += tris[plane].size();

    if (lat.hasZeroEntryY())
      cout << "A neighbor has a zero entry.\n";
    else
      cout << "No neighbor has a zero entry.\n";

    size_t sumCount =
      (lat.getNeighborCount() * (lat.getNeighborCount() - 1)) / 2;
    cout << "There are " << lat.getNeighborCount() << " neighbors.\n";
    cout << "There are " << sums.size() << " distinct neighbor sums out of "
	 << sumCount << ".\n";
    cout << "There are " << tris.size() << " distinct double triangle planes.\n";\
    cout << "There are " << triCount << " double triangles.\n\n";

    printNeighbors(lat);

    cout << "\n\nThe " << triCount << " double triangles, grouped by linear span:\n\n";

    for (size_t plane = 0; plane < tris.size(); ++plane) {
      vector<Tri>& p = tris[plane];
      cout << "*** Plane " << (plane + 1)
	   << " has orthogonal space spanned by the rows of\n";

      {
	Matrix nullSpaceBasis;
	nullSpace(nullSpaceBasis, p.front().rowAB);
	transpose(nullSpaceBasis);
	printIndentedMatrix(nullSpaceBasis);
      }
      
      cout << " and contains " << p.size() << " double triangle pairs:" << endl;
      for (size_t t = 0; t < p.size(); ++t) {
	cout << "  Pair of g" << p[t].a << " and g" << p[t].b << ":";
	printIndentedMatrix(p[t].rowAB);
      }
    }
  }

  void writeScarfGraph(const SatBinomIdeal& ideal) {
    ofstream out("graph.dot");
    out << "digraph G {\n" << flush;
    for (size_t gen1 = 0; gen1 < ideal.getGeneratorCount(); ++gen1) {
      const vector<mpz_class>& g1 = ideal.getGenerator(gen1);
      out << "  g" << gen1 << " [label=\"g" << gen1
	  << " (" << g1[0].get_d() << ')';
      if (ideal.isInterior(g1, g1))
	out << "\\ninterior";
      out << "\", shape = box];\n";

      for (size_t gen2 = 0; gen2 < ideal.getGeneratorCount(); ++gen2) {
	if (ideal.isInteriorEdge(gen1, gen2)) {
	  out << "    g" << gen1 << " -> g" << gen2;
	  if (ideal.isTerminatingEdge(gen1, gen2))
	    out << " [style = dashed, arrowhead = empty]";
	  out << ";\n";
	}
      }
    }
    out << "}\n";
  }

  void writeScarfComplex(const SatBinomIdeal& latticeIdeal) {
    BigIdeal bigIdeal;
    latticeIdeal.getInitialIdeal(bigIdeal);

    BigTermRecorder recorder;
    SliceParams params;
    SliceFacade facade(params, bigIdeal, recorder);
    facade.computeIrreducibleDecomposition(true);
    auto_ptr<BigIdeal> mlfbs = recorder.releaseIdeal();
    ASSERT(recorder.empty());


    const size_t varCount = bigIdeal.getVarCount();
    ColumnPrinter pr;
    for (size_t var = 0; var < varCount; ++var)
      pr.addColumn();
    pr.addColumn(true, "  is rhs of mlfb  ");

    for (size_t mlfb = 0; mlfb < mlfbs->getGeneratorCount(); ++mlfb)
      for (size_t var = 0; var < varCount; ++var)
	pr[var] << (*mlfbs)[mlfb][var] << '\n';

    for (size_t mlfb = 0; mlfb < mlfbs->getGeneratorCount(); ++mlfb) {
      size_t offset = varCount;
      for (size_t gen = 0; gen < bigIdeal.getGeneratorCount(); ++gen) {
	for (size_t var = 0; var < bigIdeal.getVarCount(); ++var)
	  if (bigIdeal[gen][var] > (*mlfbs)[mlfb][var])
	    goto skipIt1;
	if (offset >= pr.getColumnCount())
	  pr.addColumn(true, "  ", "");
	++offset;
      skipIt1:;
      }
    }

    for (size_t mlfb = 0; mlfb < mlfbs->getGeneratorCount(); ++mlfb) {
      size_t offset = varCount;
      for (size_t gen = 0; gen < bigIdeal.getGeneratorCount(); ++gen) {
	for (size_t var = 0; var < bigIdeal.getVarCount(); ++var)
	  if (bigIdeal[gen][var] > (*mlfbs)[mlfb][var])
	    goto skipIt2;
	pr[offset] << 'g' << gen << '\n';
	++offset;
      skipIt2:;
      }
      for (; offset < pr.getColumnCount(); ++offset)
	pr[offset] << '\n';
    }

    cout << "\n\nThe " << mlfbs->getGeneratorCount() << " MLFBs in the Scarf complex are\n" << pr;
  }
}

    LatticeAnalyzeAction::LatticeAnalyzeAction():
    Action
    (staticGetName(),
     "Display information about the input ideal.",
     "This action is not ready for use.\n\n"
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

  cerr << "** Counting double triangles" << endl;
  mpz_class triCount;
  ideal.getDoubleTriangleCount(triCount);

  cerr << "** Computing matrix and it's nullspace" << endl;
  cout << "Analysis of the "
       << matrix.getRowCount() << " by " << matrix.getColCount() 
       << " matrix\n";
  printIndentedMatrix(matrix);
  printNullSpace(matrix);

  cerr << "** Computing h-space vectors" << endl;
  GrobLat lat(matrix, ideal);

  cerr << "** Computing double triangles" << endl;
  printTriangles(lat);

  //cerr << "** Computing Scarf graph" << endl;
  //writeScarfGraph(ideal);

  cerr << "** Computing Scarf complex" << endl;
  writeScarfComplex(ideal);
}

const char* LatticeAnalyzeAction::staticGetName() {
  return "latanal";
}
