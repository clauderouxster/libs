/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgump3.cxx
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "instructions.h"
#include "tamgump3.h"

//We need to declare once again our local definitions.
basebin_hash<mp3Method>  Tamgump3::methods;
hmap<string, string> Tamgump3::infomethods;
basebin_hash<unsigned long> Tamgump3::exported;

short Tamgump3::idtype = 0;


//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgump3::AddMethod(TamguGlobal* global, string name, mp3Method func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Tamgump3::InitialisationModule(TamguGlobal* global, string version) {
	Tamgump3::idtype = global->Getid("mp3");

	Tamgump3::AddMethod(global, "_initial", &Tamgump3::MethodLoad, P_ONE, "_initial(string pathname): Load the sound pathname.");
	Tamgump3::AddMethod(global, "load", &Tamgump3::MethodLoad, P_ONE, "load(string pathname): Load the sound pathname.");
	Tamgump3::AddMethod(global, "play", &Tamgump3::MethodPlay, P_NONE, "play(): play the sound");
	Tamgump3::AddMethod(global, "stop", &Tamgump3::MethodStop, P_NONE, "stop(): stop the sound");

	global->newInstance[Tamgump3::idtype] = new Tamgump3(global);
	global->RecordMethods(Tamgump3::idtype, Tamgump3::exported);

	return true;
}

#ifdef MACSOUND
void completionCallback(SystemSoundID  mySSID, void* data) {
	Tamgump3* ksnd = (Tamgump3*)data;
	TamguFunction* func = (TamguFunction*)ksnd->function;
	TamguCallFunction kfunc(func);

	ksnd->Setreference();
	kfunc.arguments.push_back(ksnd);
	Tamgu* ret = kfunc.Eval(aNULL, aNULL, globalTamgu->GetThreadid());
	ksnd->Resetreference();
	ret->Release();
}
#endif

Tamgu* Tamgump3::Put(Tamgu* index, Tamgu* value, short idthread) {
	return this;
}

Tamgu* Tamgump3::Eval(Tamgu* context, Tamgu* index, short idthread) {
	return this;
}


Tamgu* Tamgump3::MethodLoad(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
#ifdef MACSOUND
	if (created) {
		if (function != NULL)
			removeSoundFunction(&sound);
		cleanSoundId(&sound);
	}
#endif
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	path = callfunc->Evaluate(0, contextualpattern, idthread)->String();
#ifdef MACSOUND
	if (createSoundId(path.c_str(), &sound) == false)
		return globalTamgu->Returnerror("SND(001): Unknown file", idthread);
	if (function != NULL)
		addSoundFunction(&sound, completionCallback, (void*)this);
#endif
#ifdef WIN32
	wstring base;
	s_utf8_to_unicode(base, (unsigned char*)path.c_str(), path.size());
	if (sound.Load((WCHAR*)base.c_str()) == false)
		return globalTamgu->Returnerror("SND(005): Error during loading", idthread);
#endif
	loaded = true;
	created = true;
	return aTRUE;
}

Tamgu* Tamgump3::MethodPlay(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (!loaded)
		return globalTamgu->Returnerror("SND(003): Please load a file first", idthread);
#ifdef MACSOUND
	playSoundId(&sound);
#endif
#ifdef WIN32
	sound.Play();
#endif
	return aTRUE;
}

Tamgu* Tamgump3::MethodStop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	if (!loaded)
		return globalTamgu->Returnerror("SND(003): Please load a file first", idthread);
#ifdef MACSOUND
	if (created) {
		if (function != NULL)
			removeSoundFunction(&sound);
		cleanSoundId(&sound);
		if (createSoundId(path.c_str(), &sound) == false)
			return globalTamgu->Returnerror("SND(001): Unknown file", idthread);
		if (function != NULL)
			addSoundFunction(&sound, completionCallback, (void*)this);
	}
#endif
#ifdef WIN32
	sound.Stop();
#endif
	return aTRUE;
}






