//
// Created by mateus on 28/10/2020.
//

#ifndef INTERP_H

#define INTERP_H

#include "textManager.h"
#include "HeaderSmv.h"
#include "STable.h"
#include "Node.h"
#include "Object.h"

Object * eval(Node* node, STable* scope, STable** writeSmvTypeTable, HeaderController* controllerSmv,int notEvaluate);


#endif

