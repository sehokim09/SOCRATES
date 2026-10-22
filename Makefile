##########################  Macro Definitions  ############################

# Let's try to auto-detect what platform we're on.  
# If this fails, set PLATFORM manually in the else block.
AUTOPLATFORM = Failed
ifeq ($(MSYSTEM),MINGW32)
	AUTOPLATFORM = Succeeded
	PLATFORM = __MSYS__
endif
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	AUTOPLATFORM = Succeeded
	PLATFORM = __linux__
endif
ifeq ($(UNAME_S),Darwin)
	AUTOPLATFORM = Succeeded
	PLATFORM = __APPLE__
endif
ifeq ($(AUTOPLATFORM),Failed)
	# Autodetect failed.  Set platform manually.
	#PLATFORM = __APPLE__
	#PLATFORM = __linux__
	PLATFORM = __MSYS__
endif

#TIMEFLAG =
TIMEFLAG = -D _USE_SYSTEM_TIME_

OPENMPFLAG = -fopenmp
#OPENMPFLAG =

NAME = SOCRATES

# Basic directories
PROJDIR = ./
CODE = $(PROJDIR)code/
OUT = $(PROJDIR)
INOUT42 = $(PROJDIR)orbit/42input/
OBJ = $(CODE)object/
INC = $(CODE)include/
SRC = $(CODE)source/
ORB42DIR = ./orbit/42/
OBJ42 = $(ORB42DIR)Object/
INC42 = $(ORB42DIR)Include/
SRC42 = $(ORB42DIR)Source/
KIT42DIR = $(ORB42DIR)Kit/
KIT42INC = $(KIT42DIR)Include/
KIT42SRC = $(KIT42DIR)Source/
SGP4DIR = ./orbit/sgp4/

ifeq ($(PLATFORM),__APPLE__)
	# Mac Macros
#	CINC = -I /usr/include
	CINC = -I /opt/homebrew/include
	LIBS = -framework System -lnetcdf
#	LFLAGS = -bind_at_load $(OPENMPFLAG)
	LFLAGS = -L/opt/homebrew/lib
	# ARCHFLAG = -arch i386
	# ARCHFLAG = -arch x86_64
	ARCHFLAG = -arch arm64
	EXENAME = $(NAME)
	CC = mpicc
endif

ifeq ($(PLATFORM),__linux__)
	# Linux Macros
	CINC =
	LIBS = -ldl -lm -lnetcdf
	LFLAGS = $(OPENMPFLAG)
	ARCHFLAG =
	EXENAME = $(NAME)
	CC = mpicc
endif

ifeq ($(PLATFORM),__MSYS__)
	CINC =
	LIBS =  -lws2_32 -lnetcdf
	LFLAGS = $(OPENMPFLAG)
	ARCHFLAG =
	EXENAME = $(NAME).exe
	CC = mpicc
endif

SOOPOBJ = $(OBJ)socrates.o $(OBJ)init.o $(OBJ)obs.o $(OBJ)geometry.o $(OBJ)ground.o $(OBJ)product.o $(OBJ)antenna.o $(OBJ)attitude.o $(OBJ)earthdata.o $(OBJ)interface.o $(OBJ)util.o $(OBJ)propagator.o

BUSOBJ = $(OBJ)comm.o

RETOBJ = $(OBJ)retrieval.o

DEVOBJ = $(OBJ)design.o $(OBJ)sai.o
	
42OBJ = $(OBJ42)42main.o $(OBJ42)42exec.o $(OBJ42)42actuators.o $(OBJ42)42cmd.o \
$(OBJ42)42dynamics.o $(OBJ42)42environs.o $(OBJ42)42ephem.o $(OBJ42)42fsw.o \
$(OBJ42)42init.o $(OBJ42)42jitter.o $(OBJ42)42joints.o \
$(OBJ42)42perturb.o $(OBJ42)42report.o $(OBJ42)42sensors.o $(OBJ42)AcApp.o 

