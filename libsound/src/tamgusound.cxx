/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : tamgusound.cxx
 Date       : 2017/09/01
 Purpose    : sound players based on the libraries mpg123 and ao
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#include "tamgu.h"
#include "tamgusound.h"
#include "tamguversion.h"
#include "atoms.h"
#include "tamguivector.h"
#include "tamgumapsi.h"
#include "tamgump3.h"

//We need to declare once again our local definitions.
basebin_hash<soundMethod>  Tamgusound::methods;
hmap<string, string> Tamgusound::infomethods;
basebin_hash<unsigned long> Tamgusound::exported;

short Tamgusound::idtype = 0;

//------------------------------------------------------------------------------------------------------------------
//If you need to implement an external library... Otherwise remove it...
//When Tamgu (탐구) loads a library, it looks for this method in particular, which then make this object available into Tamgu (탐구)
extern "C" {
    Exporting bool sound_InitialisationModule(TamguGlobal* global, string version) {
        if (version != TamguVersion())
            return false;

        global->Update();

        Tamgump3::InitialisationModule(global, version);
        return Tamgusound::InitialisationModule(global, version);
    }
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
void Tamgusound::AddMethod(TamguGlobal* global, string name, soundMethod func, unsigned long arity, string infos) {
	short idname = global->Getid(name);
	methods[idname] = func;
	infomethods[name] = infos;
	exported[idname] = arity;
}

bool Tamgusound::InitialisationModule(TamguGlobal* global, string version) {
	Tamgusound::idtype = global->Getid("sound");

	Tamgusound::AddMethod(global, "_initial", &Tamgusound::MethodLoad, P_ONE, "_initial(string pathname): Load a sound file");
	Tamgusound::AddMethod(global, "load", &Tamgusound::MethodLoad, P_ONE, "load(string pathname): Load a sound file");
	Tamgusound::AddMethod(global, "play", &Tamgusound::MethodPlay, P_NONE | P_ONE, "play(bool rst): play the sound. If rst is true start back to beginning");
	Tamgusound::AddMethod(global, "stop", &Tamgusound::MethodStop, P_NONE, "stop(): stop the sound");
	Tamgusound::AddMethod(global, "parameters", &Tamgusound::MethodInfos, P_NONE | P_ONE, "parameters(): Return the sound configuration values as a simap. Parameters rate and channels can be modified");
	Tamgusound::AddMethod(global, "clear", &Tamgusound::MethodReset, P_NONE, "clear(): Reset the audio channel");
	Tamgusound::AddMethod(global, "open", &Tamgusound::MethodOpen, P_ONE, "open(simap p): Open a sound channel with parameters p (server side)");
	Tamgusound::AddMethod(global, "close", &Tamgusound::MethodClose, P_NONE, "close(): close the sound channel (server side)");
	Tamgusound::AddMethod(global, "decode", &Tamgusound::MethodDecode, P_ONE, "decode(ivector): decode a sound channel fill in an ivector (client side)");
	Tamgusound::AddMethod(global, "encode", &Tamgusound::MethodEncode, P_ONE, "encode(ivector): play a sound channel from a decoding sessions (server side)");



	global->newInstance[Tamgusound::idtype] = new Tamgusound(global);
	global->RecordMethods(Tamgusound::idtype,Tamgusound::exported);

	return true;
}



Tamgu* Tamgusound::Put(Tamgu* index, Tamgu* value, short idthread) {
	return this;
}

Tamgu* Tamgusound::Eval(Tamgu* context, Tamgu* index, short idthread) {
	return this;
}

Tamgu* Tamgusound::loading(short idthread) {

	SF_INFO sfinfo;
	//For sampling sound files other than MP3

#ifdef USEMPG123
	if (path.find(".mp3") != string::npos || path.find(".MP3") != string::npos) {
		int channels, encoding, err;
		long rate;

		//we test if it is a MP3 file...
		mh = mpg123_new(NULL, &err);
		mp3_buffer_size = mpg123_outblock(mh);
		mp3_buffer = (unsigned char*)malloc(mp3_buffer_size * sizeof(unsigned char));

		if (mpg123_open(mh, STR(path)) != MPG123_OK)
			return globalTamgu->Returnerror("SND(003): Loading error", idthread);

		if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK)
			return globalTamgu->Returnerror("SND(013): Unknown format", idthread);

		/* set the output format and open the output device */
		format.bits = mpg123_encsize(encoding) * BITS;
		format.rate = rate;
		format.channels = channels;
		format.byte_format = AO_FMT_NATIVE;
		format.matrix = 0;
		mp3 = true;

		device = ao_open_live(driver, &format, NULL);

		//In this case, we need to do it live from the buffer initialized with the mp3 decoder
		if (device == NULL)
			return globalTamgu->Returnerror("SND(004): Cannot open mp3 audio", idthread);

		return aTRUE;
	}
#endif
	//For non MP3 files (wav or others)
	//Sampling with libsndfile
	if (!(soundfile = sf_open(STR(path), SFM_READ, &sfinfo))) {
		sf_perror(NULL);
		return globalTamgu->Returnerror("SND(003): Loading error", idthread);
	}

	//Basically, we have now all the right information about this file
	switch (sfinfo.format & SF_FORMAT_SUBMASK) {
	case SF_FORMAT_PCM_16:
		format.bits = 16;
		break;
	case SF_FORMAT_PCM_24:
		format.bits = 24;
		break;
	case SF_FORMAT_PCM_32:
		format.bits = 32;
		break;
	case SF_FORMAT_PCM_S8:
		format.bits = 8;
		break;
	case SF_FORMAT_PCM_U8:
		format.bits = 8;
		break;
	default:
		format.bits = 16;
		break;
	}

	format.channels = sfinfo.channels;
	format.rate = sfinfo.samplerate;
	format.byte_format = AO_FMT_NATIVE;
	format.matrix = 0;

	buffer = (short*)calloc(BUFFER_SIZE, sizeof(short));
	device = ao_open_live(driver, &format, NULL);
	//In this case, we need to do it live from the buffer initialized with the mp3 decoder
	if (device == NULL)
		return globalTamgu->Returnerror("SND(004): Cannot open audio", idthread);

	return aTRUE;
}

