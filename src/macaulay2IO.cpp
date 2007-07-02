#include "stdinc.h"
#include "macaulay2IO.h"

#include "monosIO.h"

namespace macaulay2 {
  void computeDecomposition(const BigIdeal& input, BigIdeal& output) {
    IOHandler* ioHandler = new Macaulay2IOHandler();

    // Write temporary file
    ofstream out("__m2tmp.in");
    ioHandler->writeIdeal(out, input);
    out.close();

    // Do computation using Macaulay 2
    system("runMacaulay2 m2Script --silent > /dev/null");

    // Read temporary file
    ifstream in("__m2tmp.out");
    ioHandler->readIrreducibleDecomposition(in, output);
  }

  void computeDecomposition(const BigIdeal& input, const string& outputFile) {
    BigIdeal output;
    computeDecomposition(input, output);

    ofstream out(outputFile.c_str());
    IOHandler* ioHandler = new MonosIOHandler();
    ioHandler->writeIdeal(out, output);
  }
}

