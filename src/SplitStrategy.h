/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

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
#ifndef SPLIT_STRATEGY_GUARD
#define SPLIT_STRATEGY_GUARD

#include "Term.h"
#include "Ideal.h"

class Slice;

class SplitStrategy {
 public:
  enum SplitType {
	Unknown,

    Minimum,
    Median,
    Maximum,
	MinGen,
	Indep,
	GCD,

	MaxLabel,
	MinLabel,
    VarLabel,

	FrobSpecial
  };

  SplitStrategy(SplitType ps):
	_id(ps) {
  }

  static void getPivot(Term& pivot, Slice& slice, SplitStrategy ps);

  static SplitStrategy getSplitStrategy(const string& name) {
	if (name == "maxlabel")
	  return MaxLabel;
	if (name == "minlabel")
	  return MinLabel;
	if (name == "varlabel")
	  return VarLabel;
	if (name == "minimum")
	  return Minimum;
	if (name == "median")
	  return Median;
	if (name == "maximum")
	  return Maximum;
	if (name == "mingen")
	  return MinGen;
	if (name == "indep")
	  return Indep;
	if (name == "gcd")
	  return GCD;
	if (name == "frob")
	  return FrobSpecial;
	return Unknown;
  }

  static size_t getLabelSplitVariable(const Slice& slice,
									  SplitStrategy labelStrategy);

  static SplitStrategy getSpecialPivotStrategy() {
	return FrobSpecial;
  }

  bool isPivotSplit() {
	switch (_id) {
	case Minimum:
	case Median:
	case Maximum:
	case MinGen:
	case Indep:
	case GCD:
	case FrobSpecial:
	  return true;

	default:
	  return false;
	}
  }

  bool isLabelSplit() {
	switch (_id) {
	case MaxLabel:
	case MinLabel:
    case VarLabel:
	  return true;

	default:
	  return false;
	}
  }

  bool isValid() {
	return _id != Unknown;
  }

  bool isSpecialPivot() {
	return _id == FrobSpecial;
  }

  SplitStrategy(): _id(Unknown) {}

 private:
  // To make these inaccessible.
  //SplitStrategy();
  //SplitStrategy(const SplitStrategy&);
  //void operator=(const SplitStrategy&);

  SplitType _id;
};

#endif