Tamgu* Tamgusound::MethodLoad(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Locking _lock(this);
	if (loaded)
		return globalTamgu->Returnerror("SND(001): Already loaded", idthread);
	//In our example, we have only two parameters
	//0 is the first parameter and so on...
	path = callfunc->Evaluate(0, contextualpattern, idthread)->String();
	Tamgu* res = loading(idthread);
	if (res != aTRUE)
		return res;
	loaded = true;
	return aTRUE;
}

Tamgu* Tamgusound::MethodPlay(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Locking _lock(this);
	bool rset = false;
	if (callfunc->Size() == 1) {
		Tamgu* k = callfunc->Evaluate(0, contextualpattern, idthread);
		if (k->Type() == a_ivector)
			return MethodEncode(contextualpattern, idthread, callfunc);
		rset = k->Boolean();
	}
	if (!loaded)
		return globalTamgu->Returnerror("SND(002): Load your file first", idthread);
	stop = false;

#ifdef USEMPG123
	if (mp3) {
		//In case, the user has requested the stream to be reset to 0
		if (rset)
			mpg123_seek(mh, 0, SEEK_SET);
		size_t done = 1;
		/* decode and play */
		while (!stop && mpg123_read(mh, mp3_buffer, mp3_buffer_size, &done) == MPG123_OK && !globalTamgu->Error(idthread) && done != 0)
			ao_play(device, (char*)mp3_buffer, done);
		return aTRUE;
	}
#endif

	bool error = false;
	int read;
	if (rset)
		sf_seek(soundfile, 0, SEEK_SET); //We reset the stream to the beginning
	read = sf_read_short(soundfile, buffer, BUFFER_SIZE);
	while (read != 0 && !stop && !globalTamgu->Error(idthread)) {
		if (ao_play(device, (char *)buffer, (uint_32)(read * sizeof(short))) == 0) {
			error = true;
			break;
		}
		read = sf_read_short(soundfile, buffer, BUFFER_SIZE);
	}
	if (error) {
		//We reset the device
		ao_close(device);
		device = ao_open_live(driver, &format, NULL);
		return globalTamgu->Returnerror("SND(005): Erreur while playing sound", idthread);
	}
	return aTRUE;
}

Tamgu* Tamgusound::MethodStop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	//Of course to be able to stop a sound, you need to play your SOUND file IN A THREAD...
	stop = true;
	return aTRUE;
}

Tamgu* Tamgusound::MethodInfos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Locking _lock(this);
	if (!loaded)
		return globalTamgu->Returnerror("SND(002): Load your file first", idthread);
	string k;
	long v;
	if (callfunc->Size() == 0) {
		Tamgumapsi* kimap = (Tamgumapsi*)globalTamgu->Provideinstance("mapsi", idthread);
		k = "channels"; v = format.channels;
		kimap->values[k] = v;
		k = "rate"; v = format.rate;
		kimap->values[k] = v;
		k = "byte_format"; v = format.byte_format;
		kimap->values[k] = v;
		k = "mp3"; v = mp3;
		kimap->values[k] = v;
		k = "bits"; v = format.bits;
		kimap->values[k] = v;
		k = "size";
		if (mp3) {
#ifdef USEMPG123
			v = mp3_buffer_size;
#endif
		}
		else
			v = BUFFER_SIZE;
		kimap->values[k] = v;
		return kimap;
	}

	Tamgu* kmap = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!kmap->isMapContainer())
		return globalTamgu->Returnerror("SND(008): The argument shoud be a map", idthread);
	TamguIteration* itrloc = kmap->Newiteration(false);
	for (itrloc->Begin(); itrloc->End() != aTRUE; itrloc->Next()) {
		k = itrloc->Keystring();
		v = itrloc->Valueinteger();
		if (k == "channels")
			format.channels = v;
		else
		if (k == "rate")
			format.rate = v;
	}
	itrloc->Release();
	//We reset the device
	ao_close(device);
	device = ao_open_live(driver, &format, NULL);
	return aTRUE;
}

