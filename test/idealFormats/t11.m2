R4 = QQ[R, R1, R2, R3, x];
I = monomialIdeal(0_R4);
I = monomialIdeal(
 1_R4,
 R*R1*R2*R3*x
);
R = QQ[R1];
I = monomialIdeal(
 R1
);
