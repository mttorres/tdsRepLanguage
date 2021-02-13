%{
  #include <stdio.h>
  #include <string.h>	
  #include "../headers/Node.h"
  #include "tdsrpl.tab.h"

  

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
[\n] 					{}
[\t]          			{}
"//".*         			{}
[0-9]+                	{yylval.sval = strdup(yytext); return RAWNUMBERDATA;}
==                    	{yylval.sval = strdup(yytext); return EQUAL;}
!=                    	{yylval.sval = strdup(yytext); return NOTEQUAL;}
=		      			{yylval.sval = strdup(yytext); return ASSIGN;}
true  					{yylval.sval = strdup(yytext); return BOOLEAN;}
false           		{yylval.sval = strdup(yytext); return BOOLEAN;}
return		        	{yylval.sval = strdup(yytext); return RETURN;}
function-domain         {yylval.sval = strdup(yytext); return FUNCTIONDOMAIN;}
function        		{yylval.sval = strdup(yytext); return FUNCTION;}
for						{yylval.sval = strdup(yytext); return FOR;}
in 						{yylval.sval = strdup(yytext); return IN;}
[{]						{yylval.sval = strdup(yytext); return LBRACE;}
[}]						{yylval.sval = strdup(yytext); return RBRACE;}
[(]	                	{yylval.sval = strdup(yytext); return LPAREN;}
[)]						{yylval.sval = strdup(yytext); return RPAREN;}
[\[]                   	{yylval.sval = strdup(yytext); return LBRACK;}
[\]]                   	{yylval.sval = strdup(yytext); return RBRACK;}
if						{yylval.sval = strdup(yytext); return IF;}
else					{yylval.sval = strdup(yytext); return ELSE;}
NULL					{yylval.sval = strdup(yytext); return Null;}
[.]						{yylval.sval = strdup(yytext); return POINT;}
[:]						{yylval.sval = strdup(yytext); return COLON;}
[,]						{yylval.sval = strdup(yytext); return COMMA;}
and						{yylval.sval = strdup(yytext); return AND;}
or						{yylval.sval = strdup(yytext); return OR;}
not                   	{yylval.sval = strdup(yytext); return NOT;}
-\|\|>					{yylval.sval = strdup(yytext); return DPASS;}
->						{yylval.sval = strdup(yytext); return PASS;}
xor						{yylval.sval = strdup(yytext); return XOR;}
[<=]              		{yylval.sval = strdup(yytext); return LE;}
[>=]              		{yylval.sval = strdup(yytext); return GE;}
[<]               		{yylval.sval = strdup(yytext); return LT;}
[>]               		{yylval.sval = strdup(yytext); return GT;}
[+]                 	{yylval.sval = strdup(yytext); return PLUS;}
[-]                 	{yylval.sval = strdup(yytext); return MINUS;}
[*]                 	{yylval.sval = strdup(yytext); return TIMES;}
[/]                 	{yylval.sval = strdup(yytext); return DIVIDE;}
[%]						{yylval.sval = strdup(yytext); return MOD;}
INTERVAL                {yylval.sval = strdup(yytext); return INTERVAL;}
~                       {yylval.sval = strdup(yytext); return BETWEEN;}
I_TIME					{yylval.sval = strdup(yytext); return INITTIME;}
C_TIME		    		{yylval.sval = strdup(yytext); return CURRENTTIME;}
F_TIME		    		{yylval.sval = strdup(yytext); return FINALTIME;}
'[A-Za-z_]*'        	{yylval.sval = strdup(yytext); return LABEL;}
linked              	{yylval.sval = strdup(yytext); return LINKED;}
data-time           	{yylval.sval = strdup(yytext); return DATATIME;}
delayed             	{yylval.sval = strdup(yytext); return DELAYED;}
portname            	{yylval.sval = strdup(yytext); return PORTNAME;}
[A-Za-z_]*				{yylval.sval = strdup(yytext); return ID;}


%%

void yyerror(const char *s) {
  printf("Erro de parsing! %s, perto da linha %d \n",s,yylineno);
  exit(-1);
}


