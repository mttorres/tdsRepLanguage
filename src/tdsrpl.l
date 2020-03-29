%{
  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include "../headers/Enum.h"	
  #include "../headers/STable.h"
  #include "../headers/constants.h"
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
[\n] 			{}
[\t]          		{}
"//".*         		{}
[0-9]+                	{yylval.sval = strdup(yytext); return RAWNUMBERDATA;}
==                    	{yylval.sval = strdup(yytext); return EQUAL;}
!=                    	{yylval.sval = strdup(yytext); return NOTEQUAL;}
=		                    {yylval.sval = strdup(yytext); return ASSIGN;}
true | false          	{yylval.sval = strdup(yytext); return BOOLEAN;}
return		              {yylval.sval = strdup(yytext); return RETURN;}
function-domain         {yylval.sval = strdup(yytext); return FUNCTIONDOMAIN;}
function        	      {yylval.sval = strdup(yytext); return FUNCTION;}
for			                {yylval.sval = strdup(yytext); return FOR;}
to 			                {yylval.sval = strdup(yytext); return TO;}
do			                {yylval.sval = strdup(yytext); return DO;}
[{]			                {yylval.sval = strdup(yytext); return LBRACE;}
[}]			                {yylval.sval = strdup(yytext); return RBRACE;}
[(]	                  	{yylval.sval = strdup(yytext); return LPAREN;}
[)]			                {yylval.sval = strdup(yytext); return RPAREN;}
[\[]                   	{yylval.sval = strdup(yytext); return LBRACK;}
[\]]                   	{yylval.sval = strdup(yytext); return RBRACK;}
if			                {yylval.sval = strdup(yytext); return IF;}
else			              {yylval.sval = strdup(yytext); return ELSE;}
NULL			              {yylval.sval = strdup(yytext); return Null;}
[.]			                {yylval.sval = strdup(yytext); return POINT;}
[:]			                {yylval.sval = strdup(yytext); return COLON;}
[,]			                {yylval.sval = strdup(yytext); return COMMA;}
and			                {yylval.sval = strdup(yytext); return AND;}
or			                {yylval.sval = strdup(yytext); return OR;}
not                   	{yylval.sval = strdup(yytext); return NOT;}
->			                {yylval.sval = strdup(yytext); return IMPLIES;}
xor			            {yylval.sval = strdup(yytext); return XOR;}
[<=]              	{yylval.sval = strdup(yytext); return LE;}
[>=]              	{yylval.sval = strdup(yytext); return GE;}
[\<]               	{yylval.sval = strdup(yytext); return LT;}
[\>]               	{yylval.sval = strdup(yytext); return GT; }
[+]                 {yylval.sval = strdup(yytext); return PLUS;}
[-]                 {yylval.sval = strdup(yytext); return MINUS;}
[*]                 {yylval.sval = strdup(yytext); return TIMES;}
[/]                 {yylval.sval = strdup(yytext); return DIVIDE;}
'[A-Za-z_]*'        {yylval.sval = strdup(yytext); return LABEL;}
linked              {yylval.sval = strdup(yytext); return LINKED;}
link                {yylval.sval = strdup(yytext); return LINK;}
data-time           {yylval.sval = strdup(yytext); return DATATIME;}
delayed             {yylval.sval = strdup(yytext); return DELAYED;}
portname            {yylval.sval = strdup(yytext); return PORTNAME;}
[A-Za-z_]*		      {yylval.sval = strdup(yytext); return ID;}


%%



