all:
	bison -d src/tdsrpl.y 
	flex src/tdsrpl.l 
	gcc src/tdsrpl.tab.c lex.yy.c src/datastructures/Node.c src/datastructures/constants.c -o tdsPARSING
	./tdsPARSING sample/merger.tdsrpl > PARSINGOUTPUT