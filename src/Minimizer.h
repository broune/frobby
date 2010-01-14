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
#ifndef IDEAL_MINIMIZER_GUARD
#define IDEAL_MINIMIZER_GUARD

#include <vector>

class Minimizer {
 private:
  typedef vector<Exponent*>::iterator iterator;
  typedef vector<Exponent*>::const_iterator const_iterator;

 public:
  Minimizer(size_t varCount):
    _varCount(varCount) {}

  iterator minimize(iterator begin, iterator end) const;

  pair<iterator, bool> colonReminimize(iterator begin, iterator end,
                                       const Exponent* colon);
  pair<iterator, bool> colonReminimize(iterator begin, iterator end,
                                       size_t var, Exponent exponent);

  bool dominatesAny(iterator begin, iterator end, const Exponent* term);
  bool dividesAny(iterator begin, iterator end, const Exponent* term);

  bool isMinimallyGenerated(const_iterator begin, const_iterator end);

 private:
  size_t _varCount;
};

#endif
