#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../headers/Node.h"


#define MAX_TABLE 550

const char* mappingEnumTable[] =  {
    "GLOBAL",
    "FUNC",
    "LOOP",
    "IF_BLOCK",
    "ELSE_BLOCK",
};

const char* mappingEnumEntry[] =  {
    "NUMBER",
    "LOGICAL",
    "LABEL",
    "TDS",
    "T_DIRECTIVE",
};



struct S_TABLE;

typedef struct E_TABLE
{
  char* name;
  ENTRY_TYPE type;
  void* val;
  int methodParam;
  int order;
  int level;
  struct S_TABLE * parentScope;
} TableEntry;


typedef struct S_TABLE
{
  struct S_TABLE* parent;
  struct S_TABLE** children;
  int nchild;
  int level;
  int order;
  SCOPE_TYPE type;
  // TABLE DATA (como representar!?)(ponteiros pra void?) (ou criar outra struct chamada TABLE ENTRY)
  // ave void!
  TableEntry** tableData;
    
} STable;


TableEntry* createEntry(char* name, ENTRY_TYPE type, void* val, int methodParam, STable* parentScope);


STable* createTable(SCOPE_TYPE type, STable* parent,  int level, int order);

STable* addSubScope(STable* parent, SCOPE_TYPE type);

void printTable(STable* t);

void letgoTable(STable* t);

int hash(const char * str);

void insert(STable* t, TableEntry* e); 

TableEntry* lookup(STable* t, const char* name);


void printEntry(TableEntry* e);

void letgoEntry(TableEntry* e);

TableEntry* createEntry(char* name, ENTRY_TYPE type, void* val, int methodParam, STable* parentScope) {
    
    TableEntry* newEntry = (TableEntry*) malloc(sizeof(TableEntry));
    newEntry->name = name; // nota para strings e para valores void criados (talvez seja necessário malloc)
    newEntry->type = type;
    newEntry->val = val;
    newEntry->methodParam = methodParam;
    newEntry->parentScope = parentScope;
    newEntry->level = parentScope->level;
  	newEntry->order = parentScope->order;
    
    
    return newEntry;
    
    
}

void printEntry(TableEntry* e) {

	if(e) {

		if(e->type == NUMBER || e->type == T_DIRECTIVE){
			printf("%s : (%s, %d, methodParam: %d, level: %d, order: %d ) \n"
				,e->name,mappingEnumEntry[e->type],*(int *)e->val,e->methodParam,e->level,e->order);
		}

		// logical == converter para numeros? 
		if(e->type == LOGICAL){
			int boolean = *(int * ) e->val;
			if(boolean){
				printf("%s : (%s, %s, methodParam: %d level: %d, order: %d ) \n"
					,e->name,mappingEnumEntry[e->type],"true",e->methodParam,e->level,e->order);
			}
			else{
				printf("%s : (%s, %s, methodParam: %d level: %d, order: %d ) \n"
					,e->name,mappingEnumEntry[e->type],"false",e->methodParam,e->level,e->order);
			}
		}

		if(e->type == LABEL){
			const char* valor; 
			valor = (char*) e->val;
		    printf("%s : (%s, '%s', methodParam: %d level: %d, order: %d ) \n"
		    	,e->name,mappingEnumEntry[e->type],valor,e->methodParam,e->level,e->order);
		}

		if(e->type == TDS){
			// TODO (struct TDS)
		}
    }
    else{
    	printf("null \n");
    }
}

STable* createTable(SCOPE_TYPE type, STable* parent,  int level, int order) {

	STable* newtable = (STable*) malloc(sizeof(STable));
	
/*	
	int nt = sizechildren /sizeof (STable*);
	STable** chillist;
	if(nt){
	    chillist = (STable**) malloc(nt*sizeof(STable*));   
	    // quais situações? PASSAR FILHOS COM VAR_ARGS ? (parece complicar desnecessáriamente...)(tabelas não vao ser criadas já com escopos filhos!)
	    // se não for feito assim vai literalmente não ter nenhuma utilidade passar os filhos para o construtor?
	}
*/
	newtable->nchild = 0;
	newtable->type = type;
	newtable->level = level;
	newtable->order = order;

	newtable->type = type;
	newtable->level = level;
	newtable->order = order;
	
	if(parent){
		newtable->parent = parent;
	}

/*
	if(chillist){
		newtable->children = chillist;
	}
*/
	
	newtable->tableData = (TableEntry**) malloc(MAX_TABLE*sizeof(TableEntry*));

	return newtable;



}

