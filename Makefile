all:
	bison  -d src/flex-bison/tdsrpl.y -b src/flex-bison/tdsrpl
	flex -o src/flex-bison/lex.yy.c src/flex-bison/tdsrpl.l
	gcc src/datastructures/Node.c src/flex-bison/tdsrpl.tab.c src/flex-bison/lex.yy.c src/util/Hash.c src/util/textManager.c src/langmodules/TypeMinMax.c src/langmodules/TypeSet.c src/langmodules/Object.c src/langmodules/TDS.c src/datastructures/STable.c src/datastructures/HeaderSmv.c src/EnvController.c src/PreProcess.c src/PostProcess.c src/Visitor.c  src/langmodules/LangCore.c src/Interpreter.c src/main.c -o tds