%{

/*
*	inicilização de headers incluindo AST e Table
*/   

  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include "../../headers/Node.h"

  int yylex(void);
  int yyparse();
  FILE *yyin;

  	
  Node* root;
 
  extern int yylineno;
  extern void yyerror(const char *s);

 
  //typedef enum {false, true} BOOLEANO;

%}

/*
*	union para retornar os tipos desejados de dados para os tokens do lexer
	retornados como um yystype.  em que devemos definir os tipos por meio de uma union (pode assumir qualquer um dos valores dela)
*/ 

%locations 

%union {
  int ival;
  float fval;
  char *sval;
  Node* ast;
  //BOOLEANO boolval
};


/*
  definição dos tokens relacionados aos tipos
*/

%token <sval> RAWNUMBERDATA
%token <sval> EQUAL
%token <sval> NOTEQUAL    
%token <sval> ASSIGN
%token <sval> BOOLEAN
%token <sval> RETURN
%token <sval> FOR
%token <sval> TO
%token <sval> IN
%token <sval> LBRACE
%token <sval> RBRACE
%token <sval> LBRACK
%token <sval> RBRACK
%token <sval> LPAREN
%token <sval> RPAREN
%token <sval> IF
%token <sval> COLON
%token <sval> POINT
%token <sval> COMMA
%token <sval> AND
%token <sval> OR
%token <sval> NOT
%token <sval> ELSE
%token <sval> Null
%token <sval> IMP
%token <sval> BIMP
%token <sval> LE
%token <sval> GE
%token <sval> LT
%token <sval> GT
%token <sval> PLUS
%token <sval> MINUS
%token <sval> TIMES
%token <sval> DIVIDE
%token <sval> MOD
%token <sval> FUNCTIONDOMAIN
%token <sval> FUNCTION
%token <sval> PLICK
%token <sval> DATATIME
%token <sval> DELAYED
%token <sval> PORTNAME
%token <sval> LINKED
%token <sval> INITTIME
%token <sval> CURRENTTIME
%token <sval> FINALTIME
%token <sval> ID
%token <sval> PASS
%token <sval> DPASS
%token <sval> INTERVAL
%token <sval> BETWEEN
%token <sval> TDSVALUE
%token <sval> FILTER

%type <ast> prog
%type <ast> cmds
%type <ast> cmd
%type <ast> functiondefs 
%type <ast> functiondef
%type <ast> param
%type <ast> params
%type <ast> data
%type <ast> otherstmt
%type <ast> assignable
%type <ast> expr
%type <ast> subexp
%type <ast> multiexp
%type <ast> ineqexp
%type <ast> logical
%type <ast> variabledata
%type <ast> variableprop
%type <ast> functioncall 
%type <ast> paramsCall
%type <ast> matchornot
%type <ast> extraaccesses
%type <ast> extras
%type <ast> passoption
%type <ast> dataflow
%type <ast> tdsprop
%type <ast> domain
%type <ast> timelist
%type <ast> timecomponent
%type <ast> anonimtdsop
%type <ast> intervaldefs
%type <ast> main
%left ELSE
%left TIMES DIVIDE MINUS PLUS LE GE GT LT EQUALS NOTEQUAL


%start main


%%

/*
	Gramatica para o parser 
*/


intervaldefs: RAWNUMBERDATA BETWEEN RAWNUMBERDATA {

			Node* data = createNode(7,0,3,"OBSERVATION-INTERVAL-COMPLETE", DEFINE_INTERVAL,$1,$2,$3);
			$$ = data;

	  }
	  | BETWEEN RAWNUMBERDATA {

			Node* data = createNode(6,0,2,"OBSERVATION-INTERVAL-END-ONLY", DEFINE_INTERVAL,$1,$2);
			$$ = data;

	  }
	  | RAWNUMBERDATA BETWEEN {
			Node* data = createNode(6,0,2,"OBSERVATION-INTERVAL-BEGIN-ONLY", DEFINE_INTERVAL,$1,$2);
			$$ = data;
	  }

main: INTERVAL COLON intervaldefs prog {

        Node* main = createNode(6,2,2,"Headers + program", HEADERS_E_PROG, $3,$4, $1,$2);
        $$ = main;
        root = $$;
    }
    |
    prog {
        $$ = $1;
        root = $$;
    }
    ;

