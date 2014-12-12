/****************************************/
/**  Projekt pro IFJ/IAL                */
/**  Syntakticka analyza                */
/**  Autor: Karel Fajkus                */
/**  email: xfajku05@stud.fit.vutbr.cz  */
/****************************************/


//pouziti knihoven na precedencni analyzu, navratove hodnoty atd.

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "syntakticka_analyza.h"
#include "ilist.h"



//deklarace funkci, ktere budou zmineny driv, nez budou implementovany


void htPrintTable( tHTable *ptrht ) {
	int maxlen = 0;
	int sumcnt = 0;
	printf ("------------HASH TABLE--------------\n");
	for ( int i=0; i<HTSIZE; i++ ) {
	printf ("%i:",i);
	int cnt = 0;
	TItem* ptr = (*ptrht)[i];
	while ( ptr != NULL ) {
	printf (" (%s)",ptr->key); // vytiskne klic
	if( !strcmp(ptr->key, "$1") ){
		printf("%d\n", ptr->data->intNumber);
	}
	if( ptr->druh == 7 ){ 
	printf(" %d ", ptr->druh);
	printf("%d\n", ptr->data->param.numParam );

	for (int i = 0; i < ptr->data->param.numParam; i++){
		printf("%s ", ptr->data->param.param[i]);
		
	}
}
else if( ptr->druh == 8 ){ 
printf(" %d", ptr->druh);
}
else if( ptr->druh == 9 ){ 
printf(" %d", ptr->druh);
}
else if( ptr->druh == 10 ){ 

printf(" %d", ptr->druh);
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
printf ("Items count %i The longest list %i\n",sumcnt,maxlen);
printf ("------------------------------------\n");
}






tChyba PROGRAM () {					//funkce na pravidlo PROGRAM -> FUNKCE . eof
							//nacitani prvniho tokenu je v komentu, protoze na konci je pomocna funkce, kde se uz nacita token, pri vyslednem kodu, bude toto nacteni tokenu odkomentovano
	/*token = getNextToken();
	if(token.stav == s_lex_error) {			//nacteni prvniho tokenu a overeni, zda byl spravne nacteny
		return S_LEXIKALNI_CHYBA;
	}*/
	int analyza;					//lokalni promenna, do ktere se bude nacitat vystup z volane funkce a v pripade chyby se bude obsah promenne vracet
	
	int ret = initFrames();
	byla_funkce = false;
	pocitani_parametru = 0;



	if(ret != S_BEZ_CHYB ) {
		return S_INTERNI_CHYBA;
	}

	if(token.stav == s_klicove && (!strcmp(token.data, "function") || !strcmp(token.data, "var") || !strcmp(token.data, "begin"))) {
							//podminka kde se kontroluje, zda nacteny token je klicove slovo a nasledne se rozhodne, ktere konkretne. Pote dojde k zavolani funkce FUNKCE, kde jsou 3 pravidla, ktere zacinaji terminala vyse
		analyza = FUNKCE ();
		if (analyza != S_BEZ_CHYB) {		//pokud se vrati po zavolani FUNKCE() s_bez_chyb pokracuje se dal, jinak se vraci chybovy kod, ktery funkce vratila

		return analyza;
		}

		if(token.stav == s_tecka) {		//porovnava se ciselna hodnota tokenu s ciselnou hodnotu tecky (tecka neni primo nadefinovana jako tecka, ale je pro ni vyhrazen unikatni identifikator ciselny, ktery porovnavam s id prvku v nactenem tokenu)
			token = getNextToken();
			if(token.stav == s_lex_error) {	//jakmile nacitam token okamzite kontroluju, jestli se nevyskytla lexikalni chyba
				return S_LEXIKALNI_CHYBA;
			}

			if(token.stav == s_eof) {	//jakmile dojdeme ke konci souboru, vrati funkce syntakticka analyza ok a mame vyhrano
				return S_BEZ_CHYB;
			}
		}
	}
		return S_SYNTAKTICKA_CHYBA;		//pokud nejaky token neodpovida predem danym terminalum z pravidel, vraci chybu
}

tChyba FUNKCE() {
	int analyza;
						//opet lokalni promenna, stejna funkce jako ve funkci nahore
	TItem *nasel;
	if(!strcmp(token.data, "begin") && token.stav == s_klicove) {
		token = getNextToken();			//pres strcmp porovnavam data tokenu (prakticky primo to, co se nacte)
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		analyza = ACT_LIST();			//po zavolani funkce se getToken nevola, protoze kazda funkce, nez vrati hodnotu, nacte dalsi token
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		return S_BEZ_CHYB;
	}
	else if(!strcmp(token.data, "function") && token.stav == s_klicove) {
		byla_funkce = true;
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_identifikator) {	//pokud nacitame a porovnavame terminaly, tak po kazdem porovnani nacitame dalsi token, aby se neporovnavalo to same
			if(!strcmp(token.data, "length") || !strcmp(token.data, "copy")) {
				return S_SEMANTICKA_CHYBA_NEDEF;
			}

			if((funkce = malloc(strlen(token.data)+1)) == NULL) {	
				return S_INTERNI_CHYBA;
			}
			TData *dat = (TData*)malloc(sizeof(TData));
			dat->param.numParam = 0;
			
			strcpy(funkce, token.data);
			nasel = htSearch(ptrhtGlobal,funkce);		//pokud id funkce jiz je v globalni hash tabulce a nejedna se o doprednou deklaraci -> chyba
			if(nasel != NULL ) {
				if(nasel->init == false) {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}
			}
			//VYRESIT ZDA JE DOBRE ZE JE U DOPREDNE V GLOBALCE 2x STEJNE ID FUNKCE
			htInsert(ptrhtGlobal,funkce,dat, TYPEUNDEF , ID_FUNCTION);		//vlozeni nazvu funkce do globalni hash

			token = getNextToken();
			if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
			}

			analyza = PARAMS();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
				
			else if(token.stav == s_dvojtecka) {
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}
					
				analyza = DTYPE();
				if(analyza != S_BEZ_CHYB) {
					return analyza;
				}
										
				if(!strcmp(token.data, "integer")) {
					dtype = TYPEINT;
				}
				else if(!strcmp(token.data, "real")) {				//do docasne promene nacteme datovy typ funkce
					dtype = TYPEDOUBLE;
				}
				else if(!strcmp(token.data, "string")) {
					dtype = TYPESTR;
				}
				else if(!strcmp(token.data, "boolean")) {
					dtype = TYPEBOOL;
				}
					
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}
	
				else if(token.stav == s_strednik) {
					token = getNextToken();
					if(token.stav == s_lex_error) {
						return S_LEXIKALNI_CHYBA;
					}

					analyza = DOPREDNE();
					if(analyza != S_BEZ_CHYB) {
						return analyza;
					}

					if(init == true) {
						htCompleteInsert(ptrhtGlobal,funkce, ID_FUNCTION, dtype, true);				
						free(funkce);
							
					}																				//dokonceni vkladani funkce do tabulky, konkretne datovy typ a zda se jedna o doprednou deklaraci ci nikoliv
					else if (init == false) {
						htCompleteInsert(ptrhtGlobal,funkce, ID_FUNCTION, dtype, false);	
						htInsert(func[currFuncSize-1].table, funkce, dat, dtype, ID_FUNCTION);

							//vlozeni navratove hodnoty do lokalni tabulky
					}
						
					if(token.stav == s_strednik) {
							
						token = getNextToken();
						if(token.stav == s_lex_error) {
							return S_LEXIKALNI_CHYBA;
						}
						else {
							ptrhtLocal = ptrStack->top->ptrht;
							listIntrukci = &(ptrStack->top->field);
							// nastaveni tabulky instrukci v mainu
							byla_funkce = false;
							return FUNKCE();
						}								//volani interpretu
					}

								//jakmile je vse splneno, vola funkce sama sebe znovu, protoze podle pravidel, jakmile se dojede na konec, tak je potreba zavolat FUNKCE() znovu, protoze napriklad funkci muze byt v kodu deklarovano mnoho a ne jen jedna
						
				}
			}
		}
		return S_SYNTAKTICKA_CHYBA;		//pokud se nacte token, ktery podle pravidla tam nepatri, vrati se syntakticka chyba
	}
	else if(!strcmp(token.data, "var") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		
		analyza = DEKLARACE();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		return FUNKCE();
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba PARAMS() {
	int analyza;
	if(token.stav == s_leva_zavorka) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		else if(token.stav == s_identifikator) {
			if((params = malloc(strlen(token.data)+1)) == NULL) {
				return S_INTERNI_CHYBA;
			}
		
			strcpy(params, token.data);			//do globalni promenne params nacteme id parametru funkce
		
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			if(token.stav == s_dvojtecka) {
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}

				analyza = DTYPE();
				if(analyza != S_BEZ_CHYB) {
					return analyza;
				}

				if(!strcmp(token.data, "integer")) {
					dtype = TYPEINT;
				}
				else if(!strcmp(token.data, "real")) {
					dtype = TYPEDOUBLE;
				}
				else if(!strcmp(token.data, "string")) {
					dtype = TYPESTR;
				}
				else if(!strcmp(token.data, "boolean")) {
					dtype = TYPEBOOL;
				}
				
				htParamInsert(ptrhtGlobal,funkce, params, dtype);
				htInsert(func[currFuncSize].table, params, NULL, dtype, ID_PARAM);			//vkladame parametr funkce do globalni hash k dane funkci
				free(params);
				
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}
	
		        return PARAMS();
			}
			return S_SYNTAKTICKA_CHYBA;
		}
	}	
	else if(token.stav == s_strednik) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		
		if(token.stav == s_identifikator) {
			
			if((params = malloc(strlen(token.data)+1)) == NULL) {
				return S_INTERNI_CHYBA;
			}
			
			strcpy(params, token.data);
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_dvojtecka) {
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}
				
				analyza = DTYPE();
				if(analyza != S_BEZ_CHYB) {
					return analyza;
				}
				
				if(!strcmp(token.data, "integer")) {
				dtype = TYPEINT;
				}
				else if(!strcmp(token.data, "string")) {
					dtype = TYPESTR;
				}
				else if(!strcmp(token.data, "real")) {
					dtype = TYPEDOUBLE;
				}
				else if(!strcmp(token.data, "boolean")){
					dtype = TYPEBOOL;
				}
					
				htParamInsert(ptrhtGlobal,funkce, params, dtype);
				free(params);
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}	
				return PARAMS();
			}
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	else if(token.stav == s_prava_zavorka) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		return S_BEZ_CHYB;
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba PROM() {
	int analyza;
	int ret;
	//TItem *pt;
	if(token.stav == s_identifikator) {
		if(byla_funkce == false) {
			ret = pushFrame();
			if(ret != S_BEZ_CHYB) {
				return ret;
			}
		}
		
		TFunction *tmp = searchFuncTable( token.data );
		if( !tmp ){
			return S_SEMANTICKA_CHYBA_NEDEF;
		}

		if(byla_funkce == false) {
			ret = copyTable( tmp);//, ptrhtLocal );
			if(ret != S_BEZ_CHYB) {
				return ret;
			}
		}

		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

			analyza = TERM2();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			else {
				if(byla_funkce == false) {
					popFrame();
				}	
				
				return S_BEZ_CHYB;
			}
			return S_SYNTAKTICKA_CHYBA;
	}
	return S_SYNTAKTICKA_CHYBA;		
}

