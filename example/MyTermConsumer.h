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

// This class records the terms it consumes and provides access to the
// ideal or polynomial represented by them.
class MyTermConsumer : public Frobby::TermConsumer {
 public:
  MyTermConsumer(size_t varCount):
	_varCount(varCount) {
  }

  virtual ~MyTermConsumer() {
  }

  virtual void consume(mpz_ptr* exponentVector) {
	_ideal.push_back(toPP(exponentVector));
  }

  virtual void consume(const mpz_t coef, mpz_ptr* exponentVector) {
	_polynomial.push_back(MyTerm(toInt(coef), toPP(exponentVector)));
  }

  const MyIdeal& getIdeal() const {
	return _ideal;
  }

  const MyPolynomial& getPolynomial() const {
	return _polynomial;
  }

 private:
  MyPP toPP(mpz_ptr* exponentVector) {
	MyPP pp(_varCount);
	for (size_t var = 0; var < _varCount; ++var)
	  pp[var] = toInt(exponentVector[var]);
	return pp;
  }

  int toInt(const mpz_t bigInteger) {
	if (!mpz_fits_sint_p(bigInteger))
	  return 0xFFFFFFFF; // A real program should report an error here.
	else
	  return mpz_get_si(bigInteger);
  }

  MyIdeal _ideal;
  MyPolynomial _polynomial;
  size_t _varCount;
};
