# Makefile for unix systems
# this requires GNU make

APPNAME=cg1application

# Compiler flags
# enable all warnings in general, disable some
# (we initially provide a rough skeleton of the framework,
# so some unused variables might be present, which should not be
# considered as a problem)
WARNFLAGS= -Wall -Wextra -pedantic \
	   -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function \
	   -Wno-unused-parameter

ifeq ($(RELEASE), 1)
CFLAGS =   $(WARNFLAGS) -ansi -ffast-math -s -O4 -DNDEBUG
CXXFLAGS = $(WARNFLAGS) -ansi -ffast-math -s -O4 -DNDEBUG
else
CFLAGS =   $(WARNFLAGS) -ansi -g
CXXFLAGS = $(WARNFLAGS) -ansi -g
endif

# OpenGL Libraries 
GL_LIBS = -lGL -lGLU -lglut
GL_LIBDIR = -L/usr/X11/lib
LINK_GL = $(GL_LIBDIR) $(GL_LIBS)

# all needed libraries
LINK = $(LINK_GL) -lm -lrt

# Files

CFILES=$(wildcard *.c)
CPPFILES=$(wildcard *.cpp)
INCFILES=$(wildcard *.h)	
SRCFILES = $(CFILES) $(CPPFILES)
PRJFILES = Makefile $(wildcard *.vcxproj) $(wildcard *.sln)
ALLFILES = $(SRCFILES) $(INCFILES) $(PRJFILES)
OBJECTS = $(patsubst %.cpp,%.o,$(CPPFILES)) $(patsubst %.c,%.o,$(CFILES))
	   
# build rules
.PHONY: all
all:	$(APPNAME)

# build and start with "make run"
.PHONY: run
run:	all
	__GL_SYNC_TO_VBLANK=1 ./$(APPNAME)

# automatic dependency generation
# create $(DEPDIR) (and an empty file dir)
# create a .d file for every .c source file which contains
# 		   all dependencies for that file
.PHONY: depend
depend:	$(DEPDIR)/dependencies
DEPDIR   = ./dep
DEPFILES = $(patsubst %.c,$(DEPDIR)/%.d,$(CFILES)) $(patsubst %.cpp,$(DEPDIR)/%.d,$(CPPFILES))
$(DEPDIR)/dependencies: $(DEPDIR)/dir $(DEPFILES)
	@cat $(DEPFILES) > $(DEPDIR)/dependencies
$(DEPDIR)/dir:
	@mkdir -p $(DEPDIR)
	@touch $(DEPDIR)/dir
$(DEPDIR)/%.d: %.c $(DEPDIR)/dir
	@echo rebuilding dependencies for $*
	@set -e; $(CC) -M $(CPPFLAGS) $<	\
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' \
	> $@; [ -s $@ ] || rm -f $@
$(DEPDIR)/%.d: %.cpp $(DEPDIR)/dir
	@echo rebuilding dependencies for $*
	@set -e; $(CXX) -M $(CPPFLAGS) $<	\
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' \
	> $@; [ -s $@ ] || rm -f $@
-include $(DEPDIR)/dependencies

# rule to build application
$(APPNAME): $(OBJECTS) $(DEPDIR)/dependencies
	$(CXX) $(CFLAGS) $(LINK) $(LDFLAGS) $(OBJECTS) -o$(APPNAME)

# remove all unneeded files
.PHONY: clean
clean:
	@echo removing binary: $(APPNAME)
	@rm -f $(APPNAME)
	@echo removing object files: $(OBJECTS)
	@rm -f $(OBJECTS)
	@echo removing dependency files
	@rm -rf $(DEPDIR)
	@echo removing tags
	@rm -f tags

# update the tags file
.PHONY: TAGS
TAGS:	tags

tags:	$(SRCFILES) $(INCFILES) 
	ctags $(SRCFILES) $(INCFILES)

# look for 'TODO' in all relevant files
.PHONY: todo
todo:
	-egrep -in "TODO" $(SRCFILES) $(INCFILES)

