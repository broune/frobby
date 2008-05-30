


class FileFormat {
 public:
  virtual ~FileFormat();

  virtual void readIdeal(Scanner& in, BigIdeal& ideal) = 0;
  virtual void readIrreducibleDecomposition(Scanner& in, BigIdeal& decom) = 0;
  virtual void readTerm(Scanner& in,
						const VarNames& names,
						vector<mpz_class>& term);

  virtual void writeIdeal(FILE* out, const BigIdeal& ideal);
  virtual void writeIdeal(FILE* out, const Ideal& ideal,
						  const TermTranslator* translator);

  virtual BigTermConsumer* createIdealWriter
	(FILE* file, const VarNames& names) const = 0;

  virtual TermConsumer* createIdealWriter
    (FILE* file, const TermTranslator* translator) const = 0;

  virtual bool hasMoreInput(Scanner& scanner) const;

  virtual const char* getFormatName() const = 0;

  // Returns null if name is unknown.
  static IOHandler* getIOHandler(const string& name);

 protected:
  void readTerm(BigIdeal& ideal, Scanner& scanner);
  void readVarPower(vector<mpz_class>& term,
					const VarNames& names, Scanner& scanner);
  


};