tChyba VESTAV() {
	TItem *pt, *pt2, *pt3;

	if(!strcmp(token.data, "length")) {
		pt = searchFrames(id, ptrhtGlobal, ptrhtLocal);
		if(pt != NULL) {
			
		}
		else {
			return S_SEMANTICKA_CHYBA_NEDEF;
		}

		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}
			else if(token.stav == s_string || token.stav == s_identifikator) {
				if(token.stav == s_identifikator) {
					pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);				//prohledani lokalniho i globalniho ramce a pokud id neni nalezeno nebo ma jiny datovy typ nez string -> chyba
					if(pt != NULL) {
						
						if(pt->type != s_string) {
							return S_SEMANTICKA_CHYBA_TYPOVA;
						}
					}
					else {
						return S_SEMANTICKA_CHYBA_NEDEF;
					}
					generateInstruction(OC_LENGTH, searchFrames(token.data,ptrhtLocal, ptrhtGlobal), NULL, searchFrames(id,ptrhtLocal,ptrhtGlobal));	
				}
				else {
					TItem *str = (TItem*)malloc(sizeof(TItem));
					str->druh = PRINTSTR;
					str->type = TYPESTR;
					str->init = true;
					str->data = (TData*)malloc(sizeof(TData));
					str->data->str = allocString(token.data);
					generateInstruction(OC_LENGTH, str, NULL, searchFrames(id,ptrhtLocal,ptrhtGlobal));
				}
				
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}

				else if(token.stav == s_prava_zavorka) {
					token = getNextToken();
					if(token.stav == s_lex_error) {
						return S_LEXIKALNI_CHYBA;
					}
					return S_BEZ_CHYB;
				}
			}
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	else if(!strcmp(token.data, "copy")) {
		pt = searchFrames(id, ptrhtGlobal, ptrhtLocal);
		if(pt != NULL) {
			
		}
		else {
			return S_SEMANTICKA_CHYBA_NEDEF;
		}


		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_string || token.stav == s_identifikator) {
				if(token.stav == s_identifikator) {	
					pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);			//prohledani lokalniho i globalniho ramce a pokud id neni nalezeno nebo ma jiny datovy typ nez string -> chyba
					if(pt != NULL) {
						
						if(pt->type != s_string) {
							return S_SEMANTICKA_CHYBA_TYPOVA;
						}
											
					}
					else {
						return S_SEMANTICKA_CHYBA_NEDEF;
					}
					pt2 = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
				}
				else {
					pt2 = (TItem*)malloc(sizeof(TItem));
					pt2->druh = PRINTSTR;
					pt2->type = TYPESTR;
					pt2->init = true;
					pt2->data = (TData*)malloc(sizeof(TData));
					pt2->data->str = allocString(token.data);
				}

				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}

				else if(token.stav == s_carka) {
					token = getNextToken();
					if(token.stav == s_lex_error) {
						return S_LEXIKALNI_CHYBA;
					}

					else if(token.stav == s_cele_cislo || token.stav == s_identifikator) {
						if(token.stav == s_identifikator) {
							pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);			//prohledani lokalniho i globalniho ramce a pokud id neni nalezeno nebo ma jiny datovy typ nez string -> chyba
							if(pt != NULL) {
								
								if(pt->type != s_cele_cislo) {
									return S_SEMANTICKA_CHYBA_TYPOVA;
								}
							}
							else {
								return S_SEMANTICKA_CHYBA_NEDEF;
							}
							pt3 = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
						}
						else {
							pt3 = (TItem*)malloc(sizeof(TItem));
							pt3->type = TYPEINT;
							pt3->init = true;
							pt3->data = (TData*)malloc(sizeof(TData));
							pt3->data->intNumber = atoi(token.data);
						}

						token = getNextToken();
						if(token.stav == s_lex_error) {
							return S_LEXIKALNI_CHYBA;
						}

						else if(token.stav == s_carka) {
							token = getNextToken();
							if(token.stav == s_lex_error) {
								return S_LEXIKALNI_CHYBA;
							}

							else if(token.stav == s_cele_cislo || token.stav == s_identifikator) {
								if(token.stav == s_identifikator) {
									pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
									if(pt != NULL) {
										
										if(pt->type != s_cele_cislo) {
											return S_SEMANTICKA_CHYBA_TYPOVA;
										}
									}
									else {
										return S_SEMANTICKA_CHYBA_NEDEF;
									}
									generateInstruction(OC_COPY, pt2, pt3, searchFrames(token.data, ptrhtLocal, ptrhtGlobal));
									generateInstruction(OC_COPY, NULL, NULL, searchFrames(id, ptrhtLocal, ptrhtGlobal));	
								}
								else {
									TItem *cele = (TItem*)malloc(sizeof(TItem));
									cele->type = TYPEINT;
									cele->init = true;
									cele->data = (TData*)malloc(sizeof(TData));
									cele->data->intNumber = atoi(token.data);
									generateInstruction(OC_COPY, pt2, pt3, cele);
									generateInstruction(OC_COPY, NULL, NULL, searchFrames(id, ptrhtLocal, ptrhtGlobal));
								}

								token = getNextToken();
								if(token.stav == s_lex_error) {
									return S_LEXIKALNI_CHYBA;
								}

								else if(token.stav == s_prava_zavorka) {
									token = getNextToken();
									if(token.stav == s_lex_error) {
										return S_LEXIKALNI_CHYBA;
									}
									return S_BEZ_CHYB;
								}
							}
						}
					}
				}
			}
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	else if(!strcmp(token.data, "find") && token.stav == s_klicove) {
		pt = searchFrames(id, ptrhtGlobal, ptrhtLocal);
		if(pt != NULL) {
			
		}
		else {
			return S_SEMANTICKA_CHYBA_NEDEF;
		}

		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_string || s_identifikator) {
				if(token.stav == s_identifikator) {
					pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
					if(pt != NULL) {
						
						if(pt->type != s_string) {
							return S_SEMANTICKA_CHYBA_TYPOVA;
						}
					}
					else {
						return S_SEMANTICKA_CHYBA_NEDEF;
					}
					pt2 = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
				}
				else {
					pt2 = (TItem*)malloc(sizeof(TItem));
					pt2->druh = PRINTSTR;
					pt2->type = TYPESTR;
					pt2->init = true;
					pt2->data = (TData*)malloc(sizeof(TData));
					pt2->data->str = allocString(token.data);
				}

				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}

				else if(token.stav == s_carka) {
					token = getNextToken();
					if(token.stav == s_lex_error) {
						return S_LEXIKALNI_CHYBA;
					}

					else if(token.stav == s_string || token.stav == s_identifikator) {
						if(token.stav == s_identifikator) {
							pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
							if(pt != NULL) {
								
								if(pt->type != s_string) {
									return S_SEMANTICKA_CHYBA_TYPOVA;
								}
							}
							else {
								return S_SEMANTICKA_CHYBA_NEDEF;
							}
							generateInstruction(OC_FIND, pt2, searchFrames(token.data, ptrhtLocal, ptrhtGlobal), searchFrames(id, ptrhtLocal, ptrhtGlobal));	
						}
						else {
							TItem *str = (TItem*)malloc(sizeof(TItem));
							str->druh = PRINTSTR;
							str->type = TYPESTR;
							str->init = true;
							str->data = (TData*)malloc(sizeof(TData));
							str->data->str = allocString(token.data);
							generateInstruction(OC_FIND, pt2, str, searchFrames(id,ptrhtLocal,ptrhtGlobal));
						}

						token = getNextToken();
						if(token.stav == s_lex_error) {
							return S_LEXIKALNI_CHYBA;
						}

						else if(token.stav == s_prava_zavorka) {
							token = getNextToken();
							if(token.stav == s_lex_error) {
								return S_LEXIKALNI_CHYBA;
							}
							return S_BEZ_CHYB;
						}
					}
				}
			}
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	else if(!strcmp(token.data, "sort") && token.stav == s_klicove) {
		pt = searchFrames(id, ptrhtGlobal, ptrhtLocal);
		if(pt != NULL) {
			
		}
		else {
			return S_SEMANTICKA_CHYBA_NEDEF;
		}

		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_string || token.stav == s_identifikator) {
				if(token.stav == s_identifikator) {
					pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);	
					if(pt != NULL) {
						
							if(pt->type != s_string) {
								return S_SEMANTICKA_CHYBA_TYPOVA;
							}
					}
					else {
						return S_SEMANTICKA_CHYBA_NEDEF;
					}
					generateInstruction(OC_SORT, searchFrames(token.data, ptrhtLocal, ptrhtGlobal), NULL, searchFrames(id, ptrhtLocal, ptrhtGlobal));	
				}
				else {
					TItem *str = (TItem*)malloc(sizeof(TItem));
					str->druh = PRINTSTR;
					str->type = TYPESTR;
					str->init = true;
					str->data = (TData*)malloc(sizeof(TData));
					str->data->str = allocString(token.data);
					generateInstruction(OC_SORT, str, NULL, searchFrames(id,ptrhtLocal,ptrhtGlobal));
				}

				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}

				else if(token.stav == s_prava_zavorka) {
					token = getNextToken();
					if(token.stav == s_lex_error) {
						return S_LEXIKALNI_CHYBA;
					}
					return S_BEZ_CHYB;
				}
			}
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	return S_EPS;
}

