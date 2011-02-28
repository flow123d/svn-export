# Program "prog"
#

NAME = prog

CCC = g++
CFLAGS = -Wall
CLAPACK = ../CLAPACK-3.2.1
INCCLAPACK = -I$(CLAPACK)/INCLUDE
F2CDIR  = $(CLAPACK)/F2CLIBS
LIBCLAPACK = $(CLAPACK)/lapack_LINUX.a \
             $(CLAPACK)/blas_LINUX.a \
             $(CLAPACK)/tmglib_LINUX.a \
             $(F2CDIR)/libf2c.a -lm

FADBAD = ../FADBAD++
INCFADBAD = $(FADBAD)/badiff.h $(FADBAD)/fadbad.h

OBJS = main.o bandmatrixsolve.o functorvaluebase.o functorvalue.o functordiffbase.o \
	functordiff.o polynomial.o interpolantbase.o interpolant.o myfunction.o

$(NAME): $(OBJS)
	$(CCC) $(CFLAGS) $^ $(LIBCLAPACK) -o $(NAME)

myfunction.o: myfunction.cpp
	$(CCC) $(CFLAGS) $< myfunction.h -c 

#lagrange.o: lagrange.cpp lagrange.h functorvaluebase.h interpolant.h bandmatrixsolve.h
#	$(CCC) $(CFLAGS) -c $<

interpolant.o: interpolant.cpp 
	$(CCC) $(CFLAGS) $< interpolant.h interpolantbase.h polynomial.h -c

interpolantbase.o: interpolantbase.cpp
	$(CCC) $(CFLAGS) $< interpolantbase.h polynomial.h functordiffbase.h -c

polynomial.o: polynomial.cpp 
	$(CCC) $(CFLAGS) $< polynomial.h functordiffbase.h -c

functordiff.o: functordiff.cpp 
	$(CCC) $(CFLAGS) $< functordiff.h functordiffbase.h $(INCFADBAD) -c

functordiffbase.o: functordiffbase.cpp 
	$(CCC) $(CFLAGS) $< functordiffbase.h functorvaluebase.h -c

functorvalue.o: functorvalue.cpp
	$(CCC) $(CFLAGS) $< functorvalue.h functorvaluebase.h -c

functorvaluebase.o: functorvaluebase.cpp
	$(CCC) $(CFLAGS) $< functorvaluebase.h -c 

bandmatrixsolve.o: bandmatrixsolve.cpp
	$(CCC) $(CFLAGS) $< bandmatrixsolve.h $(INCCLAPACK) -c 

main.o: main.cpp 
	$(CCC) $(CFLAGS) $< stdafx.h -c 

stdafx.h: functorvaluebase.h functordiffbase.h functorvalue.h functordiff.h \
	  myfunction.h interpolant.h lagrange.h polynomial.h interpolantbase.h

clean: 
	rm *.o *.gch *~ $(NAME)