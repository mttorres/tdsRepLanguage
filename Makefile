all:
	bison  -d src/flex-bison/tdsrpl.y -b src/flex-bison/tdsrpl
	flex -o src/flex-bison/lex.yy.c src/flex-bison/tdsrpl.l
	gcc src/datastructures/Node.c src/flex-bison/tdsrpl.tab.c src/flex-bison/lex.yy.c src/textManager.c src/langmodules/Object.c src/langmodules/TDS.c src/datastructures/STable.c src/datastructures/HeaderSmv.c src/EnvController.c src/PreProcess.c src/PostProcess.c src/Visitor.c src/Interpreter.c src/main.c -o tds
	./tds sample/basic-main/basic.tds sample/merger-fifo/nuxmv.smv

