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
#ifndef BIGATTI_HILBERT_ALGORITHM
#define BIGATTI_HILBERT_ALGORITHM

#include "Polynomial.h"
#include "Ideal.h"

class CoefTermConsumer;
class Term;


class BigattiHilbertAlgorithm {
public:
	BigattiHilbertAlgorithm(CoefTermConsumer* consumer);

	void run(const Ideal& ideal);

private:
	void hilbert(const Ideal& ideal, const Term& term);
	bool isBaseCase(const Ideal& ideal);
    void basecase(Ideal::const_iterator, Ideal::const_iterator, bool plus, const Term& term);
	void getPivot(const Ideal& ideal, Term& pivot);

	size_t _varCount;
	CoefTermConsumer* _consumer;
    Polynomial _output;
};

#endif