prog: functiondefs cmds  {
		
		Node* prog = createNode(6,2,0,"Prog -  comandos e definições de função", PROG, $1, $2);
		
		//printf("PROGRAMA (%s) (%s) (%d) (filhos : %d) \n",prog->name,prog->children[1]->name ,prog->children[0] == NULL,prog->nchild); 	

		//printf("nome :(%s) (ref : %d) \n \n \n",$2->name,$2->children[0] != NULL); 

		//printf("END2: %d \n \n",prog->children);
		
		//infoNode($$);

		//printf("PROGRAMA (%s) (%s) (%d) (filhos : %d) \n \n",prog->name,prog->children[1]->name ,prog->children[1] == NULL,prog->nchild);

		//printf("nome :(%s) (ref : %d) \n \n \n",$$->children[1]->name,$$->children[1]->children[0] != NULL); 


		//printf(" teste lineno %d",yylineno);

		$$ = prog;

	  }
	  | cmds {

		Node* prog = createNode(5,1,0,"Prog -  comandos ", PROG, $1);	  
	  	$$ = prog;
	  } 
      ; 


cmds: cmds cmd {
		
	  	Node* cmds = createNode(6,2,0,"Cmds -  comando(s)", CMD ,$1,$2);

		//printf("%s (%s) (%s) (filhos: %d)\n \n \n",cmds->name,cmds->children[0]->name,cmds->children[1]->name,cmds->nchild);
		$$ = cmds; 
		//infoNode($$);

	  } 
	  | cmd {
		
		
	    $$ = $1; 

		//printf("%s (%s) (filhos: %d) \n \n",cmds->name,cmds->children[0]->name,cmds->nchild);
			

		//printf("END1(CMDS): %d \n \n",&filhos);

		//printf("END2(CMDS): %d \n \n",cmds->children);

		//infoNode($$);

		//printf("nome :(%s) (ref : %d) \n \n \n",$$->name,$$->children[0] != NULL); 
						
	  } 	
	  ;	

functiondefs: functiondefs functiondef {
		
	  	Node* functiondefs = createNode(6,2,0,"Functiondefs -  definição(ões) de função - loop", FUNC_DEFS, $1,$2);
		$$ = functiondefs; 
		//printf("iniciando definição de função \n");
		//infoNode($$);

	  }
	  |  functiondef {
	  	
		   $$ = $1; 	
	  }  
	  ; 	

functiondef: FUNCTION ID LPAREN params RPAREN LBRACE cmds  RBRACE  {

			Node* functiondef = createNode(12,2,6,"Functiondef -  definição de função ", FUNC_DEF,  $4,$7, $1,$2,$3,$5,$6,$8);
			//printf("DEFINICAO DE FUNÇÃO \n");
			$$ = functiondef; 
			//infoNode($$);
		
		}
		| FUNCTION ID LPAREN RPAREN LBRACE cmds  RBRACE  {

			Node* functiondef = createNode(11,1,6,"Functiondef -  definição de procedimento ", PROC_DEF, $6, $1,$2,$3,$4,$5,$7);
			//printf("DEFINICAO DE FUNÇÃO \n");
			$$ = functiondef; 
			//infoNode($$);
		}
		;


param: ID {
    Node* params = createNode(5,0,1,"VARIAVEL", IDVAR, $1);
    $$ = params;
}

params: param {
			$$ = $1;
		}
	    | params COMMA param {
			Node* params = createNode(7,2,1,"Paramslist - lista de parâmetros a mais de uma função ", LIST_ITERATOR , $1,$3, $2);
			$$ = params; 	
	    }




