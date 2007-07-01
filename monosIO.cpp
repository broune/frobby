#include "stdinc.h"
#include "monosIO.h"

#include "BigIdeal.h"

namespace monos {
  void computeDecomposition(const string& temporaryFilename,
			    BigIdeal& input,
			    BigIdeal& output,
			    bool printProgress) {
    IOHandler* ioHandler = new MonosIOHandler();

    unsigned int varCount = 1;
    if (!input.empty())
      varCount = input[0].size();

    // Write temporary file
    ofstream out((temporaryFilename + ".in").c_str());
    ioHandler->writeIdeal(out, input);
    out.close();
    
    // Do computation using monos
    deleteFile(temporaryFilename + ".out");
    string infoTag;
    if (printProgress)
      infoTag = "-i ";
    system(("runMonos decompose.ms " +
	    infoTag +
	    temporaryFilename + ".in > " +
	    temporaryFilename + ".out ").c_str());

    // Read temporary file
    ifstream in((temporaryFilename + ".out").c_str());
    ioHandler->readIrreducibleDecomposition(in, output);
  }

  void computeDecomposition(const string& temporaryFilename,
			    BigIdeal& input,
			    const string& outputFile,
			    bool printProgress) {
    BigIdeal output(input.getNames());
    computeDecomposition(temporaryFilename, input, output, printProgress);

    ofstream out(outputFile.c_str());
    IOHandler* ioHandler = new MonosIOHandler();
    ioHandler->writeIdeal(out, output);
  }

  bool areIdealsEqual(const string& a, const string& b) {
    IOHandler* ioHandler = new MonosIOHandler();

    ifstream inA(a.c_str());
    BigIdeal idealA;
    ioHandler->readIdeal(inA, idealA);

    ifstream inB(b.c_str());
    BigIdeal idealB;
    ioHandler->readIdeal(inB, idealB);

    if (idealA.sortUnique()) {
      cout << "ERROR: there were duplicates in output decomposition in file "
	   << a << '.' << endl;
    }

    if (idealB.sortUnique()) {
      cout << "ERROR: there were duplicates in output decomposition in file "
	   << b << '.' << endl;
    }

    return idealA == idealB;
  }
}