tChyba DOPREDNE() {
	int ret;
	TItem *nasel;
	
	if(!strcmp(token.data, "forward") && token.stav == s_klicove) {			
		init = true;						//nastaveni na true pokud se jedna o doprednou deklaraci funkce
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return S_BEZ_CHYB;
	}
	else {

		// check velikost tabulky
		if( currFuncSize == funcSize ){
			ret = resizeFuncField();
			if(ret != S_BEZ_CHYB) {
				return ret;
			}
		}	

		
		//nastaveni klice funkce
		func[currFuncSize].key = allocString(funkce);

		//inicializace tabulky
		ret = htInit(&func[currFuncSize].table );
		if(ret != S_BEZ_CHYB) {
			return ret;
		}
		InitList( &(func[currFuncSize].instrList) );
		listIntrukci = &(func[currFuncSize].instrList);
		ptrhtLocal = func[currFuncSize].table;
		currFuncSize++;
		//kopirovani tabulky
		//ret = copyTable( ptrhtLocal, tmp );

		//vlozeni navratove hodnoty do lokalni tabulky


		nasel = htSearch(ptrhtGlobal, funkce);		//nakopirovani parametru do lokalniho ramce
		if(nasel != NULL) {

			for(int i=0; i < nasel->data->param.numParam; i++) {
				htDeclInsert(ptrhtLocal, nasel->data->param.param[i], nasel->data->param.typeParam[i], ID_PARAM);	
			}
		}	
		return ACT_LIST();
	}
}



