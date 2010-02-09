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

#include <algorithm>
#include <set>
#include <sstream>

namespace {
  void printTriangles(const SatBinomIdeal& ideal) {
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
	  cout << "The ideal is not generic.\n";
	else
	  cout << "The ideal is generic.\n";
	size_t sumCount =
	  (ideal.getGeneratorCount() * (ideal.getGeneratorCount() - 1)) / 2;
	cout << "There are " << sums.size() << " distinct sums out of "
		 << sumCount << ".\n";
	cout << "There are " << tris.size() << " distinct double triangle sums.\n";\
	cout << "There are " << count << " double triangles.\n\n";

	cout << "The " << ideal.getGeneratorCount() << " neighbors are:\n";
	for (size_t g = 0; g < ideal.getGeneratorCount(); ++g) {
	  cout << " g" << g << ": ";
	  for (size_t var = 0; var < ideal.getVarCount(); ++var)
		cout << '\t' << ideal.getGenerator(g)[var];
	  cout << '\n';
	}

	cout << "\n\nThe " << count << " double triangles, grouped by sum:\n\n";

	for (map<string, string>::iterator it = tris.begin();
		 it != tris.end(); ++it)
	  cout << it->first << it->second << '\n';
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

  bool generic = ideal.hasZeroEntry();

  mpz_class triCount;
  ideal.getDoubleTriangleCount(triCount);
  fprintf(stdout, "%u neighbors, ", (unsigned int)ideal.getGeneratorCount());
  gmp_fprintf(stdout, "%Zd double triangles, ", triCount.get_mpz_t());

  if (!generic)
    fputs("generic.\n", stdout);
  else
    fputs("not generic.\n", stdout);

  printTriangles(ideal);

  if (triCount == 0)
    exit(1);
  if (generic)
    exit(2);
}

const char* LatticeAnalyzeAction::staticGetName() {
  return "latanal";
}
