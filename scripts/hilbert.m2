multiHilbert = id -> (
  gradedRing := varCount -> (
    x := symbol x;
    variables := apply(1..varCount, k -> x_k);
    listApply := args -> toList(apply args);
    makeVector := i -> listApply(1..varCount, k -> if k == i then 1 else 0);
    vectors := listApply(1..varCount, makeVector);
    return ZZ[variables, Degrees=>vectors];
  );

  R := ring id;
  gR := gradedRing(#(gens R));
  toGRMapping := apply(gens R, gens gR, (a, b) -> a => b);
  gHilbert := poincare(sub(id, toGRMapping));

  toRMapping := apply(gens(ring gHilbert), gens R, (a, b) -> a => b);
  return sub(gHilbert, toRMapping);
);

uniHilbert = id -> (
  R := ring id;
  uni := poincare id;
  tR := QQ[t];

  toSmallTMapping := apply(gens(ring uni), gens tR, (a, b) -> a => b);
  return sub(uni, toSmallTMapping);
);

printPoly = (poly, ofile) -> (
  ofile
    << "R = " << describe ring poly << ";" << endl
    << "p = " << toString poly << ";" << endl;
);

printIdeal = (id, ofile) -> (
  ofile
    << "R = " << describe ring id << ";" << endl
    << "I = " << toString id << ";" << endl;
);
