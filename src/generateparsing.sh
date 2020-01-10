bison -d tdsrpl.y
flex tdsrpl.l 
gcc tdsrpl.tab.c lex.yy.c -o tdsPARSING
./tdsPARSING ../sample/merger.tdsrpl > PARSINGOUTPUT