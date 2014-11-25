
/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2013
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

#include "ial.h"
#include <stdio.h>

int HTSIZE = MAX_HTSIZE;

int htCompleteInsert( char *key, int druh, int type )
{
	TItem *tmp = (*ptrht)[hashCode(key)];
	if( !tmp ){
		return -1;
	}
	tmp->druh = druh;
	tmp->type = type;
	return 0; 
}

int htParamInsert( char *key, char *param, int type )
{
	if( !ptrht || !param ){
		return -1;
	}
	//najdeme funcki
	TItem *tmp = (*ptrht)[hashCode(key)];
	if( !tmp ){
		return -1;
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
	return 0;
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

void htInit () {
	ptrht = (tHTable*) malloc ( sizeof(tHTable) );
	if( !ptrht ){
		return;
	}
	//inicializace celé tabulky
	for (int i = 0; i < HTSIZE; ++i){
		(*ptrht)[i] = NULL;
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není,
** vrací se hodnota NULL.
**
*/

TItem* htSearch ( char *key ) {

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

void htDeclInsert ( char *key, int type, int druh ) {

	if( !ptrht || !key ){
		return;
	}
	
	TItem *new = (TItem*)malloc(sizeof(struct TItem));
	new->key = malloc(strlen(key)+1);

	strcpy(new->key, key);
	new->type = type;
	new->druh = druh;
	int hashKey = hashCode(key);
	new->ptrnext = (*ptrht)[hashKey];
	(*ptrht)[hashKey] = new;
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

void htInsert ( char *key, TData data, int type, int druh ) {

	if( !ptrht || !key ){
		return;
	}
	
	TItem *new = (TItem*)malloc(sizeof(struct TItem));
	new->key = malloc(strlen(key)+1);

	strcpy(new->key, key);
	
	if( type == 0 ){
		
		new->data.str = malloc( strlen(data.str)+1 );
		
		if( !new->data.str ) return; // checky dodelat
		strcpy( new->data.str, data.str );
	}
	else new->data = data;
	new->type = type;
	new->druh = druh;
	int hashKey = hashCode(key);
	new->ptrnext = (*ptrht)[hashKey];
	(*ptrht)[hashKey] = new;
}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

TData* htRead ( char *key ) {

	if( !ptrht || !key ){
		return NULL;
	}

	TItem *tmp = htSearch( key );
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

void htDelete ( char *key ) {

	if( !ptrht || !key ){
		return;
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
			return;
		}
		tmp2 = tmp;
		tmp = tmp->ptrnext;
	}
}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll () {

	if( !ptrht ){
		return;
	}
	//prochazí tabulku
 	for (int i = 0; i < HTSIZE; ++i){
		TItem *tmp;
		//prochazí zřetězené položky
 		while( (*ptrht)[i] ){
 			tmp = (*ptrht)[i];
 			(*ptrht)[i] = (*ptrht)[i]->ptrnext;
 			free( tmp );
 		}
 	}
}
