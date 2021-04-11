#ifndef ENUM_H

#define ENUM_H

//typedef enum {false, true} bool;

typedef enum SCOPE_TYPE { GLOBAL, FUNC, LOOP, IF_BLOCK, ELSE_BLOCK, SMV_PORTS, SMV_V_MAIN, SIMPLE_HASH} SCOPE_TYPE;

typedef enum ENTRY_TYPE {NUMBER_ENTRY, LOGICAL_ENTRY, LABEL_ENTRY ,T_DIRECTIVE_ENTRY, TDS_ENTRY, NULL_ENTRY, TIME_COMPONENT, FUNCTION_ENTRY, WRITE_SMV_INFO ,TYPE_SET } object_type;
			 // permitir lista de tipos diferentes? permitir lista de TDS's?
			 // comentado por enquanto, devemos decidir oque fazer sobre os enums do node


typedef enum smvtype {MAIN = 1, AUTOMATA = 2, PORTS = 3, FUNCTION_SMV = 4} smvtype;

typedef enum TDS_TYPE {DATA_LIST, MATH_EXPRESSION, FUNCTION_APPLY, TDS_DEPEN} TDS_TYPE;

//separar enums por: OPERAÇÃO UNÁRIA, OPERAÇÃO BINÁRIA, OPERAÇÃO ATRIBUIÇÃO, DADOS, 
					
//		DEFINIÇÃO PROGRAMA, DEFINIÇÃO TDS, DEFINIÇÃO 

									

// ou...   POR "INTERVALOS" ex:  DATA_NUM, DATA_BOOL, DATA_NULL, DATA_ITD, DATA_CTD, DATA_FTD, DATA_LABEL, DATA_V,   if(DATA_NUM  < tipo <  DATA_V) {return processData(node)}


// COLOCAR OS MAIS IMPORTANTES PRIMEIRO
// NOTA, alguns ENUMS só existem para validações, outros que são usados para dar evaluate.
// esses que só existem para validações as vezes estão já dentro de outro eval (o que poderiamos separar em diferntes evals para deixar,
// mais modularizado (VAMOS TENTAR FAZER ISSO COM AS IMPLEMENTAÇÕES MAIS RECENTES)
typedef enum EVAL_TYPE { 

	NUMBER, L_BOOL, STRING, D_NULL, IDVAR, TIME_DIRECTIVE, 

	DATA_V, PARAMS_CALL, PARAMS, DEFINE_INTERVAL,

	AC_V, ASSIGN_TDIRECTIVE ,OTHER_ASSIGN, V_PROP, ADD_V, ADD_V_PROP, V_PROP_TDS,

	EXPR, MINUS_EXPR, PLUS_EXPR, MULTI_EXPR, DIV_EXPR, LE_EXPR, GE_EXPR, LT_EXPR, GT_EXPR, EQUAL_EXPR, NEQUAL_EXPR,
    NOT_EXPR, AND_EXPR, OR_EXPR, IMP_EXPR, BIMP_EXPR, PRI_EXPR,

	CMD_IF, TDS_DEF_COMPLETE, TDS_DEF_DEPEN, CMD_TDS_ANON,

    LIST_ITERATOR, TIME_COMP, DOMAIN_FUNCTION,

    MATCH_IF,

    ASSIGN_IDVAR, ASSIGN_AC_V,

	DEF_EXTRAS_LINKED, DEF_EXTRAS_DELAYED, DEF_EXTRAS_FILTER,

    HEADERS_E_PROG, PROG, FUNC_DEFS, CMD,

	FUNC_DEF, PROC_DEF, OPT_RETURN, PARAM,

	CMD_OTHER,
	
	TDS_ANON_OP_PASS, TDS_ANON_OP_DPASS, TDS_ANON_OP_FPASS,

	OTHER_LOOP, FUNC_CALL, PROC_CALL,

	SHOW_PRINT
  

} EVAL_TYPE;



#endif
