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
#ifndef HILBERT_INDEPENDENCE_CONSUMER
#define HILBERT_INDEPENDENCE_CONSUMER

#include "Projection.h"
#include "Term.h"
#include "Ideal.h"
#include "CoefTermConsumer.h"
#include "Task.h"

#include <vector>

class IndependenceSplitter;
class HilbertStrategy;

class HilbertIndependenceConsumer : public CoefTermConsumer, public Task {
public:
  HilbertIndependenceConsumer(HilbertStrategy* strategy);

  void reset(CoefTermConsumer* parent,
             IndependenceSplitter& splitter,
             size_t varCount);

  void clear();

  virtual void run(TaskEngine& engine);
  virtual void dispose();

  CoefTermConsumer* getLeftConsumer();

  virtual void consumeRing(const VarNames& names); // Does nothing.
  virtual void beginConsuming(); // Does nothing.
  virtual void consume(const mpz_class& coef, const Term& term);
  virtual void doneConsuming(); // Does nothing.

  CoefTermConsumer* getRightConsumer();

  const Projection& getLeftProjection() const;

  const Projection& getRightProjection() const;

private:
  class RightConsumer : public CoefTermConsumer {
  public:
    RightConsumer(HilbertIndependenceConsumer* parent);

    virtual void consumeRing(const VarNames& names); // Does nothing.
    virtual void beginConsuming(); // Does nothing.
    virtual void consume(const mpz_class& coef, const Term& term);
    virtual void doneConsuming(); // Does nothing.

  private:
    HilbertIndependenceConsumer* _parent;
  };

  virtual void consumeLeft(const mpz_class& leftCoef, const Term& leftTerm);

  virtual void consumeRight(const mpz_class& coef, const Term& term);

  Term _tmpTerm;
  mpz_class _tmpCoef;

  CoefTermConsumer* _parent;
  Projection _leftProjection;
  Projection _rightProjection;

  Ideal _rightTerms;
  vector<mpz_class> _rightCoefs;

  RightConsumer _rightConsumer;

  HilbertStrategy* _strategy;
};

#endif
