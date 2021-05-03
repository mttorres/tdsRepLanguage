flex tdsrpl.l 
gcc lex.yy.c -o tdsLEXER
./tdsLEXER ../sample/merger.tdsrpl > LEXEROUTPUT