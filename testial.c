
/* ************************ s016-test.c **************************** */

#include "ial.h"

#include <stdio.h>
#include <math.h>
#include <stdlib.h>


//vytiskne tabulku
void htPrintTable() {
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
	
	htInit();

/********************* SCRIPT START ******************************************/


	TData intik = { .intNumber = 5 };
	TData stringik = { .str = "holyfnuk" };
	TData doublik = { .floatNumber = 5.5 };

	htInsert( "intik", intik, TYPEINT);
	htInsert( "stringik", stringik, TYPESTR);
	htInsert( "doublik", doublik, TYPEDOUBLE);

	// vytiskne table
	htPrintTable( ptrht );

	// hledani stringiku
	TItem *find = NULL;
	find = htSearch ( "stringik" );
	if( find ){
		printf("Stringik nalezen: %s\n", find->data.str);
	}
	printf("Mazam stringik\n");
	htDelete( "stringik" );

	// hledani stringiku
	find = NULL;
	find = htSearch ( "stringik" );
	if( !find ){
		printf("Stringik nenalezen:((((\n");
	}

	free ( ptrht );
	
	return 0;	
}
