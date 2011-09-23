class FrobeniusInputAction : public InputAction {
public:
  FrobeniusInputAction(const string& filename, bool loadGrobnerBasis):
    _filename(filename),
    _loadGrobnerBasis(loadGrobnerBasis) {
  }

  virtual ~FrobeniusInputAction() {
  }

  virtual void execute(BigIdeal& ideal,
		       vector<mpz_class>& grading,
		       const Configuration& configuration) {
    readDegreesLog(_filename, grading, configuration);
    sort(grading.begin(), grading.end());

    if (_loadGrobnerBasis) {
      fourti2::readGrobnerBasisLog(_filename + ".gro", ideal, configuration);
    } else {
      BigIdeal basis;
      fourti2::computeKernelLatticeBasisLog
	(grading, basis, _filename + ".4ti2", configuration);
      
      if (configuration.getLllReduce())
	fplll::reduceLatticeBasisLog(basis, configuration);
      if (configuration.getAdjustBasis())
	fplll::makeZeroesInLatticeBasisLog(basis, configuration);
      
      fourti2::writeLatticeBasis(_filename + ".lat", basis);
      fourti2::computeGrobnerBasisLog
	(basis, ideal, _filename + ".gro", configuration);
    }
  }

  virtual void adjustDefaults(Configuration& configuration) {
    configuration.setComputeFrobeniusNumber(true);
    configuration.setUseBound(true);
    configuration.setUsePartition(false);
    configuration.setSkipRedundant(false);
  }

private:
  void readDegreesLog(const string& filename,
		      vector<Degree>& degrees,
		      const Configuration& configuration) {
    Timer timer;
    if (configuration.getPrintActions())
      cerr << "Reading Frobenius input from file \""
	   << filename << "\". " << flush;

    readDegrees(filename, degrees);

    if (configuration.getPrintActions())
      cerr << timer << endl;
  }

  void readDegrees(const string& filename,
		   vector<Degree>& degrees) {
    degrees.clear();

    ifstream in(filename.c_str());

    string number;
    while (in >> number) {
      for (unsigned int i = 0; i < number.size(); ++i) {
	if (!('0' <= number[i] && number[i] <= '9')) {
	  cerr << "ERROR: Input file \"" << filename
	       << "\" contains the character \""
	       << number[i] << "\"." << endl;
	  cerr << "Only whitespace and the digits 0-9 are valid." << endl;
	  exit(0);
	}
      }

      // number cannot represent a negative number as '-' is not
      // valid in the input and would have been caught above.
      Degree n(number);
      if (n == 0 || n == 1) {
	cerr << "ERROR: Input file \"" << filename
	     << "\" contains the number " << n << "." << endl;
	cerr << "Only integers strictly larger than 1 are valid." << endl;
	exit(0);
      }

      degrees.push_back(Degree(number));
    }

    if (degrees.size() <= 1) {
      cerr << "ERROR: Input file \"" << filename;
      if (degrees.empty())
	cerr << "\" does not contain any numbers." << endl;
      else
	cerr << "\" only contains one number." << endl;
      cerr << "At least 2 numbers are required." << endl;
      exit(0);
    }

    Degree gcd = degrees[0];
    for (unsigned int i = 1; i < degrees.size(); ++i)
      mpz_gcd(gcd.get_mpz_t(),
	      gcd.get_mpz_t(),
	      degrees[i].get_mpz_t());

    if (gcd != 1) {
      cerr << "ERROR: The numbers contained in input file \""
	   << filename << " are not relatively prime." << endl;
      cerr << "The numbers are required to be relatively prime." << endl;
      cerr << "A list of integers are relatively prime if and only if ";
      cerr << "one is the greatest number that divides all of them." << endl;
      cerr << "In this case " << gcd << " divides all the numbers, ";
      cerr << "so they are not relatively prime." << endl;
      exit(0);
    }
  }

  string _filename;
  bool _loadGrobnerBasis;
};
