%{
  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  

%}
/*
Opções:  noyywrap para facilitar o uso(?) e yylineno contador de linenumbers "automagico"
*/

%option noyywrap
%option yylineno

/* 
  TOKENS: ER'S não se pode usar /d como em outras linguagens. 
  Define-se uma ER e então uma ação para cada token, geralmente retornar ele para o nosso parser.
*/
%%
[\n] 				    {}
[\t]          	{}
"//".*         	{}
[0-9]+                printf("TOKEN: RAWNUMBERDATA - %s,  LINHA: %d \n",yytext,yylineno);
==                    printf("TOKEN: EQUAL - %s,  LINHA: %d \n",yytext,yylineno);
=				              printf("TOKEN: ASSIGN - %s, LINHA : %d \n",yytext,yylineno);
true | false		      {yylval.boolval = strdup(yytext);}
return		            {yylval.sval = strdup(yytext);}
for				            {yylval.sval = strdup(yytext);}
to 				            {yylval.sval = strdup(yytext);}
do				            {yylval.sval = strdup(yytext);}
[{]				            {yylval.sval = strdup(yytext);}
[}]				            {yylval.sval = strdup(yytext);}
[(]				            {yylval.sval = strdup(yytext);}
[)]				            {yylval.sval = strdup(yytext);}
[[]                   {yylval.sval = strdup(yytext);}
[]]                   {yylval.sval = strdup(yytext);}
if				            {yylval.sval = strdup(yytext);}
else			            {yylval.sval = strdup(yytext);}
NULL			            {yylval.sval = strdup(yytext);}
:						          {yylval.sval = strdup(yytext);}
,				              {yylval.sval = strdup(yytext);}
and				            {yylval.sval = strdup(yytext);}
or				            {yylval.sval = strdup(yytext);}
not                   {yylval.sval = strdup(yytext);}
->				            {yylval.sval = strdup(yytext);}
xor				            {yylval.sval = strdup(yytext);}
[<=]              		{yylval.sval = strdup(yytext);}
[>=]              	  {yylval.sval = strdup(yytext);}
[<]               	  {yylval.sval = strdup(yytext);}
[>]               		{yylval.sval = strdup(yytext);}
[+]                   {yylval.sval = strdup(yytext);}
[-]                   {yylval.sval = strdup(yytext);}
[*]                   {yylval.sval = strdup(yytext);}
[/]                   {yylval.sval = strdup(yytext);}
[A-Za-z_]*	         	{yylval.sval = strdup(yytext);}


%%

/*
  Programa principal responsável por ler o código passado como argv e retornar os tokens.
*/

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
  fclose(fp);
}