cmd: IF LPAREN expr RPAREN LBRACE cmds RBRACE matchornot {
	
		if($8){
			Node* condstmt = createNode(12,3,5,"cmd - Condicional - IF",  CMD_IF  ,$3,$6,$8 , $1,$2,$4,$5,$7);	
			$$ = condstmt;
			//printf("CMD -  IF\n");
	  	}
	  	else{
	  		Node* condstmt = createNode(11,2,5,"cmd - Condicional - IF",  CMD_IF  ,$3,$6 , $1,$2,$4,$5,$7);	
			$$ = condstmt;
	  	}
	 }
	 | otherstmt {

		Node* cmd = createNode(5,1,0,"CMD -  Comando (otherstatement)", CMD_OTHER ,$1); 
		//infoNode($$);
		//printf("(%s) (%s) (filhos: %d) \n \n",cmd->name,cmd->children[0]->name,cmd->nchild);
		$$ = cmd; 		
	 }
	 // jogar para nó mais abaixo a operação para avaliar antes de toda a expressão!

	 | LPAREN params RPAREN anonimtdsop ID {
        if($2->type != IDVAR && ($4->type == TDS_ANON_OP_DPASS || $4->type == TDS_ANON_OP_FPASS) ){
                if($4->type == TDS_ANON_OP_DPASS){
                    fprintf(stderr, "\n[PARSING ERROR] DELAYED-FIFO option for %s not valid for multiples inputs! \n",$5);
                }
                else{
                    fprintf(stderr, "\n[PARSING ERROR] FILTER option for %s not valid for multiples inputs! \n",$5);
                }
                exit(-1);
        }
		Node* cmd = createNode(9,2,3,"CMD -  Comando (criação TDS-anonima)",  CMD_TDS_ANON ,$2,$4,  $1,$3,$5); 
		//infoNode($$);
		//printf("(%s) (%s) (filhos: %d) \n \n",cmd->name,cmd->children[0]->name,cmd->nchild);
		$$ = cmd; 	 	
	 }
	 ;


anonimtdsop: PASS  {
				Node* p = createNode(5,0,1,"CMD -  PASS", TDS_ANON_OP_PASS ,$1); 
				$$ = p;
		}
		|   DPASS  {
        		Node* p = createNode(5,0,1,"CMD -  DPASS", TDS_ANON_OP_DPASS ,$1);
        		$$ = p;
        }
        | MINUS LBRACE expr RBRACE GT {
        		Node* p = createNode(9,1,4, "CMD -  FPASS", TDS_ANON_OP_FPASS,   $3,  $1, $2, $4, $5);
        		$$ = p;
        }

paramsCall: expr {
		
			$$ = $1; 		

		}
	    | paramsCall COMMA expr {
			
			Node* paramsCall = createNode(7,2,1,"Params -  parametro(s)  da chamada de função LOOP", PARAMS_CALL  ,$1,$3, $2);
			$$ = paramsCall; 	
	    }



functioncall: ID LPAREN paramsCall RPAREN {
	
				Node* functioncall = createNode(8,1,3,"Chamada de função", FUNC_CALL  ,$3, $1,$2,$4);
				$$ = functioncall;		

			}
			| ID LPAREN RPAREN {
	
				Node* functioncall = createNode(7,0,3,"Chamada de procedimento", PROC_CALL ,$1,$2,$3);
				$$ = functioncall;		

			}






otherstmt: FOR ID IN expr LBRACE cmds RBRACE {
	
		Node* otherstmt = createNode(11,2,5,"Otherstmt(for) -  comandos diferente de if e else ",  OTHER_LOOP,  $4,$6,  $1,$2,$3,$5,$7);

		$$ = otherstmt; 
		//printf("CMD - NAO IF/ELSE \n",otherstmt->name);
		//infoNode($$);


	 }
	 // depois adicionar sintaxe 2 de for e por um "iterable"

	 // talvez remover esse nó e por expr (?)
	 | functioncall {

		$$ = $1; 
		//infoNode($$);

	 }
	 | assignable ASSIGN variabledata {
		
		Node* assignment = createNode(7,2,1,"Assignment", OTHER_ASSIGN ,$1,$3, $2);
		$$ = assignment;	

		//printf("(!!)atribuicao (%s) \n \n",assignment->leafs[0]);
		//infoNode($$);

	 }
	 | RETURN variabledata {
	    	Node* optionalreturn = createNode(6,1,1,"Optionalreturn -  retorno opcional ", OPT_RETURN,  $2, $1);
        	$$ = optionalreturn;
	 }
	 | CURRENTTIME TO expr {
			Node* data = createNode(7,1,2,"CHANGE-CURRENTTIME-DIRECTIVE", ASSIGN_TDIRECTIVE , $3  ,$1,$2);
			$$ = data;
	 }
	 ;



assignable : ID extraaccesses {
		
		if($2){
				Node* assignment = createNode(6,1,1,"Acesso a variavel", ASSIGN_AC_V,  $2, $1);
				$$ = assignment;	

				//printf("(!!)atribuicao (%s) \n \n",assignment->leafs[0]);
				//infoNode($$);
		}
		else {
				Node* assignment = createNode(5,0,1,"VARIAVEL", ASSIGN_IDVAR, $1);
				$$ = assignment;	
		}
	 }
	 ;