void printTable(STable* t){
	if(t){
		printf("%s (%d,%d) \n",mappingEnumTable[t->type],t->level,t->order);
	}
}

void letgoEntry(TableEntry* e){
	if(!e) {
	    return;
	}
	free(e);
}

void letgoTable(STable* t){
	if(!t) {
	    return;
	}
	int i;
	if(t->children){
		for(i=0; i < t->nchild; i++){
			letgoTable(t->children[i]);
		}
		free(t->children);
	}
	if(t->tableData){
		for(i=0; i < MAX_TABLE; i++){
			if(t->tableData[i]) {
			 letgoEntry(t->tableData[i]);   
			}
		}
	    free(t->tableData);
	}
	free(t);
}

int hash(const char * str) {
	int hash = 401;
	int c;

	while (*str != '\0') {
		hash = ((hash << 4) + (int)(*str)) % MAX_TABLE;
		str++;
	}

	return hash % MAX_TABLE;
}

void insert(STable* t, TableEntry* e) {
    
    int index = hash(e->name);
    printf("HASH CALCULADO É: %d \n",index);
    t->tableData[index] = e;
    
    
}

TableEntry* lookup(STable* t, const char* name) {
    
    int index = hash(name);
    return t->tableData[index];
    
}

// op's novas que funcionam

//void ou retorna o filho??? Retornar parece melhor, provavelmente ao adicionar um subscope eu vou querer operar sobre ele imediatamente
STable* addSubScope(STable* parent, SCOPE_TYPE type) {
	
	STable* child = createTable(type,parent,parent->level+1,parent->nchild);

	if(!parent->nchild) {
		parent->children = (STable**) malloc(parent->nchild*sizeof(STable*));
	}
	parent->children[parent->nchild] = child;
	parent->nchild++;

	return child;

}


//testes com struct de TDS: 



typedef struct TDS_VAR
{
  char* name;
  int type;
  int I_TIME; // reflete os valores globais
  int F_TIME;
  int C_TIME; // apesar que o C_TIME é do contexto dessa tds... 

  int linked; // lista unica que referencia outra TDS e toma os valores dela! (economiza um campo, evita ref ciclica)
  int delayed;

  // ainda não sei como isso vai ficar
  void * fpointer;
  

  // dados da tds (lista de tamanho = F_TIME - I_TIME), (ACEITAR DADOS DIFERENTES? OU SÓ UM PARA A TDS?)
  // se for um tipo só : usar union?  (pode ser um de cada vez mais não ao mesmo tempo)
  // senão usar VOID 
  void ** values; // delayed? passar lista "alternada?"



} TDSvar;

// passar valores e função para construtor? (função já vai estar definida, valores só vai estar definido se a TDS for linked!)
TDSvar* createTDS(char* name, int type, int I_TIME, int F_TIME, int C_TIME, int linked, int delayed);


TDSvar* createTDS(char* name, int type, int I_TIME, int F_TIME, int C_TIME, int linked, int delayed) {
	
	TDSvar* newTDS = (TDSvar*) malloc(sizeof(TDSvar));

	newTDS->name = name;
	newTDS->type = type;
	newTDS->I_TIME = I_TIME;
	newTDS->F_TIME = F_TIME;
	newTDS->C_TIME = C_TIME;
	newTDS->linked = linked;
	newTDS->delayed = delayed;

	return newTDS;						
}

// testando função, por enquanto, não retorna nada (apesar que seria uma boa sintetizar os valores para eivtar consultas na tabela (apesar que consultar a tabela é 0(1) e sinceramente diminui a complexidade absurdamente ao invés de retornar tipos e valores)
 
void* process(Node* n) {

	int info = n != NULL;	
	if(info) {		
		int i = 0;
		if(n->children){
			for(i= 0; i < n->nchild; i++){
				if(n->children[i]) {
					process(n->children[i]); // se tiver filho (desce um nível e resolve as dependencias) 
				}


			}
		}
		// ao chegar nas folhas avaliar a informação delas (todas "conjunto") e recuperar dependencias resolvidas do nível mais abaixo 
		if(n->leafs) {
			for(i = 0; i< n->nleafs;i++){					
			    
			}
		}
	}	
}


