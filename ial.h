
/* Hlavičkový soubor pro c016.h - Tabulka s Rozptýlenými Položkami,
**  obsahuje jednak nutné includes a externované proměnné,
**  ale rovnež definici datových typů. Tento soubor neupravujte!
**  Téma:  Tabulka s explicitně zřetězenými synonymy
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2013
**
***/


#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Maximální velikost pole pro implementaci
   vyhledávací tabulky. Řešené procedury však
   využívají pouze HTSIZE prvků pole (viz deklarace této proměnné).
*/
#define MAX_HTSIZE 131

// typ dat

#define TYPESTR      0
#define TYPEBOOL     1
#define TYPEINT      2
#define TYPEDOUBLE   3


#define TYPEUNDEF    10


/* typ obsahu (například cena zboží) */
typedef union TData{
   char *str;  // nebo mozno udelat strukturu, ktera bude obsahovat retezes a jeho delku..
   bool boolValue;
   int intNumber;
   double floatNumber;
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
tHTable* ptrht;

/* Pro účely testování je vhodné mít možnost volby velikosti pole,
   kterým je vyhledávací tabulka implementována. Fyzicky je deklarováno
   pole o rozměru MAX_HTSIZE, ale při implementaci vašich procedur uvažujte
   velikost HTSIZE.  Ve skriptu se před voláním řešených procedur musí
   objevit příkaz HTSIZE N, kde N je velikost požadovaného prostoru.

   POZOR! Pro správnou funkci TRP musí být hodnota této proměnné prvočíslem.
*/
extern int HTSIZE;

/* Hlavičky řešených procedur a funkcí. */

int hashCode ( char *key );

void htInit ();

TItem* htSearch ( char *key );

void htInsert ( char *key, TData data, int type, int druh );

TData* htRead ( char *key );

void htDelete ( char *key );

void htClearAll ();

#endif
