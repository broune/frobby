    SliceParams& useBoundElimination(bool value);
    SliceParams& useBoundSimplification(bool value);
    bool _useBoundElimination;
    bool _useBoundSimplification;

    slice.useBoundElimination(getBool(cli, UseBoundEliminationName));
    slice.useBoundSimplification(getBool(cli, UseBoundSimplificationName));
    const char* UseBoundEliminationName = "bound";
    const char* UseBoundSimplificationName = "boundSimplify";

  SliceParams& SliceParams::useBoundElimination(bool value) {
    _useBoundElimination = value;
    return *this;
  }

  SliceParams& SliceParams::useBoundSimplification(bool value) {
    _useBoundSimplification = value;
    return *this;
  }

    _useBoundElimination(true),
    _useBoundSimplification(true)
