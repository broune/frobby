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
#ifndef PIVOT_EULER_ALG_GUARD
#define PIVOT_EULER_ALG_GUARD

#include <vector>
#include <cstdio>

class Ideal;
class RawSquareFreeIdeal;
class EulerState;

class PivotEulerAlg {
 public:
  PivotEulerAlg();

  mpz_class computeEulerCharacteristic(const Ideal& ideal);

  enum Alg {
	HybridAlg = 0,
	PivotAlg = 1,
	MayerVietorisAlg = 2
  };
  void setAlgorithm(Alg value) {_alg = value;}
  void setPrintStatistics(bool value, FILE* out) {
	_printStatistics = value;
	_statisticsOut = out;
  }
  void setUseUniqueDivSimplify(bool value) {_useUniqueDivSimplify = value;}
  void setUseManyDivSimplify(bool value) {_useManyDivSimplify = value;}
  void setUseAllPairsSimplify(bool value) {_useAllPairsSimplify = value;}

 private:
  bool processState(EulerState& state, EulerState& newState);
  void getPivot(const EulerState& state, Word* pivot);

  mpz_class _euler;
  Word* _termTmp;
  vector<size_t> _divCountsTmp;
  size_t _stepsPerformed;

  FILE* _statisticsOut;
  bool _printStatistics;
  bool _useUniqueDivSimplify;
  bool _useManyDivSimplify;
  bool _useAllPairsSimplify;
  bool _needDivCounts;
  Alg _alg;
};

#endif
