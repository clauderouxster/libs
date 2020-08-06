/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgump3.h
 Date       : 2017/07/11
 Purpose    : TAMGU API to play MP3 file
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#ifndef tamgump3_h
#define tamgump3_h


#ifdef MACSOUND
typedef unsigned int SystemSoundID;
typedef void(*AudioServicesSystemSoundCompletionProc)(SystemSoundID  mySSID, void* myself);

bool createSoundId(const char* thepath, SystemSoundID* _soundId);
void cleanSoundId(SystemSoundID*);
void playSoundId(SystemSoundID*);
void addSoundFunction(SystemSoundID* soundid, AudioServicesSystemSoundCompletionProc c, void* data);
void removeSoundFunction(SystemSoundID* soundid);
void completionCallback(SystemSoundID  mySSID, void* myself);
#endif

#ifdef WIN32
#include "Mp3.h"
#endif



//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgump3;
//This typedef defines a type "mp3Method", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgump3::*mp3Method)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgump3 : public TamguObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static basebin_hash<mp3Method> methods;
    static hmap<string, string> infomethods;
    static  basebin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
#ifdef MACSOUND
	SystemSoundID sound;
#endif
#ifdef WIN32
	Mp3 sound;
#endif

	string path;
	bool loaded;
	bool created;

#ifdef MACSOUND
	Tamgu* function;
#endif
    //---------------------------------------------------------------------------------------------------------------------
    Tamgump3(TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
        //Do not forget your variable initialisation
		loaded = false;
#ifdef MACSOUND
		function = NULL;
#endif
		created = false;
	}


	~Tamgump3() {
#ifdef MACSOUND
		if (created) {
			if (function != NULL)
				removeSoundFunction(&sound);
			cleanSoundId(&sound);
		}
#endif
#ifdef WIN32
		sound.Cleanup();
#endif
	}

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

    unsigned long CallBackArity() {
        return P_ONE;
    }


    //----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);
    
	Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);

#ifdef MACSOUND
	void Addfunction(Tamgu* a) {
		function = a;
	}
#endif

    short Type() {
        return Tamgump3::idtype;
    }

    string Typename() {
        return "mp3";
    }

    bool isString() {
        return false;
    }

    bool isNumber() {
        return false;
    }

    bool isBoolean() {
        return false;
    }

    bool isFrame() {
        return false;
    }

    Tamgu* Atom(bool forced=false) {
        return this;
    }

	//If it is too complex to duplicate an element (for instance when passed to a function)
	//then you can use return false...  Usually in that case, Atom should always be just: return this;
	bool duplicateForCall() {
		return false;
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
		Tamgump3* a= new Tamgump3(NULL);
#ifdef MACSOUND
		a->function = f;
#endif
		return a;
    }

    TamguIteration* Newiteration(bool direction) {
        return aITERNULL;
    }

    static void AddMethod(TamguGlobal* g, string name, mp3Method func, unsigned long arity, string infos);
    static bool InitialisationModule(TamguGlobal* g, string version);

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
	Tamgu* MethodLoad(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPlay(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodStop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

    void Clear() {
        //To set a variable back to empty
    }

    void Clean() {
        //Your cleaning code
    }

    string String() {
        Locking _lock(this);
        return "";
    }

    //wstring UString() {
    //   Locking _lock(this);
    //  return L"";
    //	}

    long Integer() {
        Locking _lock(this);
        return 0;
    }
    double Float() {
        Locking _lock(this);
        return 0;
    }
    BLONG Long() {
        Locking _lock(this);
        return 0;
    }

    bool Boolean() {
        Locking _lock(this);
        return false;
    }

    
    //Basic operations
    long Size() {
        Locking _lock(this);
        return 0;
    }
    
    Tamgu* andset(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Tamgu* orset(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Tamgu* xorset(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    
    Tamgu* plus(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Tamgu* minus(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Tamgu* multiply(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }

    Tamgu* divide(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    Tamgu* power(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    Tamgu* shiftleft(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    Tamgu* shiftright(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    Tamgu* mod(Tamgu* a, bool itself) {
        Locking _lock(this);
        return this;
    }
    
    Tamgu* less(Tamgu* a) {
        Locking _lock(this);
        return aFALSE;
    }
    
    Tamgu* more(Tamgu* a) {
        Locking _lock(this);
        return aFALSE;
    }

    Tamgu* same(Tamgu* a) {
        Locking _lock(this);
        if (a == this)
            return aTRUE;
        return aFALSE;
    }

    Tamgu* different(Tamgu* a) {
        Locking _lock(this);
        if (same(a) == aFALSE)
            return aTRUE;
        return aFALSE;
    }

    Tamgu* lessequal(Tamgu* a) {
        Locking _lock(this);
        return aFALSE;
    }
    
    Tamgu* moreequal(Tamgu* a) {
        Locking _lock(this);
        return aFALSE;
    }

};

#endif


