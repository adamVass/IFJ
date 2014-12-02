#include "frame.h"

tChyba initFrames()
{
	//globalni ht
	int ret = htInit( &ptrhtGlobal );
	if( ret != S_BEZ_CHYB ){
		return S_INTERNI_CHYBA;
	}
	//init table
	ret = initTable();
	if( ret != S_BEZ_CHYB ){
		return S_INTERNI_CHYBA;
	}
	//init func
	ret = initFunction();
	if( ret != S_BEZ_CHYB ){
		return S_INTERNI_CHYBA;
	}
	//inicializace zasobniku lokalnich ramcu a lokalniho ramce
	return initStack();
}

//inicializace pole funkci
tChyba initFunction()
{
	funcSize = 10;
	currFuncSize = 0;
	func = (TFunction*)malloc(sizeof(TFunction)*funcSize);
	if( !func ){
		return S_INTERNI_CHYBA;
	}
	for (int i = 0; i < funcSize; ++i){
		func[i].key = NULL;
		func[i].table = NULL;
	}
	return S_BEZ_CHYB;
}

//inicializace pole instrukci
tChyba initTable()
{
	fieldSize = 10;
	currentSize = 0;
	ptrTables = (TTableField*)malloc(sizeof(TTableField)*fieldSize);
	if( !ptrTables ){
		return S_INTERNI_CHYBA;
	}
	for (int i = 0; i < fieldSize; ++i){
		ptrTables[i].field = NULL;
		ptrTables[i].table = NULL;
	}
	return S_BEZ_CHYB;
}

tChyba initStack()
{
	//vytvoreni zasobniku
	ptrStack = (TStack*) malloc ( sizeof(TStack) );
	if( !ptrStack ){
		return S_INTERNI_CHYBA;
	}
	//vytvoreni prvniho ramce v zasobniku
	ptrStack->top = (TStackItem*)malloc( sizeof(TStackItem) );
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
	ptrhtLastLocal = NULL; 
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
	//ptr na posledni lokalni ramec
	ptrhtLastLocal = ptrStack->top->ptrht;
	//ustanoveni noveho vrcholu
	ptrStack->top = tmp;
	return S_BEZ_CHYB;
}


//maze ramec ze zasobniku a do frame predava ukazatel na dalsi ramec do ptrhtLocal
tChyba popFrame()
{
	//uchovani ukazatele do TableFields
	if( currentSize == fieldSize ){
		int ret = resizeTableField();
		if( ret == S_INTERNI_CHYBA ){
			return ret;
		}
	}
	//pridani odebiraneho ramce do pole tabulek
	ptrTables[currentSize++].table = ptrStack->top->ptrht;
	//posunuti vrcholu
	ptrStack->top = ptrStack->top->lower;
	//nova ht v local ukazateli
	ptrhtLocal = ptrStack->top->ptrht;
	return S_BEZ_CHYB;
}


TItem* searchFrames( char *key, tHTable *ptrLocal, tHTable *ptrGlobal )
{
	TItem *tmp = htSearch( ptrLocal, key );
	if( !tmp ){
		tmp = htSearch( ptrGlobal, key );
	}
	return tmp;
}

tChyba resizeTableField()
{
	fieldSize = 2*fieldSize;
	ptrTables = (TTableField*)realloc(ptrTables, fieldSize);
    if( !ptrTables ){
		return S_INTERNI_CHYBA;
    }
	return S_BEZ_CHYB;
}

tChyba resizeFuncField()
{
	funcSize = 2*funcSize;
	func = (TFunction*)realloc(func, funcSize);
    if( !func ){
		return S_INTERNI_CHYBA;
    }
	return S_BEZ_CHYB;
	
}



tChyba copyTable( tHTable *source, tHTable *dest )
{
	TItem *tmp;
	for (int i = 0; i < MAX_HTSIZE; ++i){

		tmp = (*source)[i];
		if( tmp ){
			int ret;
			if( tmp->type == TYPESTR ){
				TData data;
				data.str = allocString( tmp->data.str );
				ret = htInsert( dest, tmp->key, data, tmp->type, tmp->druh );
				if(ret != S_BEZ_CHYB) {
					return ret;
				}
			}
			else if( tmp->type == ID_FUNCTION ){
				ret = htInsert( dest, tmp->key, tmp->data, tmp->type, tmp->druh );
				if( ret == S_INTERNI_CHYBA ){
					return ret;
				}
				tmp->data.param.numParam = 0;
				for (int i = 0; i < tmp->data.param.numParam; ++i){
					ret = htParamInsert( dest, tmp->key, tmp->data.param.param[i], tmp->data.param.typeParam[i] );
					if( ret == S_INTERNI_CHYBA ){
						return ret;
					}
				}
			}
			else{
				htInsert( dest, tmp->key, tmp->data, tmp->type, tmp->druh );	
			}
			ret = htCompleteInsert(dest, tmp->key, tmp->druh, tmp->type, tmp->init);
			if(ret != S_BEZ_CHYB) {
				return ret;
			}
		}
	}
	return S_BEZ_CHYB;
}



/*

if( currFuncSize == funcSize ){
	resizeFuncField();
}
func[currFuncSize]->key = allocstr(param);


tHTable *tmp = func[currFuncSize]->table;
currFuncSize++;
htInit(&tmp);
copyTable( ptrhtLocal, tmp )

tHTable *tmp 
for (int i = 0; i < currFuncSize; ++i){
	if ( !strcmp( func[i], "key" )){
		tmp = func[i]->table;
		break;
	}
}
*/
