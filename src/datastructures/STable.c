#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../headers/STable.h"
#include "../../headers/constants.h"


const char* mappingEnumTable[] =  {
    "GLOBAL",
    "FUNC",
    "LOOP",
    "IF_BLOCK",
    "ELSE_BLOCK",
    "SMV_PORTS",
    "SIMPLE_HASH"
};


// entry
TableEntry* createEntry(char* name, Object* val, int methodParam, STable* parentScope) {
    
    TableEntry* newEntry = (TableEntry*) malloc(sizeof(TableEntry));
    
    int s = strlen(name);
    //printf("[createEntry] nome original : %s \n",name);
    //printf("[createEntry] tamanho nome: %d \n",s);


    // agora ele sempre aloca um nome novo, deve-se dar free também!
    char* newName = malloc(sizeof(char)*(s+1));
    int i;

    char* dest = newName; 
    char* src = name;
    for(i=0; i < s; i++)
    {
    	// enquanto não \0
    	if(*src)
    	{
    		*dest = *src;
    		dest++;
    		src++;
    	}

    }
    *dest = '\0';

    printf("[createEntry] novo nome: %s \n",newName);
    
    newEntry->name = newName; 
    newEntry->val = val;   // objects sempre vão ser alocados, string devemos ter tratativa! (a não ser que essa seja alocada pelo bison?)
    newEntry->methodParam = methodParam;
    newEntry->parentScope = parentScope;
    newEntry->level = parentScope->level;
  	newEntry->order = parentScope->order;
    
    
    return newEntry;
    
    
}


void printEntry(TableEntry* e) {

	int info = e == NULL ? 0 : 1;
	//printf("teste : %d \n",info);
	if(info) 
	{
		printf("\t %s : ( ",e->name);
		printObject(e->val);
		if(e->val->type == TYPE_SET)
		{
			printf("\t");
			printTable((STable*)e->val->values[2]);
		}
		printf("\t ( methodParam: %d, level: %d, order: %d ) \n",e->methodParam,e->level,e->order);

	}
}



/// table

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
	newtable->lastEntryIndex = 0;
	newtable->backup = 0;
	
	if(parent){
		newtable->parent = parent;
	}

/*
	if(chillist){
		newtable->children = chillist;
	}
*/

	int selectSize =  type == SIMPLE_HASH ?  15 : MAX_TABLE;

/*
	if(type == SIMPLE_HASH)
	{
		printf("[createTable] criando simple hash \n\n");
	}
	else
	{
		printf("[createTable] criando escopo \n\n");
	}
*/
	
	newtable->tableData = (TableEntry**) malloc(selectSize*sizeof(TableEntry*));
	
	
	// garantia (tudo bem que eu NÃO VOU PRECISAR PERCORRER A TABELA DE SIMBOLOS, mas ele ta quebrando no print (por existir "qualquer coisa na tabela"))
	int i;
	for (i = 0; i < selectSize; ++i)
	{
		newtable->tableData[i] = NULL;	
	}


	return newtable;

}

void printTable(STable* t){
	if(t){
		printf("%s (%d,%d) \n",mappingEnumTable[t->type],t->level,t->order);
		if(t->lastEntryIndex != 0 && t->tableData){
			printf("|--> Entries: \n");
			int i;
			//printf("(total) %d \n",t->lastEntryIndex);
			for(i=0;i<= t->lastEntryIndex; i++){
				if(t->tableData[i])
				{
					//printf("%d \n",i);
					printEntry(t->tableData[i]);
				}
				
			}
		}
	}
}


void letgoEntry(TableEntry* e) {
	if(!e) {
	    return;
	}
	if(e->name)
	{
		free(e->name);
	}
	free(e);
}


