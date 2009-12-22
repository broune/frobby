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
#include "stdinc.h"
#include "BigattiPivotStrategy.h"

#include "BigattiState.h"
#include "Ideal.h"
#include "Term.h"
#include "NameFactory.h"

BigattiPivotStrategy::BigattiPivotStrategy() {
}

BigattiPivotStrategy::~BigattiPivotStrategy() {
}

namespace {
  class MedianPivot : public BigattiPivotStrategy {
  public:
	const Term& getPivot(BigattiState& state) {
	  _counts.reset(state.getVarCount());
	  state.getIdeal().getSupportCounts(_counts);
	  size_t var = _counts.getFirstMaxExponent();

	  _pivot.reset(state.getVarCount());
	  _pivot[var] = state.getMedianPositiveExponentOf(var);	
	  return _pivot;
	}

	virtual const char* getName() const {
	  return staticGetName();
	}

	static const char* staticGetName() {
	  return "median";
	}

  private:
	Term _counts;
	Term _pivot;
  };

  class TypicalPivot : public BigattiPivotStrategy {
  public:
	const Term& getPivot(BigattiState& state) {
	  size_t typicalVar;
	  size_t typicalExponent;
	  size_t count = state.getTypicalExponent(typicalVar, typicalExponent);

	  // Otherwise we can get into infinite loop, and this is better
	  // in any case.
	  if (count == 1)
		return _median.getPivot(state);

	  _pivot.reset(state.getVarCount());
	  _pivot[typicalVar] = typicalExponent;
	  return _pivot;
	}

	virtual const char* getName() const {
	  return staticGetName();
	}

	static const char* staticGetName() {
	  return "typical";
	}

  private:
	Term _pivot;
	MedianPivot _median;
  };

  typedef NameFactory<BigattiPivotStrategy> StrategyFactory;

  StrategyFactory makeStrategyFactory() {
	StrategyFactory factory;

	nameFactoryRegister<MedianPivot>(factory);
	nameFactoryRegister<TypicalPivot>(factory);

	return factory;
  }
}

auto_ptr<BigattiPivotStrategy> BigattiPivotStrategy::
createStrategy(const string& name) {
  return makeStrategyFactory().create(name);
}
