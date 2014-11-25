#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_HTSIZE 61

#define TYPESTR      0
#define TYPEBOOL     1
#define TYPEINT      2
#define TYPEDOUBLE   3
#define TYPEFUNC     4

#define ID_FUNCTION  5
#define ID_LOCAL     6
#define ID_GLOBAL    7
#define ID_PARAM     8

#define TYPEUNDEF    10

typedef struct TParam{
   int numParam;     //pocet parametru - vynulovat pri vkladani funkce
   char **param;     //parametry
   int *typeParam;   //typy parametru
}TParam;

/* typ obsahu (například cena zboží) */
typedef union TData{
   char *str;  // nebo mozno udelat strukturu, ktera bude obsahovat retezes a jeho delku..
   bool boolValue;
   int intNumber;
   double floatNumber;
   TParam param;
}TData;

/*Datová položka TRP s explicitně řetězenými synonymy*/
 typedef struct TItem{
   char *key;            /* klíč  */
   TData data;          /* obsah */
   int type;
   int druh;
   struct TItem* ptrnext;  /* ukazatel na další synonymum */
} TItem;

/* TRP s explicitně zřetězenými synonymy. */
typedef TItem* tHTable[MAX_HTSIZE];

/* Globalni promenna hash table */
tHTable *ptrhtGlobal;
tHTable *ptrhtLocal;

/* Pro účely testování je vhodné mít možnost volby velikosti pole,
   kterým je vyhledávací tabulka implementována. Fyzicky je deklarováno
   pole o rozměru MAX_HTSIZE, ale při implementaci vašich procedur uvažujte
   velikost HTSIZE.  Ve skriptu se před voláním řešených procedur musí
   objevit příkaz HTSIZE N, kde N je velikost požadovaného prostoru.

   POZOR! Pro správnou funkci TRP musí být hodnota této proměnné prvočíslem.
*/
extern int HTSIZE;

/* Hlavičky řešených procedur a funkcí. */


tChyba htCompleteInsert( tHTable *ptrht, char *key, int druh, int type );

tChyba htParamInsert( tHTable *ptrht, char *key, char *param, int type );

int hashCode( char *key );

tChyba htInit( tHTable **ptrht );

TItem* htSearch( tHTable *ptrht, char *key );

tChyba htInsert( tHTable *ptrht, char *key, TData data, int type, int druh );

tChyba htDeclInsert( tHTable *ptrht, char *key, int type, int druh );

TData* htRead( tHTable *ptrht, char *key );

tChyba htDelete( tHTable *ptrht, char *key );

tChyba htClearAll( tHTable *ptrht );

#endif
