/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamguchoice.h
 Date       : 2017/07/22
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamguchoice_h
#define tamguchoice_h
#include "tamguwidget.h"

class Tamguchoice;
class ChoiceCallback {
public:
	
	Tamgu* function;
	Tamguchoice* choice;
	Tamgu* object;

	ChoiceCallback(Tamgu* func, Tamgu* o, Tamguchoice* ch) {
		function = func;
		choice = ch;
		object = o;
	}

};

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamguchoice;
//This typedef defines a type "choiceMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamguchoice::*choiceMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamguchoice : public Tamguwidget {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static basebin_hash<choiceMethod> methods;
    static hmap<string, string> infomethods;
    static  basebin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
	Fl_Choice* choice;
	Fl_Menu_Item* flmenu;
	int flnbelements;

	int fontsize;
	int font;
	int fontcolor;
	char txt[1024];

    //---------------------------------------------------------------------------------------------------------------------
	Tamguchoice(TamguGlobal* g, Tamgu* parent = NULL) : Tamguwidget(g, parent) {
        //Do not forget your variable initialisation
		choice = NULL;
		flnbelements = 0;
		font = FL_HELVETICA;
		fontsize = 12;
		fontcolor = FL_BLACK;
		flmenu = NULL;
	}

	void itemclose() {
		if (flmenu != NULL) {
			for (int i = 0; i < flnbelements; i++) {
				free((char*)flmenu[i].text);
				delete (ChoiceCallback*)flmenu[i].user_data();
			}
			delete[] flmenu;
			flmenu = NULL;
		}
		if (choice != NULL)
			delete choice;
		choice = NULL;
		clean();
	}

	Fl_Widget* Widget() {
		return choice;
	}

	void ResetWidget() {
        if (choice != NULL)
            delete choice;
		choice = NULL;
	}
    //----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* index, Tamgu* value, short idthread);
	Tamgu* Eval(Tamgu* context, Tamgu* index, short idthread);


    short Type() {
        return Tamguchoice::idtype;
    }

    string Typename() {
        return "choice";
    }

    Tamgu* Atom(bool forced=false) {
        return this;
    }

    //---------------------------------------------------------------------------------------------------------------------
    //Declaration
    //All our methods must have been declared in tamguexportedmethods... See MethodInitialization below
    bool isDeclared(short n) {
        if (exported.find(n) != exported.end())
            return true;
        return false;
    }

    Tamgu* Newinstance(short, Tamgu* f = NULL) {
		Tamguchoice* a = new Tamguchoice(globalTamgu);
		a->function = f;
		return a;
    }

	TamguIteration* Newiteration(bool direction);

    static void AddMethod(TamguGlobal* g, string name, choiceMethod func, unsigned long arity, string infos);
    
	static bool InitialisationModule(TamguGlobal* g, string version);

	
	void Methods(Tamgu* v) {
		hmap<string, string>::iterator it;
		for (it = infomethods.begin(); it != infomethods.end(); it++)
			v->storevalue(it->first);
	}

	string Info(string n) {

		if (infomethods.find(n) != infomethods.end())
			return infomethods[n];
		return "Unknown method";
	}



	bool isString() {
		return false;
	}

	bool isNumber() {
		return false;
	}

	bool isContainer() {
		return false;
	}

	virtual bool isValueContainer() {
		return false;
	}

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
    //Tamgu* MethodSize(TamguGlobal* global,Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {return aZERO;}

    
	Tamgu* MethodCreate(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodValue(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFont(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodMenu(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodFontSize(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		if (globalTamgu->Error(idthread))
			return globalTamgu->Errorobject(idthread);

		if (Stopall())
			return aNULL;

		///Specific section to handle threads... One exception the redraw function...
		if (idthread && callfunc->Size() != 0) {
			//In this case, we do not want to process the method, we postpone its activation
			RecordGUIObject(callfunc, idthread);
			return aNULL;
		}

		return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    void Clear() {
        //To set a variable back to empty
		
    }

    void Clean() {
        //Your cleaning code
    }

	long Size() {
		return 0;
	}

	uchar Byte() {
		
		return 0;
	}

	long Integer() {
		
		return 0;
	}
	
	double Float() {
		
		return 0;
	}
	
	string String() {
		
		return "";
	}

	
	bool Boolean() {
        if (choice != NULL)
            return true;
        return false;
	}

    
    //Basic operations    
	Tamgu* andset(Tamgu* a, bool autoself) {
		
	    return this;
	}

	Tamgu* orset(Tamgu* a, bool autoself) {
		
	    return this;
	}

	Tamgu* xorset(Tamgu* a, bool autoself) {
		
	    return this;
	}

	Tamgu* plus(Tamgu* a, bool autoself) {
		
	    return this;
	}
	Tamgu* minus(Tamgu* a, bool autoself) {
		
	    return this;
	}

	Tamgu* multiply(Tamgu* a, bool autoself) {
		
	    return this;
	}

	Tamgu* divide(Tamgu* a, bool autoself) {
		
	    return this;

	}
	Tamgu* mod(Tamgu* a, bool autoself) {
		
	    return this;
	}

	Tamgu* shiftleft(Tamgu* a, bool autoself) {
		
	    return this;
	}


	Tamgu* shiftright(Tamgu* a, bool autoself) {
		
	    return this;
	}

	Tamgu* power(Tamgu* a, bool autoself) {
		
	    return this;
	}

	Tamgu* less(Tamgu* a) {
		return aFALSE;
	}
	Tamgu* more(Tamgu* a) {
		return aFALSE;
	}
	Tamgu* same(Tamgu* a) {
		return aFALSE;
	}
	Tamgu* different(Tamgu* a) {
		return aFALSE;
	}

	Tamgu* lessequal(Tamgu* a) {
		return aFALSE;
	}

	Tamgu* moreequal(Tamgu* a) {
		return aFALSE;
	}

	Tamgu* plusplus() {
		return this;
	}

	Tamgu* minusminus() {
		return this;
	}
};

#endif








