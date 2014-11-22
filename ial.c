
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

int HTSIZE = MAX_HTSIZE;

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

void htInsert ( char *key, TData data, int type ) {

	if( !ptrht || !key ){
		return;
	}
	//hledani podle klice
	TItem *tmp = htSearch( key );
	//prvek nalezen, aktualizace datové části
	if( tmp ){
		tmp->data = data;
		return;
	}
	//prvek nebyl nalezen, musí být vytvořen
	else{
		TItem *new = (TItem*)malloc(sizeof(struct TItem));
		new->key = malloc(strlen(key)+1);
		strcpy(new->key, key);
		new->data = data;
		new->type = type;
		int hashKey = hashCode(key);
		new->ptrnext = (*ptrht)[hashKey];
		(*ptrht)[hashKey] = new;
	}
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