expr: expr PLUS subexp {
		Node* expr = createNode(7,2,1,"Expressão Básica - soma ", PLUS_EXPR ,$1,$3,  $2);
		$$ = expr;
	 }
	 | subexp {

       Node* expr = createNode(5,1,0,"Expressão Básica - Nível 2", EXPR ,$1);
       $$ = expr;
	 }
	 ;

subexp: subexp MINUS multiexp {

        Node* expr = createNode(7,2,1,"Expressão Básica - subtração ",  MINUS_EXPR ,$1,$3,  $2);
        		$$ = expr;

        }
        | multiexp {

           Node* expr = createNode(5,1,0,"Expressão Básica - Nível 2", EXPR ,$1);
           $$ = expr;
        }

multiexp: multiexp TIMES ineqexp {
	
			Node* multiexp = createNode(7,2,1,"Expressão Básica - Multiplicação",  MULTI_EXPR ,$1,$3,  $2);
			$$ = multiexp;					
		

		}
		|multiexp DIVIDE ineqexp {
	
			Node* multiexp = createNode(7,2,1,"Expressão Básica - Divisão ", DIV_EXPR, $1,$3,  $2);
			$$ = multiexp;					
		

		}
		|multiexp MOD ineqexp {
	
			Node* multiexp = createNode(7,2,1,"Expressão Básica - MOD ", DIV_EXPR ,$1,$3,  $2);
			$$ = multiexp;

		}
		|ineqexp {

            Node* expr = createNode(5,1,0,"Expressão Básica - Nível 3", EXPR ,$1);
            $$ = expr;
		}
		;

ineqexp:  ineqexp LE logical {
			Node* ineqexp = createNode(7,2,1,"Inequação  - Menor igual a ", LE_EXPR, $1,$3,  $2);
			$$ = ineqexp;
		}
		| ineqexp GE logical {
			Node* ineqexp = createNode(7,2,1,"Inequação  - Maior igual a ", GE_EXPR ,$1,$3,  $2);
			$$ = ineqexp;
		}
		| ineqexp LT logical {
			Node* ineqexp = createNode(7,2,1,"Inequação  - Menor que ",  LT_EXPR ,$1,$3,  $2);
			$$ = ineqexp;
		}
		| ineqexp GT logical {
			Node* ineqexp = createNode(7,2,1,"Inequação  - Maior que ", GT_EXPR, $1,$3,  $2);
			$$ = ineqexp;
		}
        | ineqexp EQUAL logical {

			Node* ineqexp = createNode(7,2,1,"Inequação  - Igual a ", EQUAL_EXPR ,$1,$3,  $2);
			$$ = ineqexp;
        }
        | ineqexp NOTEQUAL logical {

			Node* ineqexp = createNode(7,2,1,"Inequação  - Diferente de ", NEQUAL_EXPR ,$1,$3,  $2);
			$$ = ineqexp;
        }
        | logical {
            Node* expr = createNode(5,1,0,"Expressão Básica - Nível 4", EXPR ,$1);
            $$ = expr;
        }
        ;


logical: MINUS data {
             Node* expr = createNode(6,1,1,"Expressão Básica - negativo", MINUS_EXPR, $2,  $1);
             $$ = expr;
        }
		 | logical AND data {
			Node* logical = createNode(7,2,1,"Lógico  - AND ", AND_EXPR ,$1,$3,  $2);
			$$ = logical;
		 }
		 | logical OR data {
			Node* logical = createNode(7,2,1,"Lógico  - OR ", OR_EXPR ,$1,$3,  $2);
			$$ = logical;
		 }
		 | logical IMP data {
			Node* logical = createNode(7,2,1,"Lógico  - OR ", IMP_EXPR ,$1,$3,  $2);
			$$ = logical;
		 }
		 | logical BIMP data {
			Node* logical = createNode(7,2,1,"Lógico  - OR ", BIMP_EXPR ,$1,$3,  $2);
			$$ = logical;
		 }
		 | data {
            Node* expr = createNode(5,1,0,"Expressão Básica - Nível 5", EXPR ,$1);
            $$ = expr;
		 }
		 ;
		 
