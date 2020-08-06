/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgubitmap.cxx
 Date       : 2017/09/01
 Purpose    : bitmap implementation (based on FLTK)
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgubitmap.h"

//We need to declare once again our local definitions.
basebin_hash<bitmapMethod>  Tamgubitmap::methods;
hmap<string, string> Tamgubitmap::infomethods;
basebin_hash<unsigned long> Tamgubitmap::exported;

short Tamgubitmap::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgubitmap::AddMethod(TamguGlobal* global, string name, bitmapMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Tamgubitmap::InitialisationModule(TamguGlobal* global, string version) {
	methods.clear();
	infomethods.clear();
	exported.clear();

	Tamgubitmap::idtype = global->Getid("bitmap");

	Tamgubitmap::AddMethod(global, "_initial", &Tamgubitmap::MethodInitial, P_THREE, "_initial(ivector,int l,int h): Loading of a bitmap from a vector. 'l' is the lenght, 'h' the hight.");
	Tamgubitmap::AddMethod(global, "load", &Tamgubitmap::MethodLoad, P_THREE, "load(vector bitmaps,int w,int h): Load a bitmap");


	global->newInstance[Tamgubitmap::idtype] = new Tamgubitmap(NULL, global);
	global->RecordMethods(Tamgubitmap::idtype,Tamgubitmap::exported);

	return true;
}





