## About

 - Requires flex & bison
 - A high-level language used to represent data streams for Reo connectors. 
 - Generate smv "code" for the data streams given a Reo Model used for model checking purposes.

Instructions
========

## 1. Building
    make
    
    or

    bison  -d src/tdsrpl.y -b src/tdsrpl
    flex -o src/lex.yy.c src/tdsrpl.l
    gcc src/main.c src/tdsrpl.tab.c src/lex.yy.c src/datastructures/Node.c src/datastructures/constants.c -o tds
	  
## 2. How to run
    tds ##your-file.smv > results/PARSINGOUTPUT
    
## 3. Results
    Abstract Syntax Tree (AST): results/PARSINGOUTPUT
    Check your original .smv file !
    
