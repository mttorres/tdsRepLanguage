%{
  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  

%}
%option noyywrap
%option yylineno

/* TOKENS */
%%
[\n] 				 	{}
[\t]          			{}
"//".*         	  		{}
[0-1]+            		printf("TOKEN: RAWNUMBERDATA - %s,  LINHA: %d \n",yytext,yylineno);
==                		printf("TOKEN: EQUAL - %s,  LINHA: %d \n",yytext,yylineno);
=				        printf("TOKEN: ASSIGN - %s, LINHA : %d \n",yytext,yylineno);
true | false			printf("TOKEN: BOOLEAN - %s, LINHA: %d \n",yytext,yylineno);
return		        	printf("TOKEN: RETURN - %s, LINHA: %d \n",yytext,yylineno);
for				        printf("TOKEN: FOR - %s, LINHA: %d \n",yytext,yylineno);
to 				        printf("TOKEN: TO - %s, LINHA: %d \n",yytext,yylineno);
do				        printf("TOKEN: DO - %s, LINHA: %d \n",yytext,yylineno);
[{]				        printf("TOKEN: LBRACE - %s, LINHA: %d \n",yytext,yylineno);
[}]				        printf("TOKEN: RBRACE - %s, LINHA: %d \n",yytext,yylineno);
[(]				        printf("TOKEN: LPAREN - %s, LINHA: %d \n",yytext,yylineno);
[)]				        printf("TOKEN: RPAREN - %s, LINHA: %d \n",yytext,yylineno);
if				        printf("TOKEN: IF - %s, LINHA: %d \n",yytext,yylineno);
else			        printf("TOKEN: ELSE - %s, LINHA: %d \n",yytext,yylineno);
NULL			        printf("TOKEN: NULL - %s, LINHA: %d \n",yytext,yylineno);
:						printf("TOKEN: COLON - %s, LINHA %d \n",yytext,yylineno);
,				        printf("TOKEN: COMMA - %s, LINHA: %d \n",yytext,yylineno);
and				        printf("TOKEN: AND - %s, LINHA: %d \n",yytext,yylineno);
or				        printf("TOKEN: OR - %s, LINHA: %d \n",yytext,yylineno);
->				        printf("TOKEN: IMPLIES - %s, LINHA: %d \n",yytext,yylineno);
xor				        printf("TOKEN: XOR - %s, LINHA: %d \n",yytext,yylineno);
[<=]              		printf("TOKEN: LE - %s, LINHA: %d \n",yytext,yylineno);
[>=]              	    printf("TOKEN: GE - %s, LINHA: %d \n",yytext,yylineno);
[<]               	    printf("TOKEN: LT - %s, LINHA: %d \n",yytext,yylineno);
[>]               		printf("TOKEN: GT - %s, LINHA: %d \n",yytext,yylineno);
[A-Za-z_]*	         	printf("TOKEN: ID - %s,  LINHA: %d \n",yytext,yylineno);


%%
int main(int argc, char* argv[]) {
  
  FILE *fp;
  //printf("teste %s \n",argv[0]);
  fp = fopen(argv[1], "r");
  if(!fp){
  	printf("CADE MEU CODIGO!?");
  	return -1;
  } 
  yyin = fp;

  while (yylex());
}