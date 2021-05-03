//
// Created by mateus on 22/03/2021.
//

#include "../headers/EnvController.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void setUpTypeSetDict(EnvController* controller){
    char** dict = malloc(sizeof(char*)*TYPE_SET_DIR_SIZE);
    int i;
    for (i = 0; i  < TYPE_SET_DIR_SIZE ; i++) {
        dict[i] = NULL;
    }
    controller->typeSetWords = dict;
    addTypeSetWordToDict("0",controller);
    addTypeSetWordToDict("1",controller);
    addTypeSetWordToDict("NULL",controller);
}


EnvController *createController(int enableInteractive) {



    EnvController* Hcontrol = malloc(sizeof(EnvController));
    Hcontrol->interactiveMode = enableInteractive;
    Hcontrol->H_AUTOMATA_CURRENT_SIZE = 0;
    Hcontrol->H_PORTS_CURRENT_SIZE = 0;
    Hcontrol->PORTS_INFO_CURRENT_SIZE = 0;
    Hcontrol->H_FUNCTION_CURRENT_SIZE = 0;

    Hcontrol->originalPorts = createTable(SMV_PORTS, NULL, 0, 0, -1);
    Hcontrol->mainInfo = createTable(SMV_V_MAIN, NULL, 0, 0, -1);
    Hcontrol->functionsInfo = malloc(sizeof(STable*)*DEFAULT_HEADERS_AUX_SIZE);
    Hcontrol->portsInfo = malloc(sizeof(STable*)*DEFAULT_HEADERS_AUX_SIZE);

    Hcontrol->MAIN = NULL;
    Hcontrol->AUTOMATA_RELATED = malloc(sizeof(HeaderSmv*)*DEFAULT_HEADERS_AUX_SIZE);
    Hcontrol->PORTS_RELATED = malloc(sizeof(HeaderSmv*)*DEFAULT_HEADERS_AUX_SIZE);
    Hcontrol->FUNCTIONS = malloc(sizeof(HeaderSmv*)*DEFAULT_HEADERS_AUX_SIZE);

    Hcontrol->expectedPorts = 0;
    Hcontrol->validPorts = 0;
    Hcontrol->declaredPortsNumber= 0;
    Hcontrol->IO_RELATION = 0;
    Hcontrol->multiPortDeclartion = 0;
    Hcontrol->declaredPorts = malloc(sizeof(TDS*)*DEFAULT_HEADERS_AUX_SIZE); // NULL até sabermos o intervalo de tempo
    Hcontrol->currentTDScontext = NULL;

    int i;
    for (i = 0; i < DEFAULT_HEADERS_AUX_SIZE; i++) {
        Hcontrol->functionsInfo[i] = NULL;
        Hcontrol->portsInfo[i] =  NULL;
        Hcontrol->AUTOMATA_RELATED[i] = NULL;
        Hcontrol->PORTS_RELATED[i] = NULL;
        Hcontrol->FUNCTIONS[i] = NULL;
    }

    setUpTypeSetDict(Hcontrol);
    Hcontrol->modelHasFilter = 0;
    Hcontrol->automatasToChange = NULL;
    Hcontrol->F_AUTOMATAS_CHANGE_POINTER = 0;
    Hcontrol->filterContext = 0;
    Hcontrol->modelHasFinalAutomata = 0;
    Hcontrol->filterUsed = 0;
    return Hcontrol;
}

void letGoHeadersStruct(EnvController *H) {

    letgoHeader(H->MAIN);
    letGoRelatedHeaders(H->AUTOMATA_RELATED, H->H_AUTOMATA_CURRENT_SIZE);
    letGoRelatedHeaders(H->PORTS_RELATED, H->H_PORTS_CURRENT_SIZE);
    letGoRelatedHeaders(H->FUNCTIONS, H->H_FUNCTION_CURRENT_SIZE);

    letgoTable(H->mainInfo);
    // assim como temos n headers de função temos n tabelas de simbolos auxiliares que pode ou não ter sido liberada
    // antecipidamente (quando a função já terminou de ser escrita como smv)
    if(H->functionsInfo){
        int i;
        for (i=0; i < H->H_FUNCTION_CURRENT_SIZE; i++){
            if(H->functionsInfo[i]){
                letgoTable(H->functionsInfo[i]);
            }
        }
    }
    if(H->portsInfo){
        int i;
        for (i=0; i < H->PORTS_INFO_CURRENT_SIZE; i++){
            if(H->portsInfo[i]){
                letgoTable(H->portsInfo[i]);
            }
        }
    }
    if(H->portsInfo){
        free(H->portsInfo);
        H->portsInfo = NULL;
    }

    if(H->typeSetWords){
        int i;
        for (i = 0; i < TYPE_SET_DIR_SIZE; i++) {
            if(H->typeSetWords[i]){
                free(H->typeSetWords[i]);
            }
        }
    }

    if(H->declaredPorts){
        free(H->declaredPorts);
        H->declaredPorts = NULL;
    }
    if(H->automatasToChange){
        free(H->automatasToChange);
        H->automatasToChange = NULL;
    }
    letgoTable(H->originalPorts);
    free(H);
}


