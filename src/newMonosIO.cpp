#include "stdinc.h"
#include "newMonosIO.h"

namespace newMonos {
  void computeDecomposition(const string& temporaryFilename,
			    BigIdeal& input,
			    BigIdeal& output,
			    bool useScarfComplex,
			    bool printProgress) {
    IOHandler* ioHandler = new NewMonosIOHandler();

    unsigned int varCount = 1;
    if (!input.empty())
      varCount = input[0].size();

    // Write temporary file
    ofstream out((temporaryFilename + ".in").c_str());
    ioHandler->writeIdeal(out, input);
    out.close();

    // TODO: implement progress printing.
    
    // Do computation using monos
    string script;
    if (useScarfComplex)
      script = "decomposeScarfComplex.scm";
    else
      script = "decomposeAlexanderDual.scm";

    deleteFile(temporaryFilename + ".out");
    system(("runNewMonos " + script + ' ' +
	    temporaryFilename + ".in > " +
	    temporaryFilename + ".out ").c_str());

    // Read temporary file
    ifstream in((temporaryFilename + ".out").c_str());
    ioHandler->readIrreducibleDecomposition(in, output);
  }

  void computeDecomposition(const string& temporaryFilename,
			    BigIdeal& input,
			    const string& outputFile,
			    bool useScarfComplex,
			    bool printProgress) {
    BigIdeal output(input.getNames());
    computeDecomposition(temporaryFilename, input, output, useScarfComplex, printProgress);

    ofstream out(outputFile.c_str());
    IOHandler* ioHandler = new MonosIOHandler();
    ioHandler->writeIdeal(out, output);
  }
}
