/*
 *  Tamgu (ÌÉêÍµ¨)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 */
/* --- CONTENTS ---
 Project    : Tamgu (ÌÉêÍµ¨)
 Version    : See tamgu.cxx for the version number
 filename   : tamgusound.h
 Date       : 2017/02/06
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
*/

#ifndef tamgusound_h
#define tamgusound_h


//YOU NEED TO DEFINE: SOUND on the compiling line

//We use libao, libmpg123 and libsndfile sur le raspberry
/*
sudo apt-get install libao-dev
sudo apt-get install libmpg123-dev
sudo apt-get install libsndfile1-dev
*/

//Documentations: 
/*
http://www.mega-nerd.com/libsndfile/
http://snippets.khromov.se/raspberry-pi-shairport-build-instructions/

For instance, to solve an error message that occurs on raspberry pi (but does not prevent the system from playing the music)

ìUnknown PCM cards.pcm.frontî

You can modify:

nano /usr/share/alsa/alsa.conf
Change the line ìpcm.front cards.pcm.frontî to ìpcm.front cards.pcm.defaultî
*/

//We have used the following examples to implement this library
/*
http://hzqtc.github.io/2012/05/play-mp3-with-libmpg123-and-libao.html
http://www.music.columbia.edu/pipermail/music-dsp/2002-January/047060.html
https://gist.github.com/maxammann/52d6b65b42d8ce23512a
*/

#ifdef WIN32
#define ssize_t size_t
#endif

//To play files
#include <ao/ao.h>

#ifdef USEMPG123
//Used to decode MP3 files...
#include <mpg123.h>
#endif

//To sample sound file before hand and get the right information to ao_play
#include <sndfile.h>

#define BITS 8

static void intialization() {
	static bool init = false;
	if (!init) {
		ao_initialize();
#ifdef USEMPG123
		mpg123_init();
#endif
		init = true;
	}
}

const int BUFFER_SIZE = 8192;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Tamgusound;
//This typedef defines a type "soundMethod", which expose the typical parameters of a new Tamgu method implementation
typedef Tamgu* (Tamgusound::*soundMethod)(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

//---------------------------------------------------------------------------------------------------------------------

class Tamgusound : public TamguObject {
    public:
    //We export the methods that will be exposed for our new object
    //this is a static object, which is common to everyone
    //We associate the method pointers with their names in the linkedmethods map
    static basebin_hash<soundMethod> methods;
    static hmap<string, string> infomethods;
    static  basebin_hash<unsigned long> exported;

    static short idtype;

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //Your personal variables here...
#ifdef USEMPG123
	mpg123_handle *mh;
	unsigned char *mp3_buffer;
	size_t mp3_buffer_size;
#endif

	ao_sample_format format;

	SNDFILE* soundfile;
	short *buffer;

	int driver;

	//Tamgu variable
	string path;
	bool loaded;
	bool stop;
	bool mp3;

	ao_device *device;

    //---------------------------------------------------------------------------------------------------------------------
    Tamgusound(TamguGlobal* g, Tamgu* parent = NULL) : TamguObject(g, parent) {
        //Do not forget your variable initialisation
		intialization();
		loaded = false;
		stop = false;
		mp3 = false;
		soundfile = NULL;
		buffer = NULL;
		driver = ao_default_driver_id();
		device = NULL;
#ifdef USEMPG123
		mp3_buffer_size = 0;
		mh = NULL;
		mp3_buffer = NULL;
#endif
	}

	~Tamgusound() {
		if (device != NULL)
			ao_close(device);

		if (soundfile != NULL)
			sf_close(soundfile);

		if (buffer != NULL)
			free(buffer);

#ifdef USEMPG123
		if (mp3_buffer != NULL)
			free(mp3_buffer);

		if (mh != NULL) {
			mpg123_close(mh);
			mpg123_delete(mh);
		}
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


    //----------------------------------------------------------------------------------------------------------------------
	Tamgu* Put(Tamgu* v, Tamgu* i, short idthread);
    
	Tamgu* Eval(Tamgu* context, Tamgu* v, short idthread);


    short Type() {
        return Tamgusound::idtype;
    }

    string Typename() {
        return "sound";
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
        return new Tamgusound(NULL);
    }

    TamguIteration* Newiteration(bool direction) {
        return aITERNULL;
    }

    static void AddMethod(TamguGlobal* g, string name, soundMethod func, unsigned long arity, string infos);
    static bool InitialisationModule(TamguGlobal* g, string version);

    //---------------------------------------------------------------------------------------------------------------------
    //This SECTION is for your specific implementation...
    //This is an example of a function that could be implemented for your needs.
	Tamgu* MethodLoad(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodPlay(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodStop(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodInfos(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodReset(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodOpen(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodClose(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodDecode(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);
	Tamgu* MethodEncode(Tamgu* contextualpattern, short idthread, TamguCall* callfunc);

    

    //---------------------------------------------------------------------------------------------------------------------

    //ExecuteMethod must be implemented in order to execute our new Tamgu methods. This method is called when a TamguCallMethodMethod object
    //is returned by the Declaration method.
    Tamgu* CallMethod(short idname, Tamgu* contextualpattern, short idthread, TamguCall* callfunc) {
        //This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
        //and run it with the proper parameters. This is the right call which should be invoked from within a class definition
        return (this->*methods.get(idname))(contextualpattern, idthread, callfunc);
    }

	Tamgu* loading(short idthread);

    void Clear() {
        //To set a variable back to empty
    }

    void Clean() {
        //Your cleaning code
    }

    string String() {
        Locking _lock(this);
        return path;
    }

  
};

#endif


