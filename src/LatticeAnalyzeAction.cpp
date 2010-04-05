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

#include <algorithm>
#include <set>
#include <sstream>
#include <limits>
#include <fstream>

/** @todo: do not use cout */
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

  void printNeighbors(const SatBinomIdeal& ideal, const Matrix& matrix) {
	ColumnPrinter pr;

	pr.addColumn(true, " ");
	for (size_t i = 0; i < ideal.getGeneratorCount(); ++i)
	  pr[0] << 'g' << i << ":\n";

	{
	  Matrix idealMatrix;
	  ideal.getMatrix(idealMatrix);

	  pr.addColumn(true, " y=");
	  print(pr, idealMatrix);

	  pr.addColumn(true, ",  h=");

	  Matrix transposedIdealMatrix;
	  transpose(transposedIdealMatrix, idealMatrix);

	  Matrix transposedMatrix;
	  transpose(transposedMatrix, matrix);

	  Matrix transposedHSpace;
	  solve(transposedHSpace, transposedMatrix, transposedIdealMatrix);
	  
	  Matrix hSpace;
	  transpose(hSpace, transposedHSpace);
	  print(pr, hSpace);
	}

	fprintf(stdout, "The %u neighbors in y-space and h-space are\n",
			(unsigned int)ideal.getGeneratorCount());
	print(stdout, pr);
	fputc('\n', stdout);
  }

  void printTriangles(const SatBinomIdeal& ideal, const Matrix& matrix) {
	map<string, string> tris;
	set<string> sums;
	vector<mpz_class> sum(ideal.getVarCount());

	size_t count = 0;
	for (size_t gen1 = 0; gen1 < ideal.getGeneratorCount(); ++gen1) {
	  for (size_t gen2 = gen1 + 1; gen2 < ideal.getGeneratorCount(); ++gen2) {
		const vector<mpz_class>& g1 = ideal.getGenerator(gen1);
		const vector<mpz_class>& g2 = ideal.getGenerator(gen2);

		// Set sum = g1 + g2.
		for (size_t var = 0; var < ideal.getVarCount(); ++var)
		  sum[var] = g1[var] + g2[var];

		ostringstream sumStr;
		sumStr << "sum:";
		for (size_t var = 0; var < ideal.getVarCount(); ++var)
		  sumStr << ' ' << sum[var];
		sumStr << '\n';
		sums.insert(sumStr.str());
		if (ideal.isPointFreeBody(g1, sum) && ideal.isPointFreeBody(g2, sum)) {
		  ostringstream tmp;
		  tmp << tris[sumStr.str()] << " g" << gen1 << " and g" << gen2 << '\n';
		  tris[sumStr.str()] = tmp.str();
		  ++count;
		}
	  }
	}

	if (ideal.hasZeroEntry())
	  cout << "A neighbor has a zero entry.\n";
	else
	  cout << "No neighbor has a zero entry.\n";
	size_t sumCount =
	  (ideal.getGeneratorCount() * (ideal.getGeneratorCount() - 1)) / 2;
	cout << "There are " << ideal.getGeneratorCount() << " neighbors.\n";
	cout << "There are " << sums.size() << " distinct neighbor sums out of "
		 << sumCount << ".\n";
	cout << "There are " << tris.size() << " distinct double triangle sums.\n";\
	cout << "There are " << count << " double triangles.\n\n";

	printNeighbors(ideal, matrix);

	cout << "\n\nThe " << count << " double triangles, grouped by sum:\n\n";

	for (map<string, string>::iterator it = tris.begin();
		 it != tris.end(); ++it)
	  cout << it->first << it->second << '\n';
  }

  void writeScarfGarph(const SatBinomIdeal& ideal) {
	ofstream out("graph.dot");
	out << "digraph G {\n" << flush;
	for (size_t gen1 = 0; gen1 < ideal.getGeneratorCount(); ++gen1) {
	  size_t gen2 = ideal.getInteriorEdgeFrom(gen1);
	  out << "  g" << gen1 << " [label=\"g" << gen1;
	  if (gen2 == numeric_limits<size_t>::max()) {
		out << " (no interior pair)\"];\n";
		continue;
	  }

	  const vector<mpz_class>& g1 = ideal.getGenerator(gen1);
	  const vector<mpz_class>& g2 = ideal.getGenerator(gen2);

	  out << ", g" << gen2 << "\\n" << g1[0] << '\"';

	  vector<mpz_class> sum(g1.size());
	  for (size_t var = 0; var < g1.size(); ++var)
		sum[var] = g1[var] + g2[var];

	  if (ideal.isPointFreeBody(g1, sum))
		out << ",shape=box";

	  out << "];\n";
	  out << "  g" << gen1 << " -> g" << gen2 << ";\n";	
	}
	out << "}\n";
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

  mpz_class triCount;
  ideal.getDoubleTriangleCount(triCount);

  cout << "Analysis of the "
	   << matrix.getRowCount() << " by " << matrix.getColCount() 
	   << " matrix\n";
  printIndentedMatrix(matrix);
  printNullSpace(matrix);

  printTriangles(ideal, matrix);

  writeScarfGarph(ideal);
}

const char* LatticeAnalyzeAction::staticGetName() {
  return "latanal";
}
