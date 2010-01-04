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
#include "stdinc.h"
#include "FacadeCommon.h"

#include "CommonParams.h"
#include "IOFacade.h"
#include "BigIdeal.h"
#include "Ideal.h"
#include "TermTranslator.h"
#include "VarSorter.h"
#include "Scanner.h"
#include "IOHandler.h"
#include "DataType.h"
#include "error.h"
#include "BigTermConsumer.h"
#include "CoefBigTermConsumer.h"

namespace {
  void sortVars(Ideal& ideal, TermTranslator& translator) {
	VarSorter sorter(translator.getNames());
	sorter.permute(&translator);

	Ideal::iterator stop = ideal.end();
	for (Ideal::iterator it = ideal.begin(); it != stop; ++it)
	  sorter.permute(*it);
  }
}

FacadeCommon::FacadeCommon(const CommonParams& params,
						   const DataType& output): 
  Facade(params.getPrintActions()) {

  Scanner in(params.getInputFormat(), stdin);
  auto_ptr<IOHandler> outputHandler =
	createOHandler(in.getFormat(), params.getOutputFormat());
  if (output == DataType::getPolynomialType()) {
	_polyConsumerDeleter = outputHandler->createPolynomialWriter(stdout);
	_polyConsumer = _polyConsumerDeleter.get();
  } else if (output == DataType::getMonomialIdealType()) {
	_idealConsumerDeleter = outputHandler->createIdealWriter(stdout);
	_idealConsumer = _idealConsumerDeleter.get();
  } else {
	INTERNAL_ERROR(string("Unsupported output type ") + output.getName());
  }

  BigIdeal ideal;
  IOFacade facade(params.getPrintActions());
  facade.readIdeal(in, ideal);
  in.expectEOF();

  setup(params, ideal);
}

FacadeCommon::FacadeCommon(const CommonParams& params,
						   const BigIdeal& input,
						   BigTermConsumer& output):
  Facade(params.getPrintActions()),
  _idealConsumer(&output) {
  setup(params, input);
}

FacadeCommon::FacadeCommon(const CommonParams& params,
						   const BigIdeal& input,
						   CoefBigTermConsumer& output):
  Facade(params.getPrintActions()),
  _polyConsumer(&output) {
  setup(params, input);
}

FacadeCommon::~FacadeCommon() {
  // Constructor defined so auto_ptr<T> in the header does not need
  // definition of T.
}

void FacadeCommon::setup(const CommonParams& params, const BigIdeal& bigIdeal) {
  beginAction("Translating ideal to internal data structure.");
  _ideal.reset(new Ideal());
  _translator.reset(new TermTranslator(bigIdeal, *_ideal, false));
  endAction();

  if (!params.getIdealIsMinimal()) {
	beginAction("Minimizing ideal.");
	_ideal->minimize();
	endAction();
  }

  if (params.getProduceCanonicalOutput()) {
	beginAction("Sorting variables for canonical representation.");
	sortVars(*_ideal, *_translator);
	endAction();
  }
}