void letGoHeaderControl(EnvController* Hcontrol) {
    letGoHeadersStruct(Hcontrol);
}

void printAllHeaders(EnvController* Hcontrol){
    int i;
    printHeader(Hcontrol->MAIN);

    for (i = 0; i < Hcontrol->H_FUNCTION_CURRENT_SIZE; i++){
        printHeader(Hcontrol->FUNCTIONS[i]);
    }
    for (i = 0; i < Hcontrol->H_AUTOMATA_CURRENT_SIZE; i++){
        printHeader(Hcontrol->AUTOMATA_RELATED[i]);
    }
    for (i = 0; i < Hcontrol->H_PORTS_CURRENT_SIZE; i++){
        printHeader(Hcontrol->PORTS_RELATED[i]);
    }
}

HeaderSmv* accessHeader(EnvController* controller, smvtype cat, int indexOfHeader){
    HeaderSmv* toReturn = NULL;
    if(cat == MAIN){
        toReturn = controller->MAIN;
    }

    if(cat == AUTOMATA){
        toReturn = indexOfHeader >= 0? controller->AUTOMATA_RELATED[indexOfHeader] : controller->AUTOMATA_RELATED[controller->H_AUTOMATA_CURRENT_SIZE-1];
    }
    if(cat == PORTS){
        toReturn = indexOfHeader >= 0? controller->PORTS_RELATED[indexOfHeader] : controller->PORTS_RELATED[controller->H_PORTS_CURRENT_SIZE-1];
    }

    if(cat == FUNCTION_SMV){
        toReturn = indexOfHeader >= 0? controller->FUNCTIONS[indexOfHeader] : controller->FUNCTIONS[controller->H_FUNCTION_CURRENT_SIZE-1];
    }

    if(!toReturn){
        fprintf(stderr, "[accessHeader] INVALID SMV TYPE FOR OPERATION");
        exit(-1);
    }
    return toReturn;
}


STable* accessSmvInfo(EnvController* controller, smvtype cat, int SMV_INFO_ID){
    if(cat == MAIN){
        return controller->mainInfo;
    }
    if(SMV_INFO_ID < controller->H_FUNCTION_CURRENT_SIZE  && cat == FUNCTION_SMV){
        return SMV_INFO_ID >= 0? controller->functionsInfo[SMV_INFO_ID] : controller->functionsInfo[controller->H_FUNCTION_CURRENT_SIZE-1];
    }
    if(SMV_INFO_ID < controller->H_PORTS_CURRENT_SIZE && cat == PORTS){
        return SMV_INFO_ID >= 0? controller->portsInfo[SMV_INFO_ID] : controller->portsInfo[controller->PORTS_INFO_CURRENT_SIZE-1];
    }
    if(cat == AUTOMATA){
        return NULL; // melhorar, apesar que sabe-se que não tem operação em tabela de simbolos para automato (por enquanto)
    }
    fprintf(stderr, "[accessSmvInfo] INVALID SMV TYPE FOR OPERATION");
    exit(-1);
}

void addNewHeader(EnvController* controller, HeaderSmv* newHeader){

    if(newHeader->type == MAIN){
        controller->MAIN = newHeader;
    }
    if(newHeader->type == AUTOMATA){
        controller->AUTOMATA_RELATED[controller->H_AUTOMATA_CURRENT_SIZE] = newHeader;
        controller->H_AUTOMATA_CURRENT_SIZE++;
    }
    if(newHeader->type == PORTS){
        controller->PORTS_RELATED[controller->H_PORTS_CURRENT_SIZE] = newHeader;
        controller->H_PORTS_CURRENT_SIZE++;
    }
    if(newHeader->type == FUNCTION_SMV ){
        controller->FUNCTIONS[controller->H_FUNCTION_CURRENT_SIZE] = newHeader;
        controller->H_FUNCTION_CURRENT_SIZE++;
    }
}

