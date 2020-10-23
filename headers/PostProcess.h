#ifndef POSP_H

#define POSP_H

#include "textManager.h"
#include "HeaderSmv.h"
#include "STable.h"
#include "Node.h"
#include "Object.h"




void* eval(Node* n, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv);


#endif

