/*
 * Soubor:  ilist.c
 * Autor:   Adam Vass, xvassa00@stud.fit.vutbr.cz
 * Projekt: IFJ; zoznam istrukcii
 */

#include "ilist.h"


void ErrReport()
{
	printf("Illegal operation within instruction list\n");
	return;
}

void InitList (tList *L)
{
	L->First = NULL;
	L->Last = NULL;
	L->Active = NULL;
}

void DisposeList (tList *L)
{
	L->Active = L->First;
	while(L->Active != NULL)
	{
		L->First = L->Active->nextItem; 	/* Nastaví nasledujúci prvok ako prvý */
		free(L->Active); 					/* Uvoľní aktívny prvok */
		L->Active = L->First; 				/* Nastaví ďalší (po novom prvý) prvok ako aktuálny */
	}

	L->Last = NULL;
}

void InsertFirst (tList *L, tInst instr)
{
	tItemPtr newFirst = NULL;
	if ((newFirst = malloc(sizeof(struct tItem))) == NULL) /* Ak alokácia zlyhá volá ErrReport() a končí */
	{
		ErrReport();
		return;
	}

	newFirst->instruction = instr;
	newFirst->nextItem = L->First; 		/* Nový prvý prvok zoznamu bude ukazovať na pôvodný prvý prvok zoznamu */
	if (L->First == NULL)
		L->Last = newFirst; 			/* Ak je zoznam prázdny nový prvý prvok bude zároveň posledný */
	L->First = newFirst; 				/* Nový prvok zoznamu bude nastavený ako začiatočný */
}

void InsertLast(tList *L, tInst instr)
{
	tItemPtr newLast = NULL;
	if ((newLast = malloc(sizeof(struct tItem))) == NULL)
	{
		ErrReport();
		return;
	}

	newLast->instruction = instr;
	newLast->nextItem = NULL;

	if (L->First == NULL)
		L->First = newLast; /* Ak je zoznam prázdny nový posledný prvok bude zároveň prvý */
	else
		L->Last->nextItem = newLast;

	L->Last = newLast;
}

void DeleteFirst (tList *L)
{
	if (L->First != NULL)
	{
	tItemPtr temp = L->First; 			/* Do pomocnej premennej ukladá adresu prvého prvku zoznamu */

	if (L->Active == L->First)
		L->Active = NULL; 				/* Ak je prvý prvok zároveň aktívny, aktivita sa stráca */
		L->First = L->First->nextItem; 	/* Prvým prvkom sa stáva nasledujúci prvok */
		free(temp); 					/* Pôvodný prvý prvok zaniká, pamäť je vrátená OS */
	}
}

void PostDelete (tList *L)
{
	if ((L->Active != NULL) && (L->Active->nextItem != NULL))
	{
		tItemPtr temp = L->Active->nextItem; 		/* Do pomocnej premennej vloží prvok zoznamu za akt. prvkom */
		L->Active->nextItem = temp->nextItem; 		/* Ako nasledujúci prvok nastaví prvok za tým, kt. bude vymazaný */
		free(temp);
	}
}

void PostInsert (tList *L, tInst instr)
{
	if (L->Active != NULL)
	{
		tItemPtr insert = NULL;
		if ((insert = malloc(sizeof(struct tItem))) == NULL) /* V prípade nedostatku pamäte vracia NULL, volá ErrReport() a funkcia končí */
		{
			ErrReport();
			return;
		}

	insert->instruction = instr; 				/* Do dátovej časti prvku uloží hodnotu val */
	insert->nextItem = L->Active->nextItem; 	/* Nový prvok bude ukazovať na prvok zoznamu za aktuálnym prvkom */
	L->Active->nextItem = insert; 						/* Aktuálny prvok bude ukazovať na novo pridaný prvok */
	if (L->Active == L->Last)
		L->Last = insert;
	}
}

void Succ (tList *L)
{
	if (L->Active == NULL)
		return;

	if (L->Active == L->Last)
		L->Active = NULL;
	else
		L->Active = L->Active->nextItem; 	/* Posunie aktivitu na ďalší prvok, ak ďalší neexistuje zoznam sa stáva neakt. */

}

void LGoto(tList *L, void *gotoInstr)
{ /* Nastavi aktivny prvok podla adresy */
  L->Active = (tItemPtr) gotoInstr;
}

void *GetLastPointer(tList *L)
{ /* Vrati pointer na posledny prvok */
  return (void*) L->Last;
}

tInst *GetData(tList *L)
{ /* Vrati aktivnu instrukciu */
  if (L->Active != NULL)
    return &(L->Active->instruction);

  printf("Zoznam nema aktivnu instrukciu.\n");
  return NULL;
}
