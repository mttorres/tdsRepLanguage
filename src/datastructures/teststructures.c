#include <stdio.h>
#include <string.h>
#include "../../headers/Node.h"
#include "../../headers/STable.h"
#include "../../headers/constants.h"



int main(){



// testando nó	
/*
	char * testeLeaf[] = {
    "oi",
    "eu",
    "sou",
    "o",
    "goku!",
};
	
	
	char * testeLeaf1[] = {
    "eu",
    "tu",
    "nós",
    "...",
};

	char * testeLeaf2[] = {
    "chega",
    "de",
    "memes",
};

	int sizeleafs = sizeof(testeLeaf);
	int sizeleafs1 = sizeof(testeLeaf1);
	int sizeleafs2 = sizeof(testeLeaf2);

	char* nome = "Yare Yare...?";
	char* nome1 = "Daze?";
	char* nome2 = "ZAWAROLDO.";





	Node* filho1 = createNode(NULL, nome1,testeLeaf1,0,sizeleafs1);
	Node* filho2 = createNode(NULL, nome2,testeLeaf2,0,sizeleafs2);

	Node * filhos[] = {
		filho1,
		filho2,
};
	
	int tamanho = sizeof(filhos);



	printNode(filho1);
	printNode(filho2);

	Node* no = createNode(filhos, "Yare yare...?", testeLeaf,tamanho,sizeleafs);
	printNode(no);

	int i;
	tamanho = sizeof(filhos)/sizeof(Node);


	for(i = 0; i< tamanho;i++){
		letgoNode(filhos[i]);
	}

*/


// testando tabela de simbolos

	STable* nova = createTable(GLOBAL,NULL,0,0,NULL,0);
	printTable(nova);
	letgoTable(nova);
	return 0;
}


