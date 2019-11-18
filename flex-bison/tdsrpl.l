%{
  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h> 
  int lineno = 0;

%}
%option noyywrap
%option yylineno

/* TOKENS */
%%
\n 				 
[ \t\n]          
\t        ; /* ignora tab*/
[0-1]+            printf("TOKEN: RAWNUMBERDATA,  LINHA: %d \n",yylineno);
==                printf("TOKEN: EQUAL,  LINHA: %d \n",yylineno);
=				          printf("TOKEN: ASSIGN , LINHA : %d \n",yylineno);
[A-Za-z_]         printf("TOKEN: ID, LINHA: %d \n",yylineno);
true | false			printf("TOKEN: BOOLEAN, LINHA: %d \n",yylineno);
return		        printf("TOKEN: RETURN, LINHA: %d \n",yylineno);
for				        printf("TOKEN: FOR, LINHA: %d \n",yylineno);
to 				        printf("TOKEN: TO, LINHA: %d \n",yylineno);
do				        printf("TOKEN: DO, LINHA: %d \n",yylineno);
[{]				        printf("TOKEN: LBRACE, LINHA: %d \n",yylineno);
[}]				        printf("TOKEN: RBRACE, LINHA: %d \n",yylineno);
[(]				        printf("TOKEN: LPAREN, LINHA: %d \n",yylineno);
[)]				        printf("TOKEN: RPAREN, LINHA: %d \n",yylineno);
if				        printf("TOKEN: IF, LINHA: %d \n",yylineno);
else			        printf("TOKEN: ELSE, LINHA: %d \n",yylineno);
,				          printf("TOKEN: COMMA, LINHA: %d \n",yylineno);
and				        printf("TOKEN: AND, LINHA: %d \n",yylineno);
or				        printf("TOKEN: OR, LINHA: %d \n",yylineno);
->				        printf("TOKEN: IMPLIES, LINHA: %d \n",yylineno);
xor				        printf("TOKEN: XOR, LINHA: %d \n",yylineno);
[<=]              printf("TOKEN: LE, LINHA: %d \n",yylineno);
[>=]              printf("TOKEN: GE, LINHA: %d \n",yylineno);
[<]               printf("TOKEN: LT, LINHA: %d \n",yylineno);
[>]               printf("TOKEN: GT, LINHA: %d \n",yylineno);


%%
int main() {
  
  FILE *fp;

  fp = fopen("input", "r");
  if(!fp){
  	printf("CADE MEU CODIGO!?");
  	return -1;
  } 
  yyin = fp;

  while (yylex());
}