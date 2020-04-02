  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include "../headers/STable.h"
  #include "../headers/constants.h"
  #include "../headers/Node.h"	

  extern int yylex(void);
  extern int yyparse();
  extern FILE *yyin;
  extern Node* root;
  extern void yyerror(const char *s);

int main(int argc, char* argv[]) {
  
  
  FILE *fp;
  fp = fopen(argv[1], "r");
  if(!fp){
  	printf("CADE MEU CODIGO!?");
  	return -1;
  } 
  yyin = fp;
  yyparse();

  printf("--------------TREE--------------------\n");
  printNode(root);
  letgoNode(root);
  fclose(fp);


}

