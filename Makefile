all:
	bison  -d src/tdsrpl.y -b src/tdsrpl
	flex -o src/lex.yy.c src/tdsrpl.l  
	gcc src/main.c src/tdsrpl.tab.c src/lex.yy.c src/datastructures/Node.c src/textManager.c src/langmodules/Object.c src/datastructures/STable.c src/datastructures/HeaderSmv.c src/PreProcess.c -o tds
	./tds sample/merger-fifo/merger-fifo.tds sample/merger-fifo/nuxmv.smv
