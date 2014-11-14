
/* ************************ s016-test.c **************************** */

#include "ial.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>


TItem* UNDEFPTR;
tHTable* ptrht;

//vytiskne tabulku
void htPrintTable( tHTable* ptrht ) {
	int maxlen = 0;
	int sumcnt = 0;
	
	printf ("------------HASH TABLE--------------\n");
	for ( int i=0; i<HTSIZE; i++ ) {
		printf ("%i:",i);
		int cnt = 0;
		TItem* ptr = (*ptrht)[i];
		while ( ptr != NULL ) {
			printf (" (%s)",ptr->key);  // vytiskne klic
			if( ptr->type == 0 ){			// string
				printf(" %s", ptr->data.str);
			}
			else if( ptr->type == 1 ){ 		// bool
				printf(" %d", ptr->data.boolValue);
			}
			else if( ptr->type == 2 ){ 		// int
				printf(" %d", ptr->data.intNumber);
			}
			else if( ptr->type == 3 ){		// double
				printf(" %f", ptr->data.floatNumber);
			}
			if ( ptr != UNDEFPTR )
				cnt++;
			ptr = ptr->ptrnext;
		}
		printf ("\n");
	
		if (cnt > maxlen)
			maxlen = cnt;
		sumcnt+=cnt;
	}
	
	printf ("------------------------------------\n");
	printf ("Items count %i   The longest list %i\n",sumcnt,maxlen);
	printf ("------------------------------------\n");
}


/* Hlavni funkce pro testovani */
int main(int argc, char* argv[] ) {
	UNDEFPTR = (TItem*) malloc ( sizeof(tHTable) );
	UNDEFPTR->key = "*UNDEF*";
	UNDEFPTR->type = TYPEUNDEF;
	UNDEFPTR->ptrnext = NULL;
	ptrht = (tHTable*) malloc ( sizeof(tHTable) );
	for ( int i=0; i<MAX_HTSIZE; (*ptrht)[i++] = UNDEFPTR );  // undef item asi jako zarazka

/********************* SCRIPT START ******************************************/


	TData intik = { .intNumber = 5 };
	TData stringik = { .str = "holyfnuk" };
	TData doublik = { .floatNumber = 5.5 };

	htInsert( ptrht, "intik", intik, TYPEINT);
	htInsert( ptrht, "stringik", stringik, TYPESTR);
	htInsert( ptrht, "doublik", doublik, TYPEDOUBLE);

	// vytiskne table
	htPrintTable( ptrht );

	// hledani stringiku
	TItem *find = NULL;
	find = htSearch ( ptrht, "stringik" );
	if( find ){
		printf("Stringik nalezen: %s\n", find->data.str);
	}
	printf("Mazam stringik\n");
	htDelete( ptrht, "stringik" );

	// hledani stringiku
	find = NULL;
	find = htSearch ( ptrht, "stringik" );
	if( !find ){
		printf("Stringik nenalezen:((((\n");
	}

	free ( UNDEFPTR );
	free ( ptrht );
	
	return 0;	
}
