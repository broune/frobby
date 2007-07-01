$g := read(data-source($1));
$t := current-time();
$d := decompose($g);
write(lex-order($d));
write($d);
write(";\n");
