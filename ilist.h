/*
 * Soubor:  ilist.h
 * Autor:   Adam Vass, xvassa00@stud.fit.vutbr.cz
 * Projekt: IFJ; zoznam instrukcii
 */

#ifndef _ILIST_H_
#define _ILIST_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
	int instructionType;
	void *address1;
	void *address2;
	void *address3;
} tInst;

typedef struct tItem
{
  tInst instruction;
  struct tItem *nextItem;
} *tItemPtr;

typedef struct
{
  tItemPtr First;
  tItemPtr Last;
  tItemPtr Active;
} tList;

void ErrReport();
void InitList (tList *L);
void DisposeList (tList *L);
void InsertFirst (tList *L, tInst instr);
void InsertLast(tList *L, tInst instr);
void DeleteFirst (tList *L);
void PostDelete (tList *L);
void PostInsert (tList *L, tInst instr);
void Succ (tList *L);
void LGoto(tList *L, void *gotoInstr);
void *GetLastPointer(tList *L);
tInst *GetData(tList *L);

#endif