void addNewAuxInfo(EnvController* controller, STable* newTableInfo){
    if(newTableInfo->type == SMV_PORTS){
        controller->portsInfo[controller->PORTS_INFO_CURRENT_SIZE] = newTableInfo;
        controller->PORTS_INFO_CURRENT_SIZE++;
    }
}



void validateTdsDeclaration(char* declarationName, EnvController* controller){
    TableEntry* expected_entry =  lookup(controller->originalPorts,declarationName);
    if(expected_entry){
        Object* info = expected_entry->val;
        int* isDeclared = info->values[0];
        if(!(*isDeclared)){
            controller->validPorts++;
            *isDeclared = 1;
        }
        else{
            controller->multiPortDeclartion = 1;
            fprintf(stderr,"ERROR: Same TDS redeclared. Name: %s\n",declarationName);
            exit(-1);
        }
    }
}

/**
 * Método auxiliar que para cada header de autta passado atualiza a referencia de sua variavel que tenha filter e/ou
 * atualiza a referencia em main. Colocando um parametro novo nessa referencia
 * @param controller o controlador de ambiente
 * @param paramName a string do parametro
 * @param automata o header do automato
 */
void addParamToAutomata(EnvController* controller, char* paramName, HeaderSmv* automata){
    if(addParamToModule(automata,paramName)){
        // mudar o main (se for o final automata) e também o ponto de interesse
        if(controller->modelHasFinalAutomata && strstr(automata->moduleName,"final")){
            addParamInterestPointHeader(accessHeader(controller,MAIN,-1), paramName,1);
            addParamInterestPointHeader(automata,paramName,0);
        }
        // só tem um filter
        if(!controller->modelHasFinalAutomata){
            addParamInterestPointHeader(accessHeader(controller,MAIN,-1),paramName,1);
        }
    }
}

void addParamToAutomatasFilter(EnvController* controller, char* paramName){
    // deve adicioanr a todos os automatos esse parâmetro, e deve atualizar também as referencias desses no main e no final automata
    int i;
    for (i = 0; i < controller->F_AUTOMATAS_CHANGE_POINTER; i++) {
        HeaderSmv* automata =  accessHeader(controller,AUTOMATA,i);
        addParamToAutomata(controller,paramName,automata);
    }
}

void addParamToTds(EnvController* controller, char* param, TDS* currentTDS){
    addParamToPortsModule(controller,param);
    // adiciona a TDS (tem uma variável de retorno dizendo se foi necessário adicionar esse parâmetro ou não
    HeaderSmv* headerTDS = accessHeader(controller,PORTS,currentTDS->SMV_REF);
    int paramAdd = addParamToModule(headerTDS,param);
    if(paramAdd){
        // DEVE AGORA PROPAGAR A DEPENDENCIA PARA A DECLARAÇÃO DELE EM PORTS MODULE
        // recuperar o indice da declaração
        STable* portsAux = accessSmvInfo(controller, PORTS, 0);
        TableEntry* infoOfTdsDec = lookup(portsAux,currentTDS->name);
        int pos = *(int*) infoOfTdsDec->val->values[0];
        int size = *(int*) infoOfTdsDec->val->values[1];
        HeaderSmv* portsHeader =  accessHeader(controller,PORTS,0);
        char* bufferToUpdate = portsHeader->varBuffer[pos];
        char* newNameDeclaration = addParams(bufferToUpdate, param, "(", ")", 0);
        free(bufferToUpdate);
        portsHeader->varBuffer[pos] = newNameDeclaration;
        size = strlen(newNameDeclaration);
        void* vp[] = {&size};
        updateValue(currentTDS->name,vp,WRITE_SMV_INFO,1,1,-1,portsAux,-1);
    }
}

/**
 * Adiciona parâmetro para módulo qualquer do nuXmv.
 * @param controller o controlador de ambiente e contexto
 * @param param a string do parâmetro
 * @param cat a categoria do header/tabela auxuliar a ser recuperada
 * @param indexOfHeader o indice do header
 * @return 1 se a operação foi realizada com sucesso (isto é foi necessária)
 * ou 0 caso a operação não tenha sido realizada (o módulo já possui o parâmetro)
 */