KIT42OBJ = $(OBJ42)dcmkit.o $(OBJ42)envkit.o $(OBJ42)fswkit.o $(OBJ42)geomkit.o \
$(OBJ42)iokit.o $(OBJ42)mathkit.o $(OBJ42)nrlmsise00kit.o $(OBJ42)msis86kit.o \
$(OBJ42)orbkit.o $(OBJ42)radbeltkit.o $(OBJ42)sigkit.o $(OBJ42)sphkit.o $(OBJ42)timekit.o

# Vendored public-domain Vallado SGP4 (see orbit/sgp4/PROVENANCE.md)
SGP4OBJ = $(SGP4DIR)SGP4.o $(SGP4DIR)TLE.o

CFLAGS = -Wall -Wshadow -Wno-deprecated -g $(GLINC) $(CINC) -I $(INC) -I $(SRC) -I $(INC42) -I $(KIT42INC) -I $(KIT42SRC) -O0 $(ARCHFLAG) $(TIMEFLAG)
#-fno-stack-protector 

##########################  Rules to link SoOp  #############################

SOCRATES : $(SOOPOBJ) $(BUSOBJ) $(DEVOBJ) $(RETOBJ) $(42OBJ) $(KIT42OBJ) $(SGP4OBJ)
	   $(CC) $(LFLAGS) -o $(EXENAME) $(SOOPOBJ) $(DEVOBJ) $(RETOBJ) $(BUSOBJ) $(42OBJ) $(KIT42OBJ) $(SGP4OBJ) $(LIBS)

# Standalone checker of the SAI observation model (doc/SAI_model_validation.md)
sai_check : $(SRC)sai_check.c $(SRC)sai.c $(INC)sai.h
	$(CC) $(CFLAGS) -Wextra $(LFLAGS) -o sai_check $(SRC)sai_check.c $(SRC)sai.c $(LIBS)

####################  Rules to compile objects  ###########################

$(OBJ)socrates.o             : $(SRC)socrates.c $(INC)socrates.h $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC)socrates.c -o $(OBJ)socrates.o

$(OBJ)init.o             : $(SRC)init.c $(INC)socrates.h $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC)init.c -o $(OBJ)init.o

$(OBJ)obs.o              : $(SRC)obs.c $(INC)socrates.h $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC)obs.c -o $(OBJ)obs.o

$(OBJ)geometry.o         : $(SRC)geometry.c $(INC)socrates.h $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC)geometry.c -o $(OBJ)geometry.o

$(OBJ)ground.o           : $(SRC)ground.c $(INC)socrates.h $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC)ground.c -o $(OBJ)ground.o

$(OBJ)product.o          : $(SRC)product.c $(INC)socrates.h $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC)product.c -o $(OBJ)product.o

$(OBJ)antenna.o          : $(SRC)antenna.c $(INC)socrates.h $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC)antenna.c -o $(OBJ)antenna.o

$(OBJ)attitude.o         : $(SRC)attitude.c $(INC)socrates.h $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC)attitude.c -o $(OBJ)attitude.o

$(OBJ)interface.o        : $(SRC)interface.c $(INC)socrates.h $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC)interface.c -o $(OBJ)interface.o

$(OBJ)earthdata.o        : $(SRC)earthdata.c $(INC)socrates.h $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC)earthdata.c -o $(OBJ)earthdata.o

$(OBJ)util.o             : $(SRC)util.c $(INC)util.h $(INC)socrates.h $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC)util.c -o $(OBJ)util.o

$(OBJ)retrieval.o        : $(SRC)retrieval.c $(INC)socrates.h $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC)retrieval.c -o $(OBJ)retrieval.o

$(OBJ)comm.o             : $(SRC)comm.c $(INC)socrates.h $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC)comm.c -o $(OBJ)comm.o

$(OBJ)design.o           : $(SRC)design.c $(INC)socrates.h $(INC)sai.h $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC)design.c -o $(OBJ)design.o

$(OBJ)sai.o              : $(SRC)sai.c $(INC)sai.h
	$(CC) $(CFLAGS) -Wextra -c $(SRC)sai.c -o $(OBJ)sai.o