void letgoTable(STable* t)
{
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
		for(i=0; i < MAX_TABLE; i++)
		{
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
    printf("HASH CALCULADO para (%s) É: %d \n",e->name,index);
   	t->tableData[index] = e;
    t->lastEntryIndex = index > t->lastEntryIndex ? index : t->lastEntryIndex;
    
}

TableEntry* lookup(STable* t, const char* name) {
    
    int index = hash(name);
    return t->tableData[index];
    
}



// op's novas que funcionam

// note que ! eu poderia muito bem por que os valores FOLHA dos nós da arvore são os mesmos que ENTRY_TYPE, existe ligeira redundância...
// QUE POR SINAL.... meu entry_type vai ser o meu "proxy" para valores? 
		// acho que vai ter que ter proxy... senão vai ficar dificil manipular os TIPOS
		// JÁ QUE OS TIPOS VÃO SER LITERALMENTE REALCIONADOS A DESEMPILHAR A ARVORE
		// primeiro eu vou testar usando valores literais 


/*

	Verifica se é necessário chamar addEntryToTypeSet
*/
int checkTypeSet(STable* current, char* name,  char* typeid)
{
	TableEntry* entry = lookup(current,name);
	if(entry)
	{
		if(lookup(entry->val->values[2],typeid))
		{
			printf("[checkTypeSet] %s encontrado no conjunto \n",typeid);
			return 0;
		}
		printf("[checkTypeSet] %s não existe no conjunto \n",typeid);
		// poderiamos chamar o método add aqui né ? (problemas: efeitos colaterais demais de uma só vez (atrapalha debug, lembre-se do print haha))
		return 1;
	}
}


/*
	Adiciona um tipo para o "conjunto de tipos das variáveis" 
	para otimizar a escrita no arquivo SMV.
	
*/
void addEntryToTypeSet(STable* current, char* name, char* typeid) 
{
	TableEntry* entry = lookup(current,name);
	if(entry)
	{
		int present = 1;
		void* po = {&present};	
		addValue(typeid,po,NUMBER_ENTRY,1,0,entry->val->values[2]);
	}
}



// método especializado para adicionar valores que sejam SMV_POINTERS (indice no Header, tamanho da palavra, conjunto de tipos(hashmap ou outro objeto)) 
/*

	Objeto:  vetor dinamico[inteirosDoConjunto], nullable, vetor dinamico[labelsDoConjunto]

	Prós: Menos structs para alocar,não tem que alocar uma TABELA INTEIRA só para alguns valores, 
	a linguagem possui poucos tipos (atualmente: int, label, boolean(FALSE E TRUE são só enums no nuXmv),
	e tds (que na verdade retorna qualquer um dos tipos anteriores))

	Cons:  Outro objeto (mais coisas para dar free), 
		   "quebaria" o conceito de hashmap que é justamente não ter duplicata, ia ser um objeto só para checar algumas coisas
		    Toda vez que adicionar um tipo novo necessita alterar essa estrutura

	Hashmap: estrutura que já temos atualmente, teria pelo menos umas 5 entras (talvez mais ou menos)

		Pŕos: Não necessita criar mais nada,  não precisa alocar vetores (indexa pela string em qualquer caso),
			  aberto para novos tipos (indexa pela string)

		Cons:  Vai ficar uma estrutura "recursiva" tabela --x entradas--> entradaSmv(porta ou main) --> Objeto(iHeader,size,
																											tabela ---y entradas--->entradas --> Objeto(bool))
				Mais coisas para centralizar e dar free
 

*/
void addTypeSet(char* name, void** any, int any_type, int object_size, STable* current)
{
	STable* hashset = createTable(SIMPLE_HASH,NULL,0,0);

	void* po[] = {any[0], any[1], hashset};

	printf("[addTypeSet] (index: %d, size: %d) \n",*(int*)po[0],*(int*)po[1]);

	printf("[addTypeSet] var-name: %s \n",name);

	addValue(name,po,TYPE_SET,object_size+1,0,current);
}


// refatorar? os dois métodos, usar só um que recebe "qualquer coisa" e encapsula em um objeto
void addValue(char* name, void** any, int any_type, int object_size ,int methodParam, STable* current) 
{

	// note que po é um ponteiro para objetos que o novo objeto irá encapsular, como criar ? 

	// POR ENQUANTO:

	//void* pa[] = {&vali}; (pro :possibilita manipular arrays) (cons: tenho que tratar tudo como vetor até quando é um unico valor)


	Object* o = createObject(any_type, object_size, any);
	addValueCurrentScope(name,o,methodParam,current);
}



void addValueCurrentScope(char* name, Object* val, int methodParam,STable* current) {

	//nome da variavel,   val vai ser literalmente o valor dela (problema, e quanto for uma lista?)
	TableEntry* entry = createEntry(name,val,methodParam,current);
	//printf("[addValueCurrentScope] DEBUG: \n\n");
	//printEntry(entry);
	insert(current,entry);
}






//void ou retorna o filho??? Retornar parece melhor, provavelmente ao adicionar um subscope eu vou querer operar sobre ele imediatamente
STable* addSubScope(STable* parent, SCOPE_TYPE type) {
	
	// LEMBRE-SE nchild usa a próxima posição para o escopo filho (inicia em 0, primeira pos)
	STable* child = createTable(type,parent,parent->level+1,parent->nchild);
	if(!parent->nchild) {
		parent->children = (STable**) malloc((parent->nchild+1)*sizeof(STable*));
	}
	else
	{
		if(!parent->backup)
		{
			STable** newbuffer = realloc(parent->children, (parent->nchild+1)*sizeof(STable*));
		
			if(newbuffer == NULL)
			{
				//back up em caso de falha de alocação de escopo
				int i;
				printf("[ALERT: addSubScope]  backup children \n");
				newbuffer = (STable**) malloc(MAX_CHILD*sizeof(STable*));
				for (i = 0; i < parent->nchild; i++)
				{
					newbuffer[i] = parent->children[i];
				}
				free(parent->children);
				// copia com espaço máximo (para de realocar)
				parent->children = newbuffer;
				parent->backup = 1;
			}
			else
			{
				parent->children = newbuffer;
				// realloc já da free na memória antiga !
			}
		}
		// senão não necessita alocar indefinidamente
	}
	parent->children[parent->nchild] = child; // adiciona novo filho
	// dita a próxima posição
	parent->nchild++;

	return child;

}