Tamgu* Tamgusound::MethodReset(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Locking _lock(this);

#ifdef USEMPG123
	if (mp3) {
		if (mh != NULL)
			mpg123_seek(mh, 0, SEEK_SET);
		return aTRUE;
	}
#endif

	if (soundfile != NULL)
		sf_seek(soundfile, 0, SEEK_SET); //We reset the stream to the beginning
	return aTRUE;
}

Tamgu* Tamgusound::MethodOpen(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Locking _lock(this);
	string k;
	long v;
	Tamgu* kmap = callfunc->Evaluate(0, contextualpattern, idthread);
	if (!kmap->isMapContainer())
		return globalTamgu->Returnerror("SND(008): The argument shoud be a map", idthread);
	int sz = BUFFER_SIZE;
	TamguIteration* itrloc = kmap->Newiteration(false);
	for (itrloc->Begin(); itrloc->End() != aTRUE; itrloc->Next()) {
		k = itrloc->Keystring();
		v = itrloc->Valueinteger();
		if (k == "channels")
			format.channels = v;
		else if (k == "rate")
			format.rate = v;
		else if (k == "byte_format")
			format.byte_format = v;
		else if (k == "bits")
			format.bits = v;
		else if (k == "mp3")
			mp3 = v;
		else if (k == "size")
			sz = v;
	}
	itrloc->Release();
	if (mp3) {
#ifdef USEMPG123
		mp3_buffer_size = sz;
		if (mp3_buffer != NULL)
			free(mp3_buffer);
		mp3_buffer = (unsigned char*)malloc(mp3_buffer_size * sizeof(unsigned char));
#endif
	}
	else {
		if (buffer == NULL)
			buffer = (short*)calloc(BUFFER_SIZE, sizeof(short));
	}
	format.matrix = 0;
	//We reset the device
	if (device != NULL)
		ao_close(device);
	//We open it...
	device = ao_open_live(driver, &format, NULL);
	return aTRUE;
}

Tamgu* Tamgusound::MethodClose(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Locking _lock(this);
	if (device != NULL) {
		ao_close(device);
		device = NULL;
		return aTRUE;
	}
	return aFALSE;
}

Tamgu* Tamgusound::MethodDecode(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Locking _lock(this);
	if (!loaded)
		return globalTamgu->Returnerror("SND(002): Load your file first", idthread);
	Tamgu* k = callfunc->Evaluate(0, contextualpattern, idthread);
	if (k->Type() != a_ivector)
		return globalTamgu->Returnerror("SND(010): The argument should be an ivector", idthread);
	Tamguivector* kvect = (Tamguivector*)k;
	size_t read, i;

#ifdef USEMPG123
	if (mp3) {
		/* decode only*/
		if (mpg123_read(mh, mp3_buffer, mp3_buffer_size, &read) == MPG123_OK) {
			Locking _lockvect(kvect);
			kvect->values.clear();
			for (i = 0; i < read; i++)
				kvect->values.push_back(mp3_buffer[i]);
			if (!read)
				return aFALSE;
			return aTRUE;
		}
		return aFALSE;
	}
#endif

	read = sf_read_short(soundfile, buffer, BUFFER_SIZE);
	Locking _lockvect(kvect);
	kvect->values.clear();
	for (i = 0; i < read; i++)
		kvect->values.push_back(buffer[i]);
	if (!read)
		return aFALSE;
	return aTRUE;
}

Tamgu* Tamgusound::MethodEncode(Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
	Locking _lock(this);
	if (device == NULL)
		return globalTamgu->Returnerror("SND(011): Please open the audio first", idthread);
	Tamgu* k = callfunc->Evaluate(0, contextualpattern, idthread);
	if (k->Type() != a_ivector)
		return globalTamgu->Returnerror("SND(010): The argument should be an ivector", idthread);
	Tamguivector* kvect = (Tamguivector*)k;
	stop = false;
	int read = kvect->Size();
	int i;

#ifdef USEMPG123
	if (mp3) {
		{
			Locking _lockvect(kvect);
			read = kvect->values.size();
			for (i = 0; i < read; i++)
				mp3_buffer[i] = (char)kvect->values[i];
		}
		if (!read)
			return aFALSE;
		if (ao_play(device, (char*)mp3_buffer, read) == 0)
			return globalTamgu->Returnerror("SND(005): Erreur while playing sound", idthread);
		return aTRUE;
	}
#endif

	{
		Locking _lockvect(kvect);
		read = kvect->values.size();
		for (i = 0; i < read; i++)
			buffer[i] = (short)kvect->values[i];
	}
	if (!read)
		return aFALSE;
	if (ao_play(device, (char *)buffer, (uint_32)(read * sizeof(short))) == 0)
		return globalTamgu->Returnerror("SND(005): Erreur while playing sound", idthread);
	return aTRUE;
}

