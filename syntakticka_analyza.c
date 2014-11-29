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
int dtype;
TData dat;
bool init;
bool prirovnani;
int pocitadlo;
int pocitani_parametru=0;

char* porovnani;
char* id;
char* funkce;
char* params;
tToken token;
tChyba DTYPE2();
tChyba DRUH();
tChyba FUNKCE();
tChyba PARAMS();
tChyba DTYPE();
tChyba FORW();
tChyba ACT_LIST();
tChyba DEKLARACE();
tChyba DALSI_P();
tChyba PROM();
tChyba ROZHODNI();
tChyba VESTAV();
tChyba LEXEM();
tChyba CYKLUS();
tChyba TERM();
tChyba UKONCOVACI();
tChyba DOPREDNE();
tChyba TERM2();

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
	if( ptr->druh == 5 ){ // string
	printf(" %d ", ptr->druh);

	for (int i = 0; i < ptr->data.param.numParam; i++){
		printf("%s ", ptr->data.param.param[i]);
		
	}
}
else if( ptr->druh == 6 ){ // bool
printf(" %d", ptr->druh);
}
else if( ptr->druh == 7 ){ // int
printf(" %d", ptr->druh);
}
else if( ptr->druh == 8 ){ // double

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
		
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_identifikator) {	//pokud nacitame a porovnavame terminaly, tak po kazdem porovnani nacitame dalsi token, aby se neporovnavalo to same
			
			if((funkce = malloc(strlen(token.data)+1)) == NULL) {
				return S_INTERNI_CHYBA;
			}
			
			dat.param.numParam = 0;
			strcpy(funkce, token.data);
			nasel = htSearch(ptrhtGlobal,funkce);
			if(nasel != NULL ) {
				if(nasel->init == false) {
					return S_SEMANTICKA_CHYBA_NEDEF;
				}
			}

			htInsert(ptrhtGlobal,funkce,dat, 4,0);	
			token = getNextToken();
			if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_leva_zavorka) {
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
					else if(!strcmp(token.data, "real")) {
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
							
						}
						else if (init == false) {
							htCompleteInsert(ptrhtGlobal,funkce, ID_FUNCTION, dtype, false);	
							free(funkce);

						}

						if(token.stav == s_strednik) {
							
							token = getNextToken();
							if(token.stav == s_lex_error) {
								return S_LEXIKALNI_CHYBA;
							}								//volani interpretu
							return FUNKCE();
						}

								//jakmile je vse splneno, vola funkce sama sebe znovu, protoze podle pravidel, jakmile se dojede na konec, tak je potreba zavolat FUNKCE() znovu, protoze napriklad funkci muze byt v kodu deklarovano mnoho a ne jen jedna
						
					}
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
	if(token.stav == s_identifikator) {
		if((params = malloc(strlen(token.data)+1)) == NULL) {
			return S_INTERNI_CHYBA;
		}
		
		strcpy(params, token.data);
		
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
			free(params);		
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}
	
		        return PARAMS();
		}
		return S_SYNTAKTICKA_CHYBA;
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
	
	if(token.stav == s_identifikator) {
		

		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			analyza = TERM2();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			return S_BEZ_CHYB;
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	return S_SYNTAKTICKA_CHYBA;		//zde je zmena, vraci se epsilon a to proto, protoze v pravidle ROZHODNI jsou volany 3 funkce a vzdy muze byt jen jedna dobre. Tzn. ty ostatní 2 budou epsilon.
}

