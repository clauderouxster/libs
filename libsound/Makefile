###################################################################
include ../Makefile.in
################ To compile Tamgu#################################
SOURCESOUND = tamgusound.cxx tamgump3.cxx
#------------------------------------------------------------
LBINPATH = ../$(BINPATH)
LOBJPATH = ../$(LIBOBJPATH)
###################################################################
OBJECTSOUND = $(SOURCESOUND:%.cxx=$(LOBJPATH)/sound/%.o)
#------------------------------------------------------------
TamguBASICFLAGS = -w -c -fPIC -O3 -DUNIX $(MULTIGA) $(INTELINT) $(REGEX) $(SPECFLAGS)
TamguFLAGS = $(C++11Flag) $(TamguBASICFLAGS) $(SOUNDFLAG) $(FLAGMPG123)
Tamguincludes= -Iinclude -Iinclude/ao -I../include $(INCLUDEPATH)
TamguSYSTEMLIBS = $(SYSTEMSPATH) -L$(SYSTEMSPATH) $(MACLIBS) $(LIBSOUND) $(LIBMPG123)
#------------------------------------------------------------
$(LOBJPATH)/sound/%.o: src/%.cxx
	$(COMP) -Iinclude $(TamguFLAGS) $(Tamguincludes) $< -o $@

$(LOBJPATH)/sound/%.o: src/%.mm
	$(COMPPLUSPLUS) -Iinclude $(TamguFLAGS) $(Tamguincludes) $< -o $@
#------------------------------------------------------------
sound: install $(OBJECTSOUND) $(OBJECTLIBMM)
	$(COMPPLUSPLUS) -shared -o $(LBINPATH)/libsound.so $(OBJECTSOUND) $(OBJECTLIBMM) $(TamguSYSTEMLIBS) $(LIBREGEX) -L$(LBINPATH) -ltamgu

install:
	mkdir -p $(LOBJPATH)/sound

clean:
	rm -f $(LOBJPATH)/sound/*.o

