  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include "../headers/STable.h"
  #include "../headers/constants.h"
  #include "../headers/Node.h"
  #include "../headers/Enum.h"	
  #include "../headers/HeaderSmv.h"
   	

  extern int yylex(void);
  extern int yyparse();
  extern FILE *yyin;
  FILE *astout;
  extern Node* root;
  extern void yyerror(const char *s);

int main(int argc, char* argv[]) {
  
  
  	FILE *fp; // .tds file
  	FILE *smvP; // .smv file;
  	fp = fopen(argv[1], "r");
  	astout = fopen("results/astOutput", "w");
  	smvP = fopen(argv[2], "r+");
  	//printf("%s \n",argv[1]);
  	//printf("%s \n",argv[2]);

  	if(!fp){
  		printf("CADE MEU CODIGO!?");
  		return -1;
  	} 
  	yyin = fp;
 	yyparse();

  	fprintf(astout,"--------------TREE--------------------\n");
  	filePrintNode(root,astout);
  	fclose(astout);
  	fclose(fp);


	
	//pré processamento 
	HeaderSmv** headers = initHeadersStruct(5);
  	preProcessSmv(smvP,headers);
  	
  	size_t bufsize = 300;
  	char *buffer = (char *) malloc(bufsize * sizeof(char));
	
	int i;
	for(i = 0; i < 5; i++) {
	    	fseek(smvP,headers[i]->modulePointer,SEEK_SET);
		fgets(buffer,bufsize,smvP);
		printf("(%d) %s \n",i,buffer);
	
		fseek(smvP,headers[i]->varPointer,SEEK_SET);
		fgets(buffer,bufsize,smvP);
		printf("(%d) %s \n",i,buffer);

		fseek(smvP,headers[i]->assignPointer,SEEK_SET);
		fgets(buffer,bufsize,smvP);
		printf("(%d) %s \n",i,buffer);

		fseek(smvP,headers[i]->transPointer,SEEK_SET);
		fgets(buffer,bufsize,smvP);
		printf("(%d) %s \n",i,buffer);

    	} 
    printHeader(headers[0]);
    printHeader(headers[1]);
    printHeader(headers[2]);
    printHeader(headers[3]);
    printHeader(headers[4]);	
    //letGoHeadersStruct(headers,5);	
  	fclose(smvP);
  	//free(buffer);
  	//letgoNode(root);
  	//smvP = fopen(argv[1], "r");

  	



//TESTE PORTS REFS (aparentemente tudo funcionando)

/*
  	printf("TESTE PORT REFS !!! \n\n\n");
  	char* string1 = "	((cs = q0 & ports.a[time] = NULL & ports.b[time] = NULL & ports.d[time] = NULL & ports.c[time] = 0 & FALSE) -> next(cs) = p0) &";
  	char* nova =clearOldPortsRefs(string1);
  	printf("ANTES:  %s \n\n\n",string1);
    	printf("DEPOIS:  %s \n\n\n",nova);	
    	free(nova);

  	char* string2 = "	((cs = q0 & ports.a[time] = NULL & ports.d[time] = NULL & ports.c[time] != NULL & ports.b[time] = ports.c[time] & FALSE) -> next(cs) = q0);";
  	nova =clearOldPortsRefs(string2);
  	printf("ANTES:  %s \n\n\n",string2);
    	printf("DEPOIS:  %s \n\n\n",nova);	
    	free(nova); 

  	char* string3 = "	((cs = q0p0) -> ((next(cs) != q0p1))) &";
  	nova =  clearOldPortsRefs(string3);
  	printf("ANTES:  %s \n\n\n",string3);
    	printf("DEPOIS:  %s \n\n\n",nova);	
    	free(nova);    
*/

}