tChyba VESTAV() {
	if(!strcmp(token.data, "length") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_string) {
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
	else if(!strcmp(token.data, "copy") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_string) {
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}

				else if(token.stav == s_strednik) {
					token = getNextToken();
					if(token.stav == s_lex_error) {
						return S_LEXIKALNI_CHYBA;
					}

					else if(token.stav == s_string) {
						token = getNextToken();
						if(token.stav == s_lex_error) {
							return S_LEXIKALNI_CHYBA;
						}

						else if(token.stav == s_strednik) {
							token = getNextToken();
							if(token.stav == s_lex_error) {
								return S_LEXIKALNI_CHYBA;
							}

							else if(token.stav == s_string) {
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
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_string) {
				token = getNextToken();
				if(token.stav == s_lex_error) {
					return S_LEXIKALNI_CHYBA;
				}

				else if(token.stav == s_strednik) {
					token = getNextToken();
					if(token.stav == s_lex_error) {
						return S_LEXIKALNI_CHYBA;
					}

					else if(token.stav == s_string) {
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
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}

		if(token.stav == s_leva_zavorka) {
			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			else if(token.stav == s_string) {
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
		init = true;
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return S_BEZ_CHYB;
	}
	else
		ret = pushFrame();
		if(ret != S_BEZ_CHYB) {
			return ret;
		}

		nasel = htSearch(ptrhtGlobal, funkce);
		if(nasel != NULL) {
			for(int i=0; i < nasel->data.param.numParam; i++) {
				htDeclInsert(ptrhtLocal, nasel->data.param.param[i], nasel->data.param.typeParam[i], ID_PARAM);	
			}
		}	

	return ACT_LIST();
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
	else if(!strcmp(token.data, "string") || token.stav == s_string) {
		
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
	else 
		
		analyza2 = VESTAV();
		if(analyza2 != S_BEZ_CHYB && analyza2 != S_EPS) {
			return analyza2;
		}
			
		if(token.stav == s_identifikator) {
			if((porovnani = malloc(strlen(token.data)+1)) == NULL) {
				return S_INTERNI_CHYBA;
			}

			strcpy(porovnani, token.data);
			pt = searchFrames(token.data);
			
			if(pt != NULL && pt->druh == ID_FUNCTION) {
				analyza1 = PROM();
				if(analyza1 != S_BEZ_CHYB) {
					return analyza1;
				}
				
				free(porovnani);
				return S_BEZ_CHYB;	
			}
			else 		
	//mame 2 promenne pro 2 epsilon a proto je zde porovnavame a pokud obe predesle funkce jsou epsilon, zavolam precedencni analyzu.(jakmile 2 ze 3 funkci jsou prazdne, tak je jasne, ze ta treti funkce to musi byt)
			analyza = precedencniSA();
						//nastaveni booleanu na true, pokud je zavolana precedencni analyza. Je to proto, protoze v navazujicim pravidle je na konci strednik, a pokud se vola precedencni tak bere tokeny tak dlouho, dokud se nedostane ke stredniku, coz je zarazka. Jakmile se zavola pote nextToken, nenacte se strednik ale dalsi token. Tudiz, jakmile je true, strednik nekontrolujeme.
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			return S_BEZ_CHYB;
			
		}
		else if(token.stav == s_cele_cislo || token.stav == s_logicka_hodnota || token.stav == s_desetinne_cislo || token.stav == s_string || token.stav == s_leva_zavorka) {
			analyza = precedencniSA();
			if(analyza != S_BEZ_CHYB) {
				return analyza;
			}
			return S_BEZ_CHYB;
		}
		return S_SYNTAKTICKA_CHYBA;
		

}

tChyba LEXEM() {
	if(token.stav == s_prirazeni) {
		prirovnani = true;
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
		if(token.stav == s_lex_error) {
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
		
		if(prirovnani == true) {
			pt = searchFrames( id );
			if(pt != NULL) {
				pt->init = true;
				prirovnani = false;
			}
			else {
				return S_SEMANTICKA_CHYBA_NEDEF;
			}
		}

		if(!strcmp(token.data, "integer")) {
			dtype2 = TYPEINT;
		}
		else if(!strcmp(token.data, "string")) {
			dtype2 = TYPESTR;
		}
		else if(!strcmp(token.data, "real")) {
			dtype2 = TYPEDOUBLE;
		}
		else if(!strcmp(token.data, "boolean")) {
			dtype2 = TYPEBOOL;
		}

		analyza = ROZHODNI();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}
		
		if(dtype2 != 0) {
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
			
					
				htDeclInsert(ptrhtLocal,id, dtype, ID_LOCAL);
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
		
		analyza = precedencniSA();
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
		}
		return S_SYNTAKTICKA_CHYBA;
	}
	else if(!strcmp(token.data, "write") && token.stav == s_klicove) {
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		
		if(token.stav == s_leva_zavorka) {
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
	
	if(token.stav == s_identifikator) {
		if((id = malloc(strlen(token.data)+1)) == NULL) {
			return S_INTERNI_CHYBA;
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
			
					
			htDeclInsert(ptrhtGlobal,id, dtype, ID_GLOBAL);
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
	if(token.stav == s_cele_cislo || token.stav == s_desetinne_cislo || token.stav == s_logicka_hodnota || token.stav == s_string || token.stav == s_identifikator) {
		analyza = DRUH();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}

		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		return TERM();
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
	
	TItem *pt;
	if(token.stav == s_cele_cislo || token.stav == s_desetinne_cislo || token.stav == s_logicka_hodnota || token.stav == s_string || token.stav == s_identifikator) {
		analyza = DRUH();
		if(analyza != S_BEZ_CHYB) {
			return analyza;
		}

		pt = htSearch(ptrhtGlobal, porovnani);
		if(token.stav != pt->data.param.typeParam[pocitadlo]) {
			return S_SEMANTICKA_CHYBA_TYPOVA;
		}

		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
		pocitadlo++;
		return TERM2();
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
			
			pt = htSearch(ptrhtGlobal, porovnani);
			if(token.stav != pt->data.param.typeParam[pocitadlo]) {
				return S_SEMANTICKA_CHYBA_TYPOVA;
			}

			token = getNextToken();
			if(token.stav == s_lex_error) {
				return S_LEXIKALNI_CHYBA;
			}

			pocitadlo++;
			return TERM2();
		}
	}
	else if(token.stav == s_prava_zavorka) {
		pt = htSearch(ptrhtGlobal, porovnani);
		if(pocitadlo != pt->data.param.numParam) {
			return S_SEMANTICKA_CHYBA_TYPOVA;
		}
		
		pocitadlo = 0;
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
		token = getNextToken();
		if(token.stav == s_lex_error) {
			return S_LEXIKALNI_CHYBA;
		}
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