data: RAWNUMBERDATA {
	
			Node* data = createNode(5,0,1,"Número", NUMBER ,$1);	
			$$ = data;	

	  }
      | NOT data {
			Node* logical = createNode(6,1,1,"Lógico  - Negação ", NOT_EXPR ,$2,  $1);
			$$ = logical;
	  }
	  | BOOLEAN {

			Node* data = createNode(5,0,1,"Booleano", L_BOOL ,$1);	
			$$ = data;	

	  }
	  | Null {

			Node* data = createNode(4,0,1,"NULL", D_NULL ,$1);	
			$$ = data;	

	  }

	  | INITTIME {
		
			Node* data = createNode(5,0,1,"INITTIME-DIRECTIVE", TIME_DIRECTIVE , $1);	
			$$ = data;
		
	  }


	  | CURRENTTIME {

			Node* data = createNode(5,0,1,"CURRENTTIME-DIRECTIVE", TIME_DIRECTIVE ,$1);	
			$$ = data;	

	  }

	  | FINALTIME {

			Node* data = createNode(5,0,1,"FINALTIME-DIRECTIVE", TIME_DIRECTIVE ,$1);	
			$$ = data;	

	  }
	  	  

	  | PLICK ID PLICK {
			Node* data = createNode(5,0,1,"LABEL", STRING ,$2);
			$$ = data;	

	  }

	  | functioncall {

	  	$$ = $1;	

	  }
	  | ID extraaccesses {

			if($2){
				Node* assignment = createNode(6,1,1,"Acesso a variavel", AC_V,  $2, $1);
				$$ = assignment;

				//printf("(!!)atribuicao (%s) \n \n",assignment->leafs[0]);
				//infoNode($$);
			}
			else {
				Node* assignment = createNode(5,0,1,"VARIAVEL", IDVAR, $1);
				$$ = assignment;

	  		}
	  }
	  | LPAREN expr RPAREN {
          Node* expr = createNode(7,1,2,"Expressão Básica - encapsulada", PRI_EXPR,  $2,  $1, $3);
          $$ = expr;
      }
	  ;




// diferenciar depoi snumber (index) e number data! (apesar de serem tokens "identicos em teoria")
// vetores de tds?  avaliar
extraaccesses : LBRACK expr RBRACK variableprop {
	
					if($4)
					{
			  	 		Node* extraaccesses = createNode(8,2,2,"propriedades extra de variavel(vetor)", ADD_V_PROP ,$2,$4,  $1,$3);	
						$$ = extraaccesses;
					}
					else
					{
			  	 		Node* extraaccesses = createNode(7,1,2,"indice de variavel", ADD_V ,$2, $1,$3);	
						$$ = extraaccesses;						
					}		

			  }
			  // esse nó por enquanto é "overhead" mas se as propriedades forem expandidas algum dia ele se torna util
			  | variableprop {
			  	 	
			  	 	if($1){
			  	 		Node* extraaccesses = createNode(5,1,0,"propriedades extra de variavel ", V_PROP ,$1);	
						$$ = extraaccesses;	
			  		}
			  		else{
			  			$$ = NULL; 
			  		}
			  }

//! trocar para propriedades da TDS (vai ser vetor e tds as "estruturas dessa linguagem")

variableprop: POINT tdsprop {
	
				Node* variableprop = createNode(6,1,1,"Propriedade de variavel tds ", V_PROP_TDS, $2,   $1);
				$$ = variableprop;   

			}
			| /* empty */  { $$ = NULL;}
			;





tdsprop: functioncall {
		
		Node* prop = createNode(5,1,0,"Props TDS - function", V_PROP_TDS ,$1);
		$$ = prop; 	
	  
	  }
	  | PORTNAME {

		Node* prop = createNode(5,0,1,"Props TDS - portname", V_PROP_TDS ,$1);
		$$ = prop; 	

	  }
	  | TDSVALUE {

		Node* prop = createNode(5,0,1,"Props TDS - value",  V_PROP_TDS_VALUE , $1);
		$$ = prop; 		  

	  }
	  | LINKED {

		Node* prop = createNode(5,0,1,"Props TDS - linked", V_PROP_TDS ,$1);
		$$ = prop; 		  

	  }
	  | DELAYED {

		Node* prop = createNode(5,0,1,"Props TDS - delay", V_PROP_TDS ,$1);
		$$ = prop; 	

	  }



