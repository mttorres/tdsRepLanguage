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
    
       
   The two previous execution commands can also accept as input a nuXmv model from [reo2nuXmv](https://github.com/frame-lab/Reo2nuXmv), 
   providing integration between the two tools. 
   
      example:
   
     ./tds your-code.tds  reo2nuXmvModel.smv
    
## 3. Results
    Check the /results folder:
    Abstract Syntax Tree (AST): results/PARSINGOUTPUT
    The new .smv file with the generated model. 
    
You can check some examples on the "sample" and "execution-scripts" folders. Those contain input code as examples and the expected output model.
    
 
New Features for the language will be added in the future, more details in:

[#68](https://github.com/mttorres/tdsRepLanguage/issues/68),
[#73](https://github.com/mttorres/tdsRepLanguage/issues/73)
[General Issues](https://github.com/mttorres/tdsRepLanguage/issues/)
    
