#ifndef _FRAME_H_
#define _FRAME_H_

#include "ial.h"
#include "syntakticka_analyza.h"

typedef struct TFunction
{
	char *key;
	tHTable *table;
	tList instrList;
}TFunction;

typedef struct TStackItem{
	tHTable *ptrht;
	tList field;	//pole instrukci...
	struct TStackItem *lower;
}TStackItem;

typedef struct TStack{
    TStackItem *top;
} TStack;

typedef struct TTableField{
	TStackItem *stackItem;
}TTableField;

TStack *ptrStack;
TTableField *ptrTables;
int fieldSize, currentSize;
TFunction *func;
int funcSize, currFuncSize;

tChyba initTable();

tChyba initFunction();

tChyba initFrames();

tChyba initStack();

tChyba pushFrame();

tChyba popFrame();

TItem* searchFrames( char *key, tHTable *ptrLocal, tHTable *ptrGlobal );

tChyba addArgument( char *key );

tChyba resizeTableField();

tChyba resizeFuncField();

tChyba copyTable( TFunction *source );

TFunction* searchFuncTable( char *key );

TData* createData( int type, char *hodnota );




#endif
