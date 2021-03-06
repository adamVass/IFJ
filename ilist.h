/*
 * Soubor:  ilist.h
 * Autor:   Adam Vass, xvassa00@stud.fit.vutbr.cz
 * Projekt: IFJ; zoznam instrukcii
 *******************************************************************/
/**
*@file ilist.h
*
*@brief Header file for ilist.c
*@author Adam Vass (xvassa00)
 */
#ifndef _ILIST_H_
#define _ILIST_H_

#include "ial.h"

typedef struct
{
/** Instruction structure, using three-address code */
	int instructionType;
	void *address1;
	void *address2;
	void *address3;
} tInst;

typedef struct tItem
{
/** Individual list element */
  tInst instruction;
  struct tItem *nextItem;
} *tItemPtr;

typedef struct
{
/** Singly linked list */
  tItemPtr First;
  tItemPtr Last;
  tItemPtr Active;
} tList;

/** Vycet instrukci */
typedef enum {
    OC_ADD,
    OC_SUB,
    OC_MUL,
    OC_DIV,
    OC_MENSI,
    OC_VETSI,
    OC_MENSI_ROVNO,
    OC_VETSI_ROVNO,
    OC_ROVNO,
    OC_NEROVNO,
    OC_PRIRAZENI,
    OC_IF_GOTO,
    OC_WHILE_GOTO,
    OC_NEG,
    OC_WRITE,
    OC_READ,
    OC_LENGTH,
    OC_SORT,
    OC_COPY,
    OC_FIND
} TOpCode;

tList listIntrukci;

void ErrReport();
void InitList (tList *L);
void DisposeList (tList *L);
tChyba InsertFirst (tList *L, tInst instr);
tChyba InsertLast(tList *L, tInst instr);
void DeleteFirst (tList *L);
void PostDelete (tList *L);
tChyba PostInsert (tList *L, tInst instr);
void Succ (tList *L);
void LGoto(tList *L, void *gotoInstr);
void *GetLastPointer(tList *L);
tInst *GetData(tList *L);

#endif
