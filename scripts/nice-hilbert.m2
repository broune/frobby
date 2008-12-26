gradedRing = varCount -> (
    x := symbol x;
	variables := apply(1..varCount, k -> x_k);
	listApply := args -> toList(apply args);
	makeVector := i -> listApply(1..varCount, k -> if k == i then 1 else 0);
	vectors := listApply(1..varCount, makeVector);
	return ZZ[variables, Degrees=>vectors];
)

coerceMonomial = R -> m -> (
	exps := first exponents m;
	return product apply(gens R, exps, (v, e) -> v^e);
)

toGrade = id -> (
	R := ring id;
    gR := gradedRing(#(gens R));
	return monomialIdeal(apply(id_*, coerceMonomial gR));
)

niceHilbert = id -> (
	R := ring id;
    gR := gradedRing(#(gens R));
	gId := monomialIdeal(apply(id_*, coerceMonomial gR));
	badHilbert = poincare gId;

	mapping := apply(gens(ring badHilbert), gens R, (a, b) -> a => b);
	return sub(badHilbert, mapping)
)
