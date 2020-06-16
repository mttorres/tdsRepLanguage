all:
	bison  -d src/tdsrpl.y -b src/tdsrpl
	flex -o src/lex.yy.c src/tdsrpl.l  
	gcc src/main.c src/tdsrpl.tab.c src/lex.yy.c src/datastructures/Node.c src/textManager.c src/datastructures/HeaderSmv.c -o tds
	./tds sample/novo/merger-fifo/merger-fifo.tds sample/novo/merger-fifo/nuxmv.smv
