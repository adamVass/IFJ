#include "frame.h"

tChyba initFrames()
{
	//globalni ht
	int ret = htInit( &ptrhtGlobal );
	if( ret != S_BEZ_CHYB ){
		return S_INTERNI_CHYBA;
	}
	//inicializace zasobniku lokalnich ramcu a lokalniho ramce
	return initStack();
}

tChyba initStack()
{
	//vytvoreni zasobniku
	ptrStack = (TStack*) malloc ( sizeof(TStack) );
	if( !ptrStack ){
		return S_INTERNI_CHYBA;
	}
	//vytvoreni prvniho ramce v zasobniku
	ptrStack->top = (TStackItem*)malloc( sizeof(TStackItem) );;
	if( !ptrStack->top ){
		return S_INTERNI_CHYBA;
	}
	ptrStack->top->lower = NULL;
	//vytvoreni ht
	int ret = htInit( &ptrhtLocal );
	if( ret != S_BEZ_CHYB ){
		return S_INTERNI_CHYBA;
	}
	//prirazeni ht do ramce
	ptrStack->top->ptrht = ptrhtLocal;
	return S_BEZ_CHYB;
}

tChyba pushFrame()
{
	//novy ramec
	TStackItem *tmp = (TStackItem*)malloc( sizeof(TStackItem) );
	if( !tmp ){
		return S_INTERNI_CHYBA;
	}
	//inicializace nove ht
	int ret = htInit( &ptrhtLocal );
	if( ret != S_BEZ_CHYB ){
		return S_INTERNI_CHYBA;
	}
	//prirazeni ht do ramce
	tmp->ptrht = ptrhtLocal;
	//prirazeni vrcholu zasobniku pod novy ramec
	tmp->lower = ptrStack->top;
	//ustanoveni noveho vrcholu
	ptrStack->top = tmp;
	return S_BEZ_CHYB;
}


//maze ramec ze zasobniku a do frame predava ukazatel na dalsi ramec do ptrhtLocal
tChyba popFrame()
{
	//docasny ukazatel na mazany ramec
	TStackItem *tmp = ptrStack->top;
	//posunuti vrcholu
	ptrStack->top = ptrStack->top->lower;
	//nova ht v local ukazateli
	ptrhtLocal = ptrStack->top->ptrht;
	int ret = htClearAll( tmp->ptrht );
	if( ret == S_INTERNI_CHYBA ){
		return S_INTERNI_CHYBA;
	}
	free(tmp);
	return S_BEZ_CHYB;
}


TItem* searchFrames( char *key )
{

	TItem *tmp = htSearch( ptrhtLocal, key );
	if( !tmp ){
		tmp = htSearch( ptrhtGlobal, key );
	}
	return tmp;
}