$(OBJ42)42main.o          : $(SRC42)42main.c 
	$(CC) $(CFLAGS) -c $(SRC42)42main.c -o $(OBJ42)42main.o  

$(OBJ42)42exec.o          : $(SRC42)42exec.c $(INC42)42.h $(INC)socrates.h
	$(CC) $(CFLAGS) -c $(SRC42)42exec.c -o $(OBJ42)42exec.o  

$(OBJ42)42actuators.o : $(SRC42)42actuators.c $(INC42)42.h $(INC42)Ac.h $(INC42)AcTypes.h
	$(CC) $(CFLAGS) -c $(SRC42)42actuators.c -o $(OBJ42)42actuators.o  

$(OBJ42)42cmd.o : $(SRC42)42cmd.c $(INC42)42.h $(INC42)Ac.h $(INC42)AcTypes.h
	$(CC) $(CFLAGS) -c $(SRC42)42cmd.c -o $(OBJ42)42cmd.o  

$(OBJ42)42dynamics.o     : $(SRC42)42dynamics.c $(INC42)42.h 
	$(CC) $(CFLAGS) -c $(SRC42)42dynamics.c -o $(OBJ42)42dynamics.o  

$(OBJ42)42environs.o  : $(SRC42)42environs.c $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC42)42environs.c -o $(OBJ42)42environs.o  

$(OBJ42)42ephem.o     : $(SRC42)42ephem.c $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC42)42ephem.c -o $(OBJ42)42ephem.o  

$(OBJ42)42fsw.o       : $(SRC42)42fsw.c $(INC42)Ac.h $(INC42)AcTypes.h 
	$(CC) $(CFLAGS) -c $(SRC42)42fsw.c -o $(OBJ42)42fsw.o  

$(OBJ42)42GlutGui.o        : $(SRC42)42GlutGui.c $(INC42)42.h $(INC42)42GlutGui.h 
	$(CC) $(CFLAGS) -c $(SRC42)42GlutGui.c -o $(OBJ42)42GlutGui.o  

$(OBJ42)42init.o      : $(SRC42)42init.c $(INC42)42.h  
	$(CC) $(CFLAGS) -c $(SRC42)42init.c -o $(OBJ42)42init.o  

$(OBJ42)42jitter.o    : $(SRC42)42jitter.c $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC42)42jitter.c -o $(OBJ42)42jitter.o

$(OBJ42)42joints.o    : $(SRC42)42joints.c $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC42)42joints.c -o $(OBJ42)42joints.o

$(OBJ42)42perturb.o   : $(SRC42)42perturb.c $(INC42)42.h 
	$(CC) $(CFLAGS) -c $(SRC42)42perturb.c -o $(OBJ42)42perturb.o  

$(OBJ42)42report.o    : $(SRC42)42report.c $(INC42)42.h 
	$(CC) $(CFLAGS) -c $(SRC42)42report.c -o $(OBJ42)42report.o  

$(OBJ42)42sensors.o   : $(SRC42)42sensors.c $(INC42)42.h $(INC42)Ac.h $(INC42)AcTypes.h
	$(CC) $(CFLAGS) -c $(SRC42)42sensors.c -o $(OBJ42)42sensors.o  

$(OBJ42)AcApp.o          : $(SRC42)AcApp.c $(INC42)Ac.h $(INC42)AcTypes.h
	$(CC) $(CFLAGS) -c $(SRC42)AcApp.c -o $(OBJ42)AcApp.o

$(OBJ42)dcmkit.o      : $(KIT42SRC)dcmkit.c
	$(CC) $(CFLAGS) -c $(KIT42SRC)dcmkit.c -o $(OBJ42)dcmkit.o  

$(OBJ42)envkit.o      : $(KIT42SRC)envkit.c
	$(CC) $(CFLAGS) -c $(KIT42SRC)envkit.c -o $(OBJ42)envkit.o  

$(OBJ42)fswkit.o      : $(KIT42SRC)fswkit.c 
	$(CC) $(CFLAGS) -c $(KIT42SRC)fswkit.c -o $(OBJ42)fswkit.o  