int main()
{
    printf("Hello World \n");

    // tentando criar o escopo equivalente do merger-fifo.tds

    // global (ordem 0(primeiro), nível 0) (e sem pais)
    	
	STable* global = createTable(GLOBAL,NULL,0,0);
	printTable(global);

	int vali = 0;
	int valc = 0;
	int valf = 3;

	// NOSSA ELE CONVERTEU STRING EM NUMERO AHSDJKAHDKSAKDAS
	//TableEntry* I_TIME = createEntry("I_TIME",T_DIRECTIVE,"bigfuckingstruct",1,nova);
	//TableEntry* C_TIME = createEntry("C_TIME",T_DIRECTIVE,"pequenastruct(list)",0,nova);
	//TableEntry* F_TIME = createEntry("F_TIME",T_DIRECTIVE,"pequenastruct(list)",0,nova);

	TableEntry* I_TIME = createEntry("I_TIME",T_DIRECTIVE,&vali,0,global);
	TableEntry* C_TIME = createEntry("C_TIME",T_DIRECTIVE,&valc,0,global);
	TableEntry* F_TIME = createEntry("F_TIME",T_DIRECTIVE,&valf,0,global);
		
	//int valboo = 0;

	//TableEntry* modoEntrada = createEntry("modoEntrada",LOGICAL,&valboo,1,global);
		
	//TableEntry* enumerado = createEntry("enumerado",LABEL,"OCUPADO",0,global);


	insert(global, I_TIME);
	insert(global, C_TIME);
	insert(global, F_TIME);
	//insert(global, modoEntrada);
	//insert(global, enumerado);
	
	printEntry(lookup(global, "I_TIME"));
	printEntry(lookup(global, "C_TIME"));
	printEntry(lookup(global, "F_TIME"));

	//printEntry(lookup(global, "modoEntrada"));
	//printEntry(lookup(global, "enumerado"));



	// if comum (ordem 1(primeiro de verdade, "zero"), nível 1) 
	//addSubScope(global,IF_BLOCK);
	//printTable(addSubScope(global,IF_BLOCK));

	// else associado (ordem 2("1"), nível 1)
	//addSubScope(global,ELSE_BLOCK);
	//printTable(addSubScope(global,ELSE_BLOCK));
    


    	// testando mudanças de Node e fluxo de construção, programa simples com atribuição de variável

	// passo 1: inicia escopo global, inicia diretivas temporais (já feito) 

	// passo 2: percorrer "arovre"  ( v = 10) ("só isso!") 

	// note que ASSIGN_V e todos os outros nós desse tipo na verdade são apenas DATA(no maximo filhos de DATA mais abaixo),
	// CONCLUSÃO: DIMINUIR A QUANTIDADE EXAGERADA DE ENUMS criados ! (agrupar enums) (apesar que isso atrapalha na tipagem ? ) 
	// opções: manter enums  (prepare-se para MUITOS IFS AND ELSES)
	//	   agrupar (porém colocar "subtipos" nos nós (prepare-se para mudar todo o arquivo .y e possivelmente introduzir novos bugs)
	//	   manter enums (porém "inferir" tipos ao adicionar a tabela, até porque a linguagem só tem 3 tipos, strings, int, e TDS)
	//	   	-> problema dessa abordagem: como diferenciar strings de números ? (não é tão ruim se parar para pensar, mas necessita escovar bit).
			// por outro lado: https://stackoverflow.com/questions/29381067/does-c-and-c-guarantee-the-ascii-of-a-f-and-a-f-characters

	Node* expr = createNode(5,0,1,"RAWNUMBERDATA",ASSIGN_V,"10");

	Node* assignable = createNode(5,0,1,"ASSIGNABLE",ASSIGN_V,"v");	

	Node* otherstmt = createNode(7,2,1,"OtherStmt-ASSIGN",OTHER_ASSIGN,assignable,expr,"=");

	Node* cmd = createNode(5,1,0,"OtherStmt",CMD_OTHER,otherstmt);

	Node* root = createNode(5,1,0,"Prog",PROG,cmd);

	
	printNode(root);
	

	process(root);
	
	
	letgoTable(global);
    
	return 0;
}
