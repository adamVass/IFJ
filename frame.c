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
		ptrTables[i].stackItem = NULL;
	}
	return S_BEZ_CHYB;
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
		InitList( &(func[i].instrList) );
		func[i].table = NULL;
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
	InitList( &(ptrStack->top->field) );
	//vytvoreni ht
	int ret = htInit( &ptrhtLocal );
	if( ret != S_BEZ_CHYB ){
		return S_INTERNI_CHYBA;
	}
	//nastaveni pointeru
	listIntrukci = &(ptrStack->top->field);
	//prirazeni mainu do tables
	ptrTables[currentSize++].stackItem = ptrStack->top;
	//prirazeni ht do ramce
	ptrStack->top->ptrht = ptrhtLocal;
	ptrhtLastLocal = NULL; 
	return S_BEZ_CHYB;
}
//inicializace listu instrukci
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
	if( currentSize == fieldSize ){
		int ret = resizeTableField();
		if( ret == S_INTERNI_CHYBA ){
			return ret;
		}
	}
	//pridani ramce do pole tabulek
	ptrTables[currentSize].stackItem = tmp;
	//vytvoreni listu instrukci pro ramec
	InitList(&(ptrTables[currentSize].stackItem->field));
	listIntrukci = &(ptrTables[currentSize++].stackItem->field);

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
	//posunuti vrcholu
	ptrStack->top = ptrStack->top->lower;
	//nova ht v local ukazateli
	ptrhtLocal = ptrStack->top->ptrht;
	listIntrukci = &(ptrStack->top->field);
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
	TTableField *tmp = (TTableField*)malloc(sizeof(TTableField)*fieldSize); 
    if( !tmp ){
		return S_INTERNI_CHYBA;
    }
	for (int i = 0; i < currentSize; ++i){
		tmp[i] = ptrTables[i];
	}
	free(ptrTables);
	ptrTables = tmp;
	return S_BEZ_CHYB;
}

tChyba resizeFuncField()
{
	funcSize = 2*funcSize;
	TFunction *tmp = (TFunction*)malloc(sizeof(TFunction)*funcSize); 
    if( !tmp ){
		return S_INTERNI_CHYBA;
    }
	for (int i = 0; i < currFuncSize; ++i){
		tmp[i] = func[i];
	}
	free(func);
	func = tmp;
	return S_BEZ_CHYB;
	
}

tChyba copyTable( TFunction *source )
{
	TItem *tmp;
	int ret;
	for (int i = 0; i < MAX_HTSIZE; ++i){
		tmp = (*(source->table))[i];
		if( tmp ){
			//dmnce karlova
			if( tmp->druh == ID_FUNCTION ){
				ret = htInsert( ptrhtLocal, tmp->key, copyData( tmp->druh, tmp->data), tmp->type, tmp->druh );
			}
			else if( tmp->druh == ID_PARAM ){
				ret = htInsert( ptrhtLocal, tmp->key, NULL, tmp->type, tmp->druh );
			}
			else ret = htInsert( ptrhtLocal, tmp->key, copyData( tmp->type, tmp->data), tmp->type, tmp->druh );

			if(ret != S_BEZ_CHYB) {
				return ret;
			}
			//ret = htCompleteInsert(ptrhtLocal, tmp->key, tmp->druh, tmp->type, tmp->init);
			//if(ret != S_BEZ_CHYB) {
			//	return ret;
			//}
		}
	}
	//zkopiruje cely list instrukci
	ret = CopyList( &(source->instrList) );
	if(ret != S_BEZ_CHYB) {
		return ret;
	}
	//ustanoveni dat
	tItemPtr ptr = listIntrukci->First;
	while(ptr){
		TItem *item;
		if( ptr->instruction.instructionType == OC_COLL ){
			ptr->instruction.instructionType = OC_CALL;
			//nastaveni navratovky
			item = (TItem*)ptr->instruction.address1;
			ptr->instruction.address3 = htSearch( ptrhtLocal, item->key );
		}
		else if( ptr->instruction.instructionType == OC_NEPRIRAZENI ){
			ptr->instruction.instructionType = OC_PRIRAZENI;
			//navratovka je zdroj
			ptr->instruction.address1 = item;
			//htSearch( ptrhtLastLocal, id )
			//dodelat nalezeni cile ulozeni
		}
		else{
			if( ptr->instruction.address1 ){
				TItem *tmp1 = (TItem*)ptr->instruction.address1;
				//write
				if( tmp1->druh != PRINTSTR ){
					ptr->instruction.address1 = htSearch( ptrhtLocal, tmp1->key );
				}
			}
			if( ptr->instruction.address2 ){
	        	TItem *tmp2 = (TItem*)ptr->instruction.address2;
				ptr->instruction.address2 = htSearch( ptrhtLocal, tmp2->key );
			}
			if( ptr->instruction.address3 ){
	        	TItem *tmp3 = (TItem*)ptr->instruction.address3;
				ptr->instruction.address3 = htSearch( ptrhtLocal, tmp3->key );
			}
		}
		ptr = ptr->nextItem;
	}
	return S_BEZ_CHYB;
}

tChyba addArgument( char *key )
{
	//kopirovani argumentu
	//nalezeni funkce v globalce
	TItem *item = htSearch( ptrhtGlobal, porovnani );
	if(!item){
		return S_SEMANTICKA_CHYBA_NEDEF;
	}
	//projiti parametru funkce v jeji deklaraci
	for (int i = 0; i < item->data->param.numParam; ++i){
		//nalezeni paramtru ve funkci
		TItem *item2 = htSearch( ptrhtLocal, item->data->param.param[i] );
		if( item2 ){
			//nalezeni data argumentu v predchozi funkci
			TItem *item3 = searchFrames(key, ptrhtLastLocal, ptrhtGlobal );
			if( item3 ){
				item2->data = item3->data;
			}
			else{
				item2->data = createData( token.stav, key ); //nenalezeno, zadana raw hodnota
			}
			item2->init = true;
			return S_BEZ_CHYB;
		}
	}
	return S_SEMANTICKA_CHYBA_NEDEF;
}

TFunction* searchFuncTable( char *key )
{
	for (int i = 0; i < currFuncSize; ++i){
		if(  !strcmp( key, func[i].key ) ){
			return &func[i];
		}
		 
	}
	return NULL;
}


TData* createData( int type, char *hodnota )
{

	TData *tmp = (TData*)malloc(sizeof(TData));

	//bool
	if( type == s_logicka_hodnota ){
		if( !strcmp( hodnota, "true" ) ){
			tmp->boolValue = true;
		}
		else tmp->boolValue = false;
	}
	else if( type == s_cele_cislo ){  //int
		tmp->intNumber = atoi(hodnota);
	}
	else if( type == s_desetinne_cislo ){ //double
		tmp->floatNumber = atof(hodnota);
	}
	else if( type == s_string ){
		tmp->str = allocString(hodnota);
	}
	return tmp;
}