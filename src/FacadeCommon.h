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
#ifndef FACADE_COMMON_GUARD
#define FACADE_COMMON_GUARD

#include "Facade.h"

class FacadeCommon {
 protected:
  /** Supports operations producing output of the specified
   type. Reads an ideal from standard in and writes output to standard
   out. Params must remain valid for the lifetime of this object. */
  FacadeCommon(const CommonParams& params, const DataType& output);

  /** Supports operations producing monomial ideal output. */
  FacadeCommon(const CommonParams& params,
			   const BigIdeal& input,
			   BigTermConsumer& output);

  /** Supports operations producing polynomial output. */
  FacadeCommon(const CommonParams& params,
			   const BigIdeal& input,
			   CoefBigTermConsumer& output);

  ~FacadeCommon(); // no non-virtual destruction

  auto_ptr<Ideal> takeIdeal() {return _ideal;}
  const TermTranslator& getTranslator() {return *_translator;}
  BigTermConsumer& getIdealConsumer() {return *_idealConsumer;}
  CoefBigTermConsumer& getPolyConsumer() {return *_polyConsumer;}

 private:
  void setup(const CommonParams& params, const BigIdeal& ideal);

  FacadeCommon(const FacadeCommon&);
  FacadeCommon& operator=(const FacadeCommon&);

  auto_ptr<Ideal> _ideal;
  auto_ptr<Scanner> _scanner;
  auto_ptr<TermTranslator> _translator;

  BigTermConsumer* _idealConsumer;
  auto_ptr<BigTermConsumer> _idealConsumerDeleter;

  CoefBigTermConsumer* _polyConsumer;
  auto_ptr<CoefBigTermConsumer> _polyConsumerDeleter;
};

#endif
