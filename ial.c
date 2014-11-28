#include "ial.h"

int HTSIZE = MAX_HTSIZE;

tChyba htCompleteInsert( tHTable *ptrht, char *key, int druh, int type, bool init )
{
	TItem *tmp = (*ptrht)[hashCode(key)];
	if( !tmp ){
		return S_INTERNI_CHYBA;
	}
	tmp->druh = druh;
	tmp->type = type;
	tmp->init = init;
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

/** BUILTIN FUNCTIONS */

char *strCopy(char *str, unsigned int s, unsigned int n)
{
/** Returns a string which is a copy of str, starting at index s - 1, n characters long.
* @param *str Source string
* @param s Start index
* @param n Number of characters to be copied
* @return Pointer to the created substring
*/
	char *result;
	unsigned int len = strlen(str);
	if (s > len)
	{
		return EMPTY_STRING;	/* Behavior mimicking Pascal's Copy function */
	}

	if (n > len) n = len - s + 1;	/* Size of resulting string can't be greater than length of original - starting index + 1 (indexing is from 1, not 0) */

	if ((result = (char*)malloc(n * sizeof(char))) == NULL)
	{
		fprintf(stderr, "strCopy memory allocation error\n");
		return NULL;
	}

	strncpy(result, str + s - 1, n);	/* (str + s) - to begin from start index, -1 as indexing starts from 1 */
	return result;
}

char *strSort(char *str)
{
/** Define left and right bound, verify string's not empty
* @param *str Array of characters to be sorted
* @return Pointer to sorted array
*/
	int right = strlen(str) - 1;
	if (right > 0)
		quickSort(str, 0, right); /* left = 0 */

	return str;
}

void quickSort(char *str, int left, int right)
{
/** Recursive quick sort implementation
* @param *str Array of characters to be sorted
* @param left Left bound of the array
* @param right Right bound of the array
*/
	int i = left, j = right;
	int pivot = str[(i + j)/2]; /* Pivot defined as pseudo median */
	int temp;

	do
	{
		while (str[i] < pivot)
			i++;		/* Incrementing until first i > pivot is found (from the left)*/

		while (str[j] > pivot)
			j--;		/* Decremeting until first j < pivot is found (from the right)*/

		if (i <= j)
		{
			temp = str[i];
			str[i] = str[j];
			str[j] = temp;		/* Swapping found elements */
			i++;
			j--;
		}
	} while (i < j);

	if (right > i)
		quickSort(str, i, right);	/* Recursion to the right */
	if (j > left)
		quickSort(str, left, j);	/* Recursion to the left */
}

int kmp(char *text, char *pattern)
{
/** Searches for substring using Knuth-Morris-Pratt algorithm.
* @param *text String to be searched in
* @param *pattern Searched pattern within string
* @return Index (int) to the beginning of substring within *text or 0 if not found. Using Pascal indexing (from 1)
*/
	if (strcmp(pattern, EMPTY_STRING) == 0)
		return 1;	/* Empty string is found at first position for any string (Pascal) */
	int tSize = strlen(text), pSize = strlen(pattern);
	int tIndex, pIndex;
	int *fail = kmpGraph(pattern, pSize);	/* Get 'fail' vector */
	if (fail == NULL)
		return -1;	/* Should malloc fail, return -1 */
	pIndex = -1;
	for (tIndex = 0; tIndex < tSize; tIndex++)
	{
		while (pIndex > -1 && pattern[pIndex + 1] != text[tIndex])
			pIndex = fail[pIndex];
		if (text[tIndex] == pattern[pIndex + 1])
			pIndex++;
		if (pIndex == pSize - 1)
		{
			free(fail);	
			return tIndex - pIndex + 1;		/* +1 because of Pascal indexing */
		}
	}

	free(fail);
	return 0;
}

int *kmpGraph(char *pattern, int pSize)
{
/** Creates vector fail, required for kmp function.
* @param *pattern Searched pattern within string
* @param pSize Size of pattern array (in bytes, as it's composed of chars)
* @return Pointer to created vector - fail
*/
	int i, k;
	i = 1, k = -1;

	int *fail;
	if ((fail = ((int *)malloc(sizeof(int) * pSize))) == NULL)
		return NULL;

	fail[0] = k;	/* First element of fail vector is always 0 */
	
	while (i < pSize)
	{ /* Populate fail vector for <i, pSize - 1> */
		while (k > -1 && pattern[k + 1] != pattern[i])
			k = fail[k];
		if (pattern[i] == pattern[k+1])
			k++;
		
		fail[i] = k;
		i++;
	}

	return fail;
}

int strLength(char *str)
{ /* Obligatory */
	return strlen(str);		/* science */
}