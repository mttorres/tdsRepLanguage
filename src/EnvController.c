//
// Created by mateus on 22/03/2021.
//

#include "../headers/EnvController.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


EnvController *createController() {

    int DEFAULT_HEADERS_SIZE = 100;

    EnvController* Hcontrol = malloc(sizeof(EnvController));
    Hcontrol->H_AUTOMATA_CURRENT_SIZE = 0;
    Hcontrol->H_PORTS_CURRENT_SIZE = 0;
    Hcontrol->PORTS_INFO_CURRENT_SIZE = 0;
    Hcontrol->H_FUNCTION_CURRENT_SIZE = 0;

    Hcontrol->originalPorts = createTable(SMV_PORTS, NULL, 0, 0, -1);
    Hcontrol->mainInfo = createTable(SMV_V_MAIN, NULL, 0, 0, -1);
    Hcontrol->functionsInfo = malloc(sizeof(STable*)*DEFAULT_HEADERS_SIZE);
    Hcontrol->portsInfo = malloc(sizeof(STable*)*DEFAULT_HEADERS_SIZE);

    Hcontrol->MAIN = NULL;
    Hcontrol->AUTOMATA_RELATED = malloc(sizeof(HeaderSmv*)*DEFAULT_HEADERS_SIZE);
    Hcontrol->PORTS_RELATED = malloc(sizeof(HeaderSmv*)*DEFAULT_HEADERS_SIZE);
    Hcontrol->FUNCTIONS = malloc(sizeof(HeaderSmv*)*DEFAULT_HEADERS_SIZE);

    Hcontrol->expectedPorts = 0;
    Hcontrol->validPorts = 0;
    Hcontrol->declaredPortsNumber= 0;
    Hcontrol->IO_RELATION = 0;
    Hcontrol->multiPortDeclartion = 0;
    Hcontrol->declaredPorts = malloc(sizeof(TDS*)*DEFAULT_HEADERS_SIZE); // NULL até sabermos o intervalo de tempo


    int i;
    for (i = 0; i < DEFAULT_HEADERS_SIZE; i++) {
        Hcontrol->functionsInfo[i] = NULL;
        Hcontrol->portsInfo[i] =  NULL;
        Hcontrol->AUTOMATA_RELATED[i] = NULL;
        Hcontrol->PORTS_RELATED[i] = NULL;
        Hcontrol->FUNCTIONS[i] = NULL;
    }

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
    //letgoTable(H->portsInfo);
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
        toReturn = indexOfHeader != -1? controller->AUTOMATA_RELATED[indexOfHeader] : controller->AUTOMATA_RELATED[controller->H_AUTOMATA_CURRENT_SIZE-1];
    }
    if(cat == PORTS){
        toReturn = indexOfHeader != -1? controller->PORTS_RELATED[indexOfHeader] : controller->PORTS_RELATED[controller->H_PORTS_CURRENT_SIZE-1];
    }

    if(cat == FUNCTION_SMV){
        toReturn = indexOfHeader != -1? controller->FUNCTIONS[indexOfHeader] : controller->FUNCTIONS[controller->H_FUNCTION_CURRENT_SIZE-1];
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
        return SMV_INFO_ID > 0? controller->functionsInfo[SMV_INFO_ID] : controller->functionsInfo[controller->H_FUNCTION_CURRENT_SIZE-1];
    }
    if(SMV_INFO_ID < controller->H_PORTS_CURRENT_SIZE && cat == PORTS){
        return SMV_INFO_ID > 0? controller->portsInfo[SMV_INFO_ID] : controller->portsInfo[controller->PORTS_INFO_CURRENT_SIZE-1];
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

void addParamToPortsModule(EnvController *controller, char *param, int first) {
    HeaderSmv* updated = accessHeader(controller,PORTS,0); // O PORTS MODULE É SEMPRE O PRIMEIRO
    char* newName = addParams(updated->moduleName,param,"(",")");
    free(updated->moduleName);
    updated->moduleName = newName;
    // agora deve propagar as alterações para todos os demais módulos
    // OU SEJA JÁ QUE ESTÁ CENTRALIZADO NO MAIN (posição 3 do buffer de VAR), deve-se substituir esse

    HeaderSmv* mainUpdate = accessHeader(controller,MAIN,-1);

    char* refOldPt;
    refOldPt = mainUpdate->varBuffer[mainUpdate->VAR_RENAME_POINTER];
    char* newDeclaration = addParams(refOldPt,param,"(",")");
    free(refOldPt);
    mainUpdate->varBuffer[mainUpdate->VAR_RENAME_POINTER] = newDeclaration;

//    if(!first){
//        propagParamDependence(controller->MAIN_RELATED,param,controller->H_MAIN_CURRENT_SIZE);
//    }
//    propagParamDependence(controller->AUTOMATA_RELATED,param,controller->H_AUTOMATA_CURRENT_SIZE);
}

