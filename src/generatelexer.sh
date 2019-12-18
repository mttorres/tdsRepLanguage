flex tdsrpl.l 
gcc lex.yy.c -o tdsLEXER
./tdsLEXER merger.tdsrpl > LEXEROUTPUT