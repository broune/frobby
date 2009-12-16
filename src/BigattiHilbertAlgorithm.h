/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2009 University of Aarhus
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
#ifndef BIGATTI_HILBERT_ALGORITHM_GUARD
#define BIGATTI_HILBERT_ALGORITHM_GUARD

#include "TaskEngine.h"
#include "HashPolynomial.h"
#include "Ideal.h"
#include "ObjectCache.h"
#include "BigattiState.h"

class CoefTermConsumer;
class Term;

class BigattiHilbertAlgorithm {
public:
	BigattiHilbertAlgorithm(CoefTermConsumer* consumer);

	void run(const Ideal& ideal);

private:
	void processState(auto_ptr<BigattiState> state);
	bool baseCase(const BigattiState& state);
	void getPivot(BigattiState& state, Term& pivot);
	void simplify(BigattiState& state);

	void allCombinationsBaseCase(const BigattiState& state);
	void basecase(Ideal::const_iterator begin, Ideal::const_iterator end,
				  bool plus, const Term& term, const Term& multiply);
	void freeState(auto_ptr<BigattiState> state);

	size_t _varCount;
	CoefTermConsumer* _consumer;
    HashPolynomial _output;
	TaskEngine _tasks;
	ObjectCache<BigattiState> _stateCache;

	vector<Term> _tmp_allCombinationsBaseCase_lcms;
	vector<int> _tmp_allCombinationsBaseCase_included;
	Term _tmp_allCombinationsBaseCase_term;
	Term _tmp_processState_pivot;
	Term _tmp_getPivot_counts;
	Term _tmp_simplify_gcd;
	Term _tmp_baseCase_lcm;
	Term _tmp_baseCase_maxCount;

	friend class BigattiState;
};

#endif
