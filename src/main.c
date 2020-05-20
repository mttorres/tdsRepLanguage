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
  
  
  FILE *fp; // .tds file
  FILE *smvP; // .smv file;
  fp = fopen(argv[1], "r");
  smvP = fopen(argv[2], "r+");
  //printf("%s \n",argv[2]);

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
  fclose(smvP);
  //smvP = fopen(argv[1], "r");


}

