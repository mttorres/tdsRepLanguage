#ifndef HEADER_H
#define HEADER_H

typedef struct headersmv
{
  long modulePointer;
  long varPointer;
  long assignPointer;
  long transPointer;
  long CURR_OFFSET;

}HeaderSmv;


HeaderSmv* createHeader(long moduleP, long varP, assignP, transP, long offset);

void printHeader(HeaderSmv* h);

void letgoHeader(HeaderSmv* h);



#endif