void addParamToPortsModule(EnvController *controller, char *param) {

    HeaderSmv* portsModuleHeader = accessHeader(controller,PORTS,0);
    int paramAdd = addParamToModule(portsModuleHeader,param); // deve adicionar ao portsModule É SEMPRE O PRIMEIRO
    if(paramAdd){
        // agora deve propagar as alterações para todos os demais módulos
        // OU SEJA JÁ QUE ESTÁ CENTRALIZADO NO MAIN (posição 3 do buffer de VAR), deve-se substituir esse
        HeaderSmv* mainUpdate = accessHeader(controller,MAIN,-1);
        char* refOldPt;
        refOldPt = mainUpdate->varBuffer[mainUpdate->VAR_RENAME_POINTER];
        char* newDeclaration = addParams(refOldPt, param, "(", ")", 0);
        free(refOldPt);
        mainUpdate->varBuffer[mainUpdate->VAR_RENAME_POINTER] = newDeclaration;

//    if(!first){
//        propagParamDependence(controller->MAIN_RELATED,param,controller->H_MAIN_CURRENT_SIZE);
//    }
//    propagParamDependence(controller->AUTOMATA_RELATED,param,controller->H_AUTOMATA_CURRENT_SIZE);
    }
}


char* getTypeSetWordFromDict(char* wordRef, EnvController* controller){
    int hashWord = hash(wordRef,TYPE_SET_DIR_SIZE);
    if(controller->typeSetWords[hashWord]){
        return controller->typeSetWords[hashWord];
    }
    else{
        // cria uma entrada para essa
        addTypeSetWordToDict(wordRef,controller);
        return controller->typeSetWords[hashWord];
    }
    //return NULL; // garantia (até porque na verdade já preenchemos com null antes)
}

void addTypeSetWordToDict(char* word, EnvController* controller){
    int hashWord = hash(word,TYPE_SET_DIR_SIZE);
    if(controller->typeSetWords[hashWord]){
        if(strcmp(controller->typeSetWords[hashWord],word) != 0){
            fprintf(stderr,"[addTypeSetWordToDict] DICT WORD COLLISION!\n");
            exit(-1);
        }
    }
    else{
        //char* newWord = formatString(word);
        char* newWord = malloc(sizeof(char)*strlen(word)+1);
        strcpy(newWord,word);
        controller->typeSetWords[hashWord] = newWord;
    }
}

void validateAfterInterPost(EnvController* controller){
        if(!controller->declaredPortsNumber){
            fprintf(stderr, "\n[WARNING] THE MODEL GENERATION WAS SUCCESSFUL, HOWEVER, NO VALID TDS DEFINITION WAS FOUND.\nIT IS RECOMMENDED THAT YOU REVIEW YOUR .tds FILE\n");
        }
        else{
            if(controller->validPorts != controller->expectedPorts){
                if(controller->validPorts){
                    fprintf(stderr, "\n[WARNING] THE MODEL GENERATION WAS SUCCESSFUL,\nHOWEVER, OF THE %d PORTS DECLARED ONLY %d of %d PORTS WERE COMPLIANT TO THE ORIGINAL MODEL.\nIT IS RECOMMENDED THAT YOU REVIEW YOUR .tds FILE.\n",
                            controller->declaredPortsNumber, controller->validPorts,controller->expectedPorts);
                }
                else{
                    fprintf(stderr, "\n[WARNING] THE MODEL GENERATION WAS SUCCESSFUL,\nHOWEVER, OF THE %d PORTS DECLARED NONE WERE COMPLIANT TO THE ORIGINAL MODEL.\nIT IS RECOMMENDED THAT YOU REVIEW YOUR .tds FILE.\n",
                            controller->declaredPortsNumber);
                }
            }
        }
    if(!controller->IO_RELATION){
        fprintf(stderr, "\n[WARNING] NO TDS RELATIONSHIP WAS DECLARED. IT IS RECOMMENDED THAT YOU REVIEW YOUR .tds FILE.\n");
    }
    if(controller->modelHasFilter == 1 && !controller->filterUsed){
        fprintf(stderr, "\n[WARNING] FILTER WAS USED IN THE ORIGINAL MODEL, HOWEVER, NO FILTER RELATIONSHIP WAS DECLARED. \n IT IS RECOMMENDED THAT YOU REVIEW YOUR .tds FILE.\n");
    }
    else{
        if(!controller->filterUsed){
            fprintf(stderr, "\n[WARNING] FILTER WAS NOT USED IN THE ORIGINAL MODEL. \n IT IS RECOMMENDED THAT YOU REVIEW YOUR .tds FILE.\n");
        }
    }
}
