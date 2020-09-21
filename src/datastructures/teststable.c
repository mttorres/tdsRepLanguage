#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "../../headers/Node.h"
#include "../../headers/STable.h"

/*
Object* process(Node* n,  STable* currentScope) 
{

	int info = n != NULL;	
	if(info) {		
		int i = 0;
		if(n->children){
			for(i= 0; i < n->nchild; i++){
				if(n->children[i]) {
					process(n->children[i], currentScope); // se tiver filho (desce um nível e resolve as dependencias) 
				}


			}
		}
		// ao chegar nas folhas avaliar a informação delas (todas "conjunto") e recuperar dependencias resolvidas do nível mais abaixo 
		if(n->leafs) {
			// for necessário ?
			//for(i = 0; i< n->nleafs;i++){	

				// value type node (simplesmente retorna), mas usamos para diferenciar o tipo								
			    if(n->type == DATA_NUM) {
			    	return n->leafs[0]; // testando com valores literais (o objeto primitivo da linguagem é necessário!)
			    }
			    if(n->type == ASSIGN_V) {
			    	return n->leafs[0]; // é só o nome da variável
			    }	
			    
			    // OP (é um tipo de operação) criar método processOp(tipoOP,noAtual) que vai adicionar a varável ao escopo atual
			    if(n->type == OTHER_ASSIGN){
			    							// nome  // tipo vindo do objeto primitivo, methodParam (nível 0 e ordem...?) 
			    	int testeValor = 10; 
			    	addValueCurrentScope("v", NUMBER_ENTRY,&testeValor, 0, currentScope);
			    }

			//}
		}
	}	
}
*/


int main()
{
    printf("Hello World \n");

    // tentando criar o escopo equivalente do merger-fifo.tds

    // global (ordem 0(primeiro), nível 0) (e sem pais)
    	
	STable* global = createTable(GLOBAL,NULL,0,0);


	int vali = 0;
	int valc = 0;
	int valf = 3;

	void* pa[] = {&vali};

	void* pb[] = {&valf};

	Object* num0 = createObject(T_DIRECTIVE_ENTRY, 1, pa);
	Object* num3 = createObject(T_DIRECTIVE_ENTRY, 1, pb);

	// NOSSA ELE CONVERTEU STRING EM NUMERO AHSDJKAHDKSAKDAS
	//TableEntry* I_TIME = createEntry("I_TIME",T_DIRECTIVE,"bigfuckingstruct",1,nova);
	//TableEntry* C_TIME = createEntry("C_TIME",T_DIRECTIVE,"pequenastruct(list)",0,nova);
	//TableEntry* F_TIME = createEntry("F_TIME",T_DIRECTIVE,"pequenastruct(list)",0,nova);


	// ATENÇÃO:  PODE ESTAR USANDO ERRADAMENTE, até porque esse inteiro só existe nesse escopo (deve-se alocar memoria)

	TableEntry* I_TIME = createEntry("I_TIME",num0,0,global);
	TableEntry* C_TIME = createEntry("C_TIME",num0,0,global);
	TableEntry* F_TIME = createEntry("F_TIME",num3,0,global);
		

	int valboo = 0;

	void* pbool[] = {&valboo};

	Object* fabool = createObject(LOGICAL_ENTRY, 1, pbool);

	TableEntry* modoEntrada = createEntry("modoEntrada",fabool,1,global);


	char* vallabel = "OCUPADO";

	void* plab[] = {vallabel};

	Object* falab = createObject(LABEL_ENTRY, 1, plab);

	TableEntry* enumerado = createEntry("enumerado",falab,0,global);
	
	insert(global, I_TIME);
	insert(global, C_TIME);
	insert(global, F_TIME);
	insert(global, modoEntrada);
	insert(global, enumerado);
	
	printEntry(lookup(global, "I_TIME"));
	printEntry(lookup(global, "C_TIME"));
	printEntry(lookup(global, "F_TIME"));

	printEntry(lookup(global, "modoEntrada"));
	printEntry(lookup(global, "enumerado"));




	// if comum (ordem 1(primeiro de verdade, "zero"), nível 1) 
	//addSubScope(global,IF_BLOCK);
	printTable(addSubScope(global,IF_BLOCK));

	// else associado (ordem 2("1"), nível 1)
	//addSubScope(global,ELSE_BLOCK);
	printTable(addSubScope(global,ELSE_BLOCK));
    
/*

    // testando mudanças de Node e fluxo de construção, programa simples com atribuição de variável

	// passo 1: inicia escopo global, inicia diretivas temporais (já feito) 

	// passo 2: percorrer "arvore"  ( v = 10) ("só isso!")  (primeiro vamos fazer operações que não "geram escopo")

	// note que ASSIGN_V e todos os outros nós desse tipo na verdade são apenas DATA(no maximo filhos de DATA mais abaixo),
	
	// CONCLUSÃO: DIMINUIR A QUANTIDADE EXAGERADA DE ENUMS criados ! (agrupar enums) (apesar que isso atrapalha na tipagem ? ) 
	// opções: manter enums  (prepare-se para MUITOS IFS AND ELSES)
	//	   agrupar (porém colocar "subtipos" nos nós (prepare-se para mudar todo o arquivo .y e possivelmente introduzir novos bugs)
	//	   manter enums (porém "inferir" tipos ao adicionar a tabela, até porque a linguagem só tem 3 tipos, strings, int, e TDS)
	//	   	-> problema dessa abordagem: como diferenciar strings de números ? (não é tão ruim se parar para pensar, mas necessita escovar bit).
			// Solução adotada... criar "struct" do objeto da linguagem? (apesar que a E_TABLE faz a mesma coisa não?)
			// por outro lado: https://stackoverflow.com/questions/29381067/does-c-and-c-guarantee-the-ascii-of-a-f-and-a-f-characters

	Node* expr = createNode(5,0,1,"RAWNUMBERDATA",DATA_NUM,"10");

	Node* assignable = createNode(5,0,1,"ASSIGNABLE",ASSIGN_V,"v");	

	Node* otherstmt = createNode(7,2,1,"OtherStmt-ASSIGN",OTHER_ASSIGN,assignable,expr,"=");

	Node* cmd = createNode(5,1,0,"OtherStmt",CMD_OTHER,otherstmt);

	Node* root = createNode(5,1,0,"Prog",PROG,cmd);

	
	//printNode(root);
	

	process(root, global);

*/

	printTable(global);
	
	letgoTable(global);
    
	return 0;
}