$(OBJ42)glkit.o      : $(KIT42SRC)glkit.c $(KIT42INC)glkit.h
	$(CC) $(CFLAGS) -c $(KIT42SRC)glkit.c -o $(OBJ42)glkit.o  

$(OBJ42)geomkit.o      : $(KIT42SRC)geomkit.c $(KIT42INC)geomkit.h 
	$(CC) $(CFLAGS) -c $(KIT42SRC)geomkit.c -o $(OBJ42)geomkit.o  

$(OBJ42)gmseckit.o      : $(KIT42SRC)gmseckit.c $(KIT42INC)gmseckit.h 
	$(CC) $(CFLAGS) -c $(KIT42SRC)gmseckit.c -o $(OBJ42)gmseckit.o  

$(OBJ42)iokit.o      : $(KIT42SRC)iokit.c 
	$(CC) $(CFLAGS) -c $(KIT42SRC)iokit.c -o $(OBJ42)iokit.o  

$(OBJ42)mathkit.o     : $(KIT42SRC)mathkit.c
	$(CC) $(CFLAGS) -c $(KIT42SRC)mathkit.c -o $(OBJ42)mathkit.o  

$(OBJ42)nrlmsise00kit.o   : $(KIT42SRC)nrlmsise00kit.c  
	$(CC) $(CFLAGS) -c $(KIT42SRC)nrlmsise00kit.c -o $(OBJ42)nrlmsise00kit.o  

$(OBJ42)msis86kit.o   : $(KIT42SRC)msis86kit.c $(KIT42INC)msis86kit.h 
	$(CC) $(CFLAGS) -c $(KIT42SRC)msis86kit.c -o $(OBJ42)msis86kit.o  

$(OBJ42)orbkit.o      : $(KIT42SRC)orbkit.c
	$(CC) $(CFLAGS) -c $(KIT42SRC)orbkit.c -o $(OBJ42)orbkit.o  

$(OBJ42)radbeltkit.o      : $(KIT42SRC)radbeltkit.c
	$(CC) $(CFLAGS) -c $(KIT42SRC)radbeltkit.c -o $(OBJ42)radbeltkit.o  

$(OBJ42)sigkit.o      : $(KIT42SRC)sigkit.c
	$(CC) $(CFLAGS) -c $(KIT42SRC)sigkit.c -o $(OBJ42)sigkit.o  

$(OBJ42)sphkit.o      : $(KIT42SRC)sphkit.c
	$(CC) $(CFLAGS) -c $(KIT42SRC)sphkit.c -o $(OBJ42)sphkit.o  

$(OBJ42)timekit.o     : $(KIT42SRC)timekit.c 
	$(CC) $(CFLAGS) -c $(KIT42SRC)timekit.c -o $(OBJ42)timekit.o  
 
$(OBJ)propagator.o       : $(SRC)propagator.c $(INC)propagator.h $(INC42)42.h
	$(CC) $(CFLAGS) -c $(SRC)propagator.c -o $(OBJ)propagator.o

###################  Rules to compile vendored SGP4  ######################

$(SGP4DIR)SGP4.o          : $(SGP4DIR)SGP4.c $(SGP4DIR)SGP4.h
	$(CC) $(CFLAGS) -I $(SGP4DIR) -c $(SGP4DIR)SGP4.c -o $(SGP4DIR)SGP4.o

$(SGP4DIR)TLE.o           : $(SGP4DIR)TLE.c $(SGP4DIR)TLE.h $(SGP4DIR)SGP4.h
	$(CC) $(CFLAGS) -I $(SGP4DIR) -c $(SGP4DIR)TLE.c -o $(SGP4DIR)TLE.o

########################  Miscellaneous Rules  ############################
clean :
ifeq ($(PLATFORM),_WIN32)
	del .\$(OBJ)\*.o .\$(OBJ42)\*.o .\$(EXENAME)
else ifeq ($(PLATFORM),_WIN64)
	del .\$(OBJ)\*.o .\$(OBJ42)\*.o .\$(EXENAME)
else
	rm -f $(OBJ)*.o $(OBJ42)*.o $(SGP4DIR)*.o sai_check
endif


