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

tChyba InsertFirst (tList *L, tInst instr)
{
/** Inserts new item to the beginning of list.
* @param *L Linked list
* @param instr Instruction
* @return Error code (specified in #include navratova_hodnota.h)
*/
	tItemPtr newFirst = NULL;
	if ((newFirst = malloc(sizeof(struct tItem))) == NULL) /* Return S_INTERNI_CHYBA (errno 99) on malloc failure */
		return S_INTERNI_CHYBA;

	newFirst->instruction = instr;
	newFirst->nextItem = L->First; 		/* New first item of the list will be pointing to original first item */
	if (L->First == NULL)
		L->Last = newFirst; 			/* If the list is empty, new first element will be last as well */
	L->First = newFirst; 				/* Newly added item will be set as first */

	return S_BEZ_CHYB;
}

tChyba InsertLast(tList *L, tInst instr)
{
/** Inserts new item to the end of list.
* @param *L Linked list
* @param instr Instruction
* @return Error code
*/
	tItemPtr newLast = NULL;
	if ((newLast = malloc(sizeof(struct tItem))) == NULL)
		return S_INTERNI_CHYBA;

	newLast->instruction = instr;
	newLast->nextItem = NULL;

	if (L->First == NULL)
		L->First = newLast; /* If the list is empty, new last element will be first as well */
	else
		L->Last->nextItem = newLast;

	L->Last = newLast;

	return S_BEZ_CHYB;
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

tChyba PostInsert (tList *L, tInst instr)
{
/** Inserts new item following active one
* @param *L Linked list
* @param instr Instruction
* @return Error code
*/
	if (L->Active != NULL)
	{
		tItemPtr insert = NULL;
		if ((insert = malloc(sizeof(struct tItem))) == NULL) /* Return S_INTERNI_CHYBA (errno 99) on malloc failure */
			return S_INTERNI_CHYBA;

	insert->instruction = instr;
	insert->nextItem = L->Active->nextItem; 	/* New item of the list will be pointing to the item beyond active one */
	L->Active->nextItem = insert; 				/* Set active item's pointer to newly added item */
	if (L->Active == L->Last)
		L->Last = insert;
	}

	return S_BEZ_CHYB;
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

  fprintf(stderr, "Zoznam nema aktivnu instrukciu.\n");
  return NULL;
}
