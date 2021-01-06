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

Object *eval(Node *n, STable *scope, HeaderController *controllerSmv);


#endif

