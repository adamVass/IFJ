#include "ial.h"

int HTSIZE = MAX_HTSIZE;

tChyba htCompleteInsert( tHTable *ptrht, char *key, int druh, int type )
{
	TItem *tmp = (*ptrht)[hashCode(key)];
	if( !tmp ){
		return S_INTERNI_CHYBA;
	}
	tmp->druh = druh;
	tmp->type = type;
	return S_BEZ_CHYB; 
}

tChyba htParamInsert( tHTable *ptrht, char *key, char *param, int type )
{
	if( !ptrht || !param || !key ){
		return S_INTERNI_CHYBA;
	}
	//najdeme funkci
	TItem *tmp = (*ptrht)[hashCode(key)];
	if( !tmp ){
		return S_INTERNI_CHYBA;
	}
	//+1 parametr
	tmp->data.param.numParam++;
	//alokuju vetsi potrebne pole parametru a typu
	char **tmpParam = (char**)malloc( sizeof(char*)*tmp->data.param.numParam );
	int *tmpTypeParam = (int*)malloc( sizeof(int)*tmp->data.param.numParam );
	//prekopiruju
	for (int i = 0; i < tmp->data.param.numParam - 1 ; ++i){
		tmpParam[i] = tmp->data.param.param[i];
		tmpTypeParam[i] = tmp->data.param.typeParam[i];
	}
	// kopie parametru - char *param 
	char *tmpChar = (char*)malloc( strlen(param)+1 );
	strcpy( tmpChar, param );
	//pridani noveho parametru a jeho typu 
	tmpParam[tmp->data.param.numParam-1] = tmpChar;
	tmpTypeParam[tmp->data.param.numParam-1] = type;
	//uvolneni starych poli
	if( tmp->data.param.numParam != 1 ){
		free(tmp->data.param.param);
		free(tmp->data.param.typeParam);
	}
	//predani pointru
	tmp->data.param.param = tmpParam;
	tmp->data.param.typeParam = tmpTypeParam;
	return S_BEZ_CHYB;
}

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( char *key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

tChyba htInit( tHTable **ptrht ){
	*ptrht = (tHTable*) malloc ( sizeof(tHTable) );
	if( !ptrht ){
		return S_INTERNI_CHYBA;
	}
	//inicializace celé tabulky
	for (int i = 0; i < HTSIZE; ++i){
		(*(*ptrht))[i] = NULL;
	}
	return S_BEZ_CHYB;
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není,
** vrací se hodnota NULL.
**
*/

TItem* htSearch( tHTable *ptrht, char *key ) {

	if( !ptrht || !key ){
		return NULL;
	}

	TItem *tmp = (*ptrht)[hashCode(key)];
	while( tmp ){
		//klíč je nalezen
		if(!strcmp( tmp->key, key )){
			return tmp;
		}
		tmp = tmp->ptrnext;
	}
	return NULL;
}

//novej htInsert

tChyba htDeclInsert( tHTable *ptrht, char *key, int type, int druh ) {

	if( !ptrht || !key ){
		return S_INTERNI_CHYBA;
	}
	
	TItem *new = (TItem*)malloc(sizeof(struct TItem));
	if( !new ){
		return S_INTERNI_CHYBA;
	}
	new->key = malloc(strlen(key)+1);

	strcpy(new->key, key);
	new->type = type;
	new->druh = druh;
	int hashKey = hashCode(key);
	new->ptrnext = (*ptrht)[hashKey];
	(*ptrht)[hashKey] = new;
	return S_BEZ_CHYB;
}


/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

tChyba htInsert ( tHTable *ptrht, char *key, TData data, int type, int druh ) {

	if( !ptrht || !key ){
		return S_INTERNI_CHYBA;
	}
	
	TItem *new = (TItem*)malloc(sizeof(struct TItem));
	if( !new ){
		return S_INTERNI_CHYBA;
	}
	new->key = malloc(strlen(key)+1);
	strcpy(new->key, key);
	
	if( type == 0 ){
		
		new->data.str = malloc( strlen(data.str)+1 );
		if( !new->data.str ){
			return S_INTERNI_CHYBA;
		}
		strcpy( new->data.str, data.str );
	}
	else new->data = data;
	new->type = type;
	new->druh = druh;
	int hashKey = hashCode(key);
	new->ptrnext = (*ptrht)[hashKey];
	(*ptrht)[hashKey] = new;
	return S_BEZ_CHYB;
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

TData* htRead( tHTable *ptrht, char *key ) {

	if( !ptrht || !key ){
		return NULL;
	}

	TItem *tmp = htSearch( ptrht, key );
	if( !tmp ){
		return NULL;
	}
	return &tmp->data;
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

tChyba htDelete( tHTable *ptrht, char *key ) {

	if( !ptrht || !key ){
		return S_INTERNI_CHYBA;
	}

	int hashKey = hashCode(key);

	TItem *tmp = (*ptrht)[hashKey];
	TItem *tmp2 = tmp;
	//prochazeni polozkami
	while( tmp ){
		// položka nalezena
		if( tmp->key == key ){
			//položka je jako první
			if( tmp == (*ptrht)[hashKey] ){
				(*ptrht)[hashKey] = (*ptrht)[hashKey]->ptrnext;
			}
			else{
				tmp2->ptrnext = tmp2->ptrnext->ptrnext;
			}
			free( tmp );
			return S_BEZ_CHYB;
		}
		tmp2 = tmp;
		tmp = tmp->ptrnext;
	}
	return S_BEZ_CHYB;
}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

tChyba htClearAll( tHTable *ptrht ) {

	if( !ptrht ){
		return S_INTERNI_CHYBA;
	}
	//prochazí tabulku
 	for (int i = 0; i < HTSIZE; ++i){
		TItem *tmp;
		//prochazí zřetězené položky
 		while( (*ptrht)[i] ){
 			tmp = (*ptrht)[i];
 			(*ptrht)[i] = (*ptrht)[i]->ptrnext;
 			if( tmp->type == 0 ){
 				free( tmp->data.str );
 			}
 			free( tmp->key );
 			free( tmp );
 		}
 	}
 	return S_BEZ_CHYB;
}
