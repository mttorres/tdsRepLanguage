all:
	bison  -d src/tdsrpl.y  -b src/tdsrpl
	flex -o src/lex.yy.c src/tdsrpl.l  
	gcc src/tdsrpl.tab.c src/lex.yy.c src/datastructures/Node.c src/datastructures/constants.c -o tdsPARSING
	./tdsPARSING sample/merger.tdsrpl > PARSINGOUTPUT