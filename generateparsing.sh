bison -d tdsrpl.y
flex tdsrpl.l 
gcc tdsrpl.tab.c lex.yy.c datastructures/Node.c datastructures/constants.c -o tdsPARSING
./tdsPARSING ../sample/merger.tdsrpl > PARSINGOUTPUT