tChyba DTYPE2() {
	if(token.stav == s_cele_cislo) {
				
		return S_BEZ_CHYB;
	}
	else if(token.stav == s_desetinne_cislo) {
		
		return S_BEZ_CHYB;
	}
	else if(token.stav == s_string) {
		
		return S_BEZ_CHYB;
	}
	else if(token.stav == s_logicka_hodnota) {
		
		return S_BEZ_CHYB;
	}
	return S_SYNTAKTICKA_CHYBA;




}

tChyba DTYPE() {
	if(!strcmp(token.data, "integer") && token.stav == s_klicove) {
				
		return S_BEZ_CHYB;
	}
	else if(!strcmp(token.data, "real") && token.stav == s_klicove) {
		
		return S_BEZ_CHYB;
	}
	else if(!strcmp(token.data, "string") ) {
		
		return S_BEZ_CHYB;
	}
	else if(!strcmp(token.data, "boolean") && token.stav == s_klicove) {
		
		return S_BEZ_CHYB;
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba ROZHODNI() {
	
	int analyza2;
	int analyza;
	int analyza1;
	TItem *pt;
	if((analyza = DTYPE()) == S_BEZ_CHYB) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}	
		return S_BEZ_CHYB;
	}
	else {
			
		analyza2 = VESTAV();
		if(analyza2 != S_BEZ_CHYB && analyza2 != S_EPS) {
			return analyza2;
		}

		if(analyza2 == S_BEZ_CHYB) {
			return S_BEZ_CHYB;
		}
			
		
		
			if((porovnani = malloc(strlen(token.data)+1)) == NULL) {
				return S_INTERNI_CHYBA;
			}
			strcpy(porovnani, token.data);
			pt = htSearch(ptrhtGlobal, token.data);		
			
			if(pt != NULL && pt->druh == ID_FUNCTION) {			//prirazeni, pokud dane id bylo deklarovane a zaroven to je funkce vola se dalsi pravidlo, jinak se vola precedencni
				analyza1 = PROM();
				if(analyza1 != S_BEZ_CHYB) {
					return analyza1;
				}
				
				free(porovnani);
				return S_BEZ_CHYB;	
			}
			else {	
				analyza2 = precedencniSA();
				if(analyza2 != S_BEZ_CHYB) {
					return analyza2;
				}

				//je nutno zmenit htinsert a pridat parametr init na konci a defaultne ho mit na false
				//instrukce pro prirazeni
				generateInstruction( OC_PRIRAZENI, searchFrames(neterminal.polozkaTS.key, ptrhtLocal, ptrhtGlobal), NULL, searchFrames(id, ptrhtLocal, ptrhtGlobal) );
				
				return S_BEZ_CHYB;
				
			}
		}
		

}

