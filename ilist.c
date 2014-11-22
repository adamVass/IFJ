/*
 * Soubor:  ilist.c
 * Autor:   Adam Vass, xvassa00@stud.fit.vutbr.cz
 * Projekt: IFJ; zoznam istrukcii

*******************************************************************/
/**
*@file ilist.c
*
*@brief Instruction list (singly linked list) implementation
*@author Adam Vass (xvassa00)
*/

#include "ilist.h"

#include <stdlib.h>
#include <stdio.h>

void ErrReport()
{ /** General error reporting function */
	printf("Illegal operation within instruction list\n");
	return;
}

void InitList (tList *L)
{ 
/** Initializes list
* @param *L Linked list
*/
	L->First = NULL;
	L->Last = NULL;
	L->Active = NULL;
}

void DisposeList (tList *L)
{
	L->Active = L->First;
	while(L->Active != NULL)
	{
		L->First = L->Active->nextItem; 	/* Sets following item as first */
		free(L->Active); 					/* Frees active item */
		L->Active = L->First; 				/* Sets next item as active */
	}

	L->Last = NULL;
}

void InsertFirst (tList *L, tInst instr)
{
/** Inserts new item to the beginning of list.
* @param *L Linked list
* @param instr Instruction
*/
	tItemPtr newFirst = NULL;
	if ((newFirst = malloc(sizeof(struct tItem))) == NULL) /* Ak alokácia zlyhá volá ErrReport() a končí */
	{
		ErrReport();
		return;
	}

	newFirst->instruction = instr;
	newFirst->nextItem = L->First; 		/* New first item of the list will be pointing to original first item */
	if (L->First == NULL)
		L->Last = newFirst; 			/* If the list is empty, new first element will be last as well */
	L->First = newFirst; 				/* Newly added item will be set as first */
}

void InsertLast(tList *L, tInst instr)
{
/** Inserts new item to the end of list.
* @param *L Linked list
* @param instr Instruction
*/
	tItemPtr newLast = NULL;
	if ((newLast = malloc(sizeof(struct tItem))) == NULL)
	{
		ErrReport();
		return;
	}

	newLast->instruction = instr;
	newLast->nextItem = NULL;

	if (L->First == NULL)
		L->First = newLast; /* If the list is empty, new last element will be first as well */
	else
		L->Last->nextItem = newLast;

	L->Last = newLast;
}

void DeleteFirst (tList *L)
{
/** Deletes first item in the list, frees allocated memory.
* @param *L Linked list
*/
	if (L->First != NULL)
	{
	tItemPtr temp = L->First; 			/* Stores address of first element into variable */

	if (L->Active == L->First)
		L->Active = NULL; 				/* If activity is on the first item, it is nulled */
		L->First = L->First->nextItem; 	/* Following item in the list becomes first */
		free(temp); 					/* Original item is freed, allocated memory returned to the OS */
	}
}

void PostDelete (tList *L)
{
/** Deletes item following active one.
* @param *L Linked list.
*/
	if ((L->Active != NULL) && (L->Active->nextItem != NULL))
	{
		tItemPtr temp = L->Active->nextItem; 		/* Stores address of item following active into variable */
		L->Active->nextItem = temp->nextItem; 		/* Bridges active item and item behind soon-to-be deleted one */
		free(temp);
	}
}

void PostInsert (tList *L, tInst instr)
{
/** Inserts new item following active one
* @param *L Linked list
* @param instr Instruction
*/
	if (L->Active != NULL)
	{
		tItemPtr insert = NULL;
		if ((insert = malloc(sizeof(struct tItem))) == NULL) /* Should allocation error occur, call ErrReport() func and quit */
		{
			ErrReport();
			return;
		}

	insert->instruction = instr;
	insert->nextItem = L->Active->nextItem; 	/* New item of the list will be pointing to the item beyond active one */
	L->Active->nextItem = insert; 				/* Set active item's pointer to newly added item */
	if (L->Active == L->Last)
		L->Last = insert;
	}
}

void Succ (tList *L)
{
/** Moves activity to the next item.
* @param *L Linked list
*/
	if (L->Active == NULL)
		return;

	if (L->Active == L->Last)
		L->Active = NULL;					/* If there's no more items in the list, null the activity */
	else
		L->Active = L->Active->nextItem; 	/* Else, move activity to the next one */

}

void LGoto(tList *L, void *gotoInstr)
{ 
/** Sets active item using direct pointer
* @param *L Linked list.
* @param *gotoInstr Adress of required instruction
*/
  L->Active = (tItemPtr) gotoInstr;
}

void *GetLastPointer(tList *L)
{
/** Returns pointer to the last element of the list.
* @param *L Linked List
* @return Void pointer to the last element of the list
*/ 
  return (void*) L->Last;
}

tInst *GetData(tList *L)
{
/** Returns address of the active instruction
* @param *L Linked List
* @return Address of active instruction
*/
  if (L->Active != NULL)
    return &(L->Active->instruction);

  printf("Zoznam nema aktivnu instrukciu.\n");
  return NULL;
}