variabledata: LBRACE PORTNAME COLON PLICK ID PLICK COMMA DATATIME COLON LBRACE dataflow RBRACE RBRACE {
		
			Node* tdsformat = createNode(15,1,10,"Informações de TDS",  TDS_DEF_COMPLETE , $11,  $1,$2,$3,$5,$7,$8,$9,$10,$12,$13);
			$$ = tdsformat;

		}
		| LBRACE PORTNAME COLON PLICK ID PLICK extras RBRACE {
			
			Node* tdsformat = createNode(10,1,5,"Informações de TDS - LINKED", TDS_DEF_DEPEN ,$7,  $1,$2,$3,$5,$8);
			$$ = tdsformat;
		
		}
	  	// VETOR
	  	| LBRACE paramsCall RBRACE 
	  	{
	  		Node* data = createNode(7,1,2,"VETOR", DATA_V ,$2, $1,$3);	
			$$ = data;	
	  	}
	  	| expr {
	  	    $$ = $1;
	  	}
		;

	
	
matchornot: ELSE LBRACE cmds RBRACE {
	
		Node* matchornot = createNode(8,1,3,"Condicional - ELSE", MATCH_IF ,$3 , $1,$2,$4);	
		$$ = matchornot;
		//printf("CMD - ELSE \n");

		}
		| /* empty */  { $$ = NULL;}
		;



dataflow:  domain   {
	
	$$ = $1; 	
}


domain: timelist { 

			$$ = $1;    

		}
		| FUNCTIONDOMAIN COLON expr {

			Node* domain = createNode(7,1,2,"DOMAIN-TDS-func", DOMAIN_FUNCTION  ,$3, $1,$2);
			$$ = domain;    

		}		



timecomponent: RAWNUMBERDATA COLON expr {
			   
	Node* timecomponent = createNode(7,1,2,"TIME-COMPONENT-TDS", TIME_COMP ,$3, $1,$2);
	$$ = timecomponent;    
}



timelist: timecomponent { 

			//Node* timelist = createNode(5,1,0,"TIME-LIST-TDS-one", LIST_ITERATOR ,$1);
			$$ = $1;
		  }

		  |timelist COMMA timecomponent {

			Node* timelist = createNode(7,2,1,"TIME-LIST-TDS-loop", LIST_ITERATOR, $1,$3,  $2 );
			$$ = timelist;		  		  

		  }




extras: COMMA LINKED COLON LBRACE params RBRACE passoption {
	
	   	 	Node* extra;
	   	 	if($7){
	   	 	    // NOVA VALIDAÇÃO DE PARSING:
	   	 	    Node* synth_params = $5;
	   	 	    if(synth_params->nchild > 1){
	   	 	       if($7->type == DEF_EXTRAS_DELAYED){
                        fprintf(stderr, "\n[PARSING ERROR] DELAYED-FIFO option not valid for multiples inputs! \n");
	   	 	       }
	   	 	       else{
                        fprintf(stderr, "\n[PARSING ERROR] FILTER option not valid for multiples inputs! \n");
	   	 	       }
                   exit(-1);
	   	 	    }
	   	 		extra = createNode(11,2,5,"LINKED-EXTRA-ARGS-TDS" , DEF_EXTRAS_LINKED ,$5, $7, $1,$2,$3,$4,$6);
	   	 	}
	   	 	else {
	   	 		extra = createNode(10,1,5,"LINKED-EXTRA-ARGS-TDS" , DEF_EXTRAS_LINKED , $5,  $1,$2,$3,$4,$6);
	   	 	}
	   	 	$$ = extra;
	}
	| passoption {
			
			//Node* extra = createNode(4,1,0,"DELAYED-EXTRA-ARGS-TDS",$1);
	   	 	$$ = $1;
	}
	

passoption: COMMA DELAYED {
	   		
		Node* extra = createNode(6,0,2,"DELAYED-EXTRA-ARGS-TDS", DEF_EXTRAS_DELAYED ,  $1,$2);
	   	$$ = extra;
	   }
	   | COMMA FILTER COLON expr {
       		Node* extra = createNode(8,1,3,"FILTER-EXTRA-ARGS-TDS", DEF_EXTRAS_FILTER,  $4,  $1,$2,$3);
       	   	$$ = extra;
       }
	   | /* empty */ {  $$ = NULL;}
       ; 

%%