tChyba LEXEM() {
	if(token.stav == s_prirazeni) {
		prirovnani = true;				//zapamatovani, ze slo o prirovnani
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}	
		return S_BEZ_CHYB;
	}
	else if(token.stav == s_dvojtecka) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return S_BEZ_CHYB;
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba UKONCOVACI() {
	if(!strcmp(token.data, "end") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {								//osetreni stredniku pokud je dalsi token end
			return S_LEXIKALNI_CHYBA;
		}
		return S_BEZ_CHYB;
	}
	else if(token.stav == s_strednik) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		
		if(!strcmp(token.data, "end") && token.stav == s_klicove) {
			return S_SYNTAKTICKA_CHYBA;
		}

		return ACT_LIST();
	}
	return S_SYNTAKTICKA_CHYBA;
}


tChyba ACT_LIST() {
	int analyza;
	int dtype2=0;
	TItem *pt;

	if(!strcmp(token.data, "end") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return S_BEZ_CHYB;
	}
	else if(!strcmp(token.data, "begin") && token.stav == s_klicove) {
		
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return ACT_LIST();
	}
	else if(token.stav == s_identifikator) {

		if((id = malloc(strlen(token.data)+1)) == NULL)	{
				return S_INTERNI_CHYBA;
			}
				
		strcpy(id, token.data);	
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		analyza = LEXEM();

		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		
		
		if(!strcmp(token.data, "integer")) {
			dtype2 = TYPEINT;
		}
		else if(!strcmp(token.data, "string")) {			//pokud se jedna o deklaraci id, dojde k ulozeni jeho datoveho typu
			dtype2 = TYPESTR;
		}
		else if(!strcmp(token.data, "real")) {
			dtype2 = TYPEDOUBLE;
		}
		else if(!strcmp(token.data, "boolean")) {
			dtype2 = TYPEBOOL;
		}
		//pokud je prirazeni zkontroluj ze je v nejake tabulce a inicializuj ji
		if(prirovnani == true) {
			pt = searchFrames(id, ptrhtGlobal, ptrhtLocal);
			if(pt != NULL) {
				prirovnani = false;
			}
			else {
				return S_SEMANTICKA_CHYBA_NEDEF;
			}
		}

		analyza = ROZHODNI();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		
		if(dtype2 != 0) {									//pokud byla deklarace, ulozime id do lokalniho ramce
			htDeclInsert(ptrhtLocal,id, dtype2, ID_LOCAL);			
			free(id);
		}
		
		analyza = UKONCOVACI();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		return S_BEZ_CHYB;
	}
	else if(!strcmp(token.data, "var") && token.stav == s_klicove) {
		

		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		if(token.stav == s_identifikator) {
			if((id = malloc(strlen(token.data)+1)) == NULL)	{
				return S_INTERNI_CHYBA;
			}
			
			if(!strcmp(token.data, "length") || !strcmp(token.data, "copy")) {			//nesmi se id jmenovat stejne jak tyto funkce
				return S_SEMANTICKA_CHYBA_NEDEF;
			}

			pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);				//pokud jiz nazev existuje -> chyba
			if(pt != NULL) {
				return S_SEMANTICKA_CHYBA_NEDEF;
			}

			strcpy(id, token.data);		
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_dvojtecka) {
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}

				analyza = DTYPE();
				if(analyza != S_BEZ_CHYB) {
					return analyza;
				}
				
				if(!strcmp(token.data, "integer")) {
					dtype = TYPEINT;
				}
				else if(!strcmp(token.data, "string")) {
					dtype = TYPESTR;
				}
				else if(!strcmp(token.data, "real")) {
					dtype = TYPEDOUBLE;
				}
				else if(!strcmp(token.data, "boolean")) {
					dtype = TYPEBOOL;
				}
			
				
				htDeclInsert(ptrhtLocal,id, dtype, ID_LOCAL);		//vlozeni id do lokalniho ramce
				free(id);
				
				
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}	

				else if(token.stav == s_strednik) {
					token = getNextToken();
					if(token.stav == s_lex_error) {
						return S_LEXIKALNI_CHYBA;
					}
					return ACT_LIST();
				}
			}
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	else if(!strcmp(token.data, "if") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		
		
		
		analyza = precedencniSA();			//po if nasleduje vyraz, zavolame precedencni na vyhodnoceni
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		
		if(!strcmp(token.data, "then") && token.stav == s_klicove) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}
		
			analyza = CYKLUS();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}

			else if(!strcmp(token.data, "else") && token.stav == s_klicove) {
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}
				
				analyza = CYKLUS();
				if(analyza != S_BEZ_CHYB) {
					return analyza;
				}
				return UKONCOVACI();
			}
			
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	else if(!strcmp(token.data, "while") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		analyza = precedencniSA();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}

		if(!strcmp(token.data, "do") && token.stav == s_klicove) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			analyza = CYKLUS();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			return UKONCOVACI();
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	else if(!strcmp(token.data, "readln") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_identifikator) {
				pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);				//pokud zadany parametr nebyl deklarovan -> chyba
				if(pt == NULL) {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}

				if(pt->type == TYPEBOOL) {
					return S_SEMANTICKA_CHYBA_TYPOVA;
				}
				//generovani instrukce read, v token.data je id
				generateInstruction(OC_READ, NULL, NULL, searchFrames(token.data,ptrhtLocal,ptrhtGlobal));		
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}

				else if(token.stav == s_prava_zavorka) {
					token = getNextToken();
					if(token.stav == s_lex_error) {
						return S_LEXIKALNI_CHYBA;
					}

					analyza = UKONCOVACI();
					if(analyza != S_BEZ_CHYB) {
						return analyza;
					}
					return S_BEZ_CHYB;
					
				}
			}
			else {
				return S_SEMANTICKA_CHYBA_TYPOVA;
			}
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	else if(!strcmp(token.data, "write") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		
		
			analyza = TERM();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			
			analyza = UKONCOVACI();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			
			return S_BEZ_CHYB;
		
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba CYKLUS() {
	if(!strcmp(token.data, "begin") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return ACT_LIST();
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba DEKLARACE() {
	int analyza;
	TItem *pt;
	if(token.stav == s_identifikator) {
		if((id = malloc(strlen(token.data)+1)) == NULL) {
			return S_INTERNI_CHYBA;
		}
		
		if(!strcmp(token.data, "length") || !strcmp(token.data, "copy")) {		//id se nesmi jmenovat jako tyto funkce
				return S_SEMANTICKA_CHYBA_NEDEF;
		}

		pt = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);
		if(pt != NULL) {								//id musi byt deklarovano jinak -> chyba
			return S_SEMANTICKA_CHYBA_NEDEF;
		}

		strcpy(id, token.data);		
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_dvojtecka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			analyza = DTYPE();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			
			if(!strcmp(token.data, "integer")) {
				dtype = TYPEINT;
			}
			else if(!strcmp(token.data, "string")) {
				dtype = TYPESTR;
			}
			else if(!strcmp(token.data, "real")) {
				dtype = TYPEDOUBLE;
			}
			else if(!strcmp(token.data, "boolean")) {
				dtype = TYPEBOOL;
			}
			
					
			htDeclInsert(ptrhtGlobal,id, dtype, ID_GLOBAL);		//vlozime do globalniho ramce globalni promennou
			free(id);
		
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}	

			else if(token.stav == s_strednik) {
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}
				return DEKLARACE();
			}
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	else if((!strcmp(token.data, "function")  || !strcmp(token.data, "begin")) && token.stav == s_klicove) {
		return S_BEZ_CHYB;		//pokud dalsi token je jeden z vyse uvedenych jedna se o epsilon pravidlo a vracime OK
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba TERM() {
	int analyza;
	TItem *pt, *pt2;
	

	if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}
		if(token.stav == s_cele_cislo || token.stav == s_desetinne_cislo || token.stav == s_logicka_hodnota || token.stav == s_string || token.stav == s_identifikator) {
			analyza = DRUH();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			
			if(token.stav == s_identifikator) {
				if(byla_funkce == true) {
					pt = searchFrames(funkce, ptrhtLocal, ptrhtGlobal);
					if(pt != NULL) {
						for (int i = 0; i < pt->data->param.numParam; ++i){	
							if(!strcmp(token.data, pt->data->param.param[i])){
								
								break;
							}
						}
					}
				}		

				pt2 = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);
				if(pt2 == NULL) {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}

				generateInstruction( OC_WRITE, pt2 , NULL, NULL );
						
								
			}
			else{
				TItem *str = (TItem*)malloc(sizeof(TItem));
				str->druh = PRINTSTR;
				str->type = TYPESTR;
				str->init = true;
				str->data = (TData*)malloc(sizeof(TData));
				str->data->str = allocString(token.data);
				generateInstruction( OC_WRITE, str , NULL, NULL );
			}

			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}
			return TERM();
		}	
	}
	else if(token.stav == s_carka) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_cele_cislo || token.stav == s_desetinne_cislo || token.stav == s_logicka_hodnota || token.stav == s_string || token.stav == s_identifikator) {
			analyza = DRUH();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}

			if(token.stav == s_identifikator) {
				//pokud jsme v definici funkce hledej i na parametry, jinak je ve "funkce" nic.
				if(byla_funkce == true) {
					pt = searchFrames(funkce, ptrhtLocal, ptrhtGlobal);
					if(pt != NULL) {
						for (int i = 0; i < pt->data->param.numParam; ++i){	
							if(!strcmp(token.data, pt->data->param.param[i])){
								
								break;
							}
						}
					}
				}		

					pt2 = searchFrames(token.data, ptrhtLocal, ptrhtGlobal);
					if(pt2 == NULL) {
						return S_SEMANTICKA_CHYBA_NEDEF;
					}
					generateInstruction( OC_WRITE, pt2 , NULL, NULL );
			}
			else{
				TItem *str = (TItem*)malloc(sizeof(TItem));
				str->druh = PRINTSTR;
				str->init = true;
				str->type = TYPESTR;
				str->data = (TData*)malloc(sizeof(TData));
				str->data->str = allocString(token.data);
				generateInstruction( OC_WRITE, str , NULL, NULL );
			}

			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}
			return TERM();
		}
	}
	else if(token.stav == s_prava_zavorka) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return S_BEZ_CHYB;
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba TERM2() {
	int analyza;
	TItem *pt2;
	TItem *pt;
	bool nasel = false;
	if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}
		if(token.stav == s_cele_cislo || token.stav == s_desetinne_cislo || token.stav == s_logicka_hodnota || token.stav == s_string || token.stav == s_identifikator) {
			analyza = DRUH();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			if(byla_funkce == false) {
				int ret = addArgument(token.data);
				if( ret != S_BEZ_CHYB ){
					return ret;
				}
			}
				
					//pokud se jedna id, ktere davame jako parametr, vyhledame odpovidajici funkci a overime, zda datovy typ vkladaneho id odpovida typu parametru pri deklaraci funkce
			/* VYGENEROVAT INSTRUKCE A PREDAT GLOBALNI UKAZATEL NA TABULKU A JE POTREBA UDELAT BOOLEAN JESTLI TO JE V MAINU */			
			
			pt = htSearch(ptrhtGlobal, porovnani);
			if(token.stav == s_identifikator) {
					//pokud to bylo id
				if(pt != NULL) {
					for (int i = 0; i < pt->data->param.numParam; ++i){			//prochazime pole parametru
						if(!strcmp(token.data, pt->data->param.param[i])) {		//pokud najdeme parametr tak nastavime na true
							nasel = true;
							break;
						}				//slouzi k zapamatovani, zda bylo id 
					}

					if(nasel != true) { //POKUD NENI PARAMETR PROJDEME
						pt2 = searchFrames(token.data, ptrhtLastLocal, ptrhtGlobal);
						if(pt2 != NULL) {		
							if(pt->data->param.typeParam[pocitadlo] != pt2->type) {
								return S_SEMANTICKA_CHYBA_TYPOVA;
							}		
						}	
						else {
							return S_SEMANTICKA_CHYBA_NEDEF;
						}
					}		
				}	
				else {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}			

				nasel = false;
			}
			else {
				if(pt != NULL) {
					if(token.stav != pt->data->param.typeParam[pocitadlo]) {
						return S_SEMANTICKA_CHYBA_TYPOVA;
					}
				}
				else {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}	
			}

			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}
			pocitadlo++;
			return TERM2();
		}	
	}
	else if(token.stav == s_carka) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(byla_funkce == false) {
			int ret = addArgument(token.data);
			if( ret != S_BEZ_CHYB ){
				return ret;
			}
		}
		if(token.stav == s_cele_cislo || token.stav == s_desetinne_cislo || token.stav == s_logicka_hodnota || token.stav == s_string || token.stav == s_identifikator) {
			analyza = DRUH();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			pt = htSearch(ptrhtGlobal, porovnani);
			if(token.stav == s_identifikator) {
				if(pt != NULL) {	
					for (int i = 0; i < pt->data->param.numParam; ++i){
						if(!strcmp(token.data, pt->data->param.param[i])) {
							nasel = true;
							break;
						}				//slouzi k zapamatovani, zda bylo id 
					}

					if(nasel != true) {
						pt2 = searchFrames(token.data, ptrhtLastLocal, ptrhtGlobal);
						if(pt2 != NULL) {			
							if(pt->data->param.typeParam[pocitadlo] != pt2->type) {
								return S_SEMANTICKA_CHYBA_TYPOVA;
							}	
						}
						else {
							return S_SEMANTICKA_CHYBA_NEDEF;
						}
					}		
				}	
				else {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}			
				
				nasel = false;
			}
			else {
				if(token.stav != pt->data->param.typeParam[pocitadlo]) {
					return S_SEMANTICKA_CHYBA_TYPOVA;
				}
			}	

		}

		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		pocitadlo++;
		return TERM2();
		
	}
	else if(token.stav == s_prava_zavorka) {
		pt = htSearch(ptrhtGlobal, porovnani);			//zjistime, zda pocet zadanych parametru odpovida poctu parametru pri deklaraci funkce
		if(pocitadlo != pt->data->param.numParam) {
			return S_SEMANTICKA_CHYBA_TYPOVA;
		}
		
		if(byla_funkce == true) {
			generateInstruction(OC_COLL, htSearch(ptrhtGlobal, porovnani ), NULL, NULL );
			generateInstruction(OC_NEPRIRAZENI, NULL, NULL, NULL );
		}
		else{
			//vygenerovani instrukci InsertFirst
			for (int i = 0; i < MAX_HTSIZE; ++i){
				if( (*ptrhtLocal)[i] ){
					if( (*ptrhtLocal)[i]->druh == ID_FUNCTION  ){
						generateCopyInstr(OC_CPY, (*ptrhtLocal)[i], NULL, NULL );
					}
				}
			}
			//predchozi list instrukci, protoze se volal push
			listIntrukci = &(ptrStack->top->lower->field);
			//htPrintTable(ptrhtGlobal);
			//param CALL, Funkce v GLOBALCE, NULL, NAVRATOVKA
			generateInstruction(OC_CALL, htSearch(ptrhtGlobal, porovnani ), NULL, htSearch( ptrhtLocal, porovnani ) );
			//PRIRAZ, NAVRATOVKA, NULL, IDCKO
			generateInstruction(OC_PRIRAZENI, htSearch( ptrhtLocal, porovnani ), NULL, searchFrames( id, ptrhtLastLocal, ptrhtGlobal ) );
			//nastaveni spravneho listu instrukci
			listIntrukci = &(ptrStack->top->field);
			//instrukce return na konci volane fce
			generateInstruction(OC_RET, NULL, NULL, NULL );
		}

		pocitadlo = 0;								//globalni promenna -> musime nulovat 
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return S_BEZ_CHYB;
	}
	return S_SYNTAKTICKA_CHYBA;
}

tChyba DRUH() {
	int analyza;	
	if(token.stav == s_cele_cislo || token.stav == s_desetinne_cislo || token.stav == s_logicka_hodnota || token.stav == s_string) {
		analyza = DTYPE2();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		return S_BEZ_CHYB;
	}
	else if(token.stav == s_identifikator) {
					//zapamatujeme si, ze bylo zadano id
		return S_BEZ_CHYB;
	}
		
	return S_SYNTAKTICKA_CHYBA;		

}



int syntakticka_anal() {		//pomocna funkce pro kontrolu cele syntakticke analyzy v main.c souboru
	int navrat;
	tokenInit(&token);
	token = getNextToken();
	if(token.stav == s_lex_error) {
		navrat = S_LEXIKALNI_CHYBA;
	}
	else
	navrat = PROGRAM();
    tokenFree(&token);
	return navrat;
}



