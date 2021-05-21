## About

 - Requires flex & bison
 - A high-level language used to represent data streams for Reo connectors. 
 - Generate smv "code" for the data streams of a given Reo Model used for model checking purposes.

Instructions
========

## 1. Building
    make
    
## 2. How to run
    ./tds your-code.tds
    
    or you can use the interactive mode
    
    ./tds -int your-code.tds
    
    The two previous execution commands also can accept a nuXmv model from [reo2nuXmv] (https://github.com/frame-lab/Reo2nuXmv).
    
## 3. Results
    Check the /results folder:
    Abstract Syntax Tree (AST): results/PARSINGOUTPUT
    The new .smv file with the generated model. 
    
    You can check some examples on the "sample" and "execution-scripts" folders.
    
 
New Features for the language will be added to the future, more details in:

[#68] (https://github.com/mttorres/tdsRepLanguage/issues/68)
[#73] (https://github.com/mttorres/tdsRepLanguage/issues/73)
    
