# Program "prog"
#

NAME = prog

CCC = g++
CFLAGS = -Wall -g -O3
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
	functordiff.o polynomial.o interpolantbase.o interpolant.o myfunction.o \
	bcondition.o lagrange.o iinterpolation.o adaptive.o adaptivesimpson.o norm.o

$(NAME): $(OBJS)
	$(CCC) $(CFLAGS) $^ $(LIBCLAPACK) -o $(NAME)

myfunction.o: myfunction.cpp myfunction.h
	$(CCC) $(CFLAGS) $< -c  

iinterpolation.o: iinterpolation.cpp iinterpolation.h functorvaluebase.h interpolantbase.h \
		  massert.h
	$(CCC) $(CFLAGS) $< -c

norm.o: norm.cpp functorvaluebase.h interpolantbase.h
	$(CCC) $(CFLAGS) $< -c

adaptivesimpson.o: adaptivesimpson.cpp functorvaluebase.h interpolantbase.h
	$(CCC) $(CFLAGS) $< -c

adaptive.o: adaptive.cpp adaptive.h functorvaluebase.h interpolantbase.h interpolant.h \
	    iinterpolation.h massert.h
	$(CCC) $(CFLAGS) $< -c

lagrange.o: lagrange.cpp lagrange.h functorvaluebase.h interpolantbase.h interpolant.h \
	    iinterpolation.h bandmatrixsolve.h massert.h
	$(CCC) $(CFLAGS) $< -c

bcondition.o: bcondition.cpp bcondition.h
	$(CCC) $(CFLAGS) $< -c

interpolant.o: interpolant.cpp interpolant.h interpolantbase.h polynomial.h
	$(CCC) $(CFLAGS) $< -c

interpolantbase.o: interpolantbase.cpp interpolantbase.h polynomial.h functordiffbase.h
	$(CCC) $(CFLAGS) $< -c

polynomial.o: polynomial.cpp polynomial.h functordiffbase.h
	$(CCC) $(CFLAGS) $< -c

functordiff.o: functordiff.cpp functordiff.h functordiffbase.h $(INCFADBAD)
	$(CCC) $(CFLAGS) $< -c

functordiffbase.o: functordiffbase.cpp functordiffbase.h functorvaluebase.h
	$(CCC) $(CFLAGS) $< -c

functorvalue.o: functorvalue.cpp functorvalue.h functorvaluebase.h
	$(CCC) $(CFLAGS) $< -c

functorvaluebase.o: functorvaluebase.cpp functorvaluebase.h
	$(CCC) $(CFLAGS) $< -c 

bandmatrixsolve.o: bandmatrixsolve.cpp bandmatrixsolve.h
	$(CCC) $(CFLAGS) $< $(INCCLAPACK) -c 

main.o: main.cpp stdafx.h
	$(CCC) $(CFLAGS) $< -c 

stdafx.h: functorvaluebase.h functordiffbase.h functorvalue.h functordiff.h \
	  myfunction.h interpolant.h lagrange.h polynomial.h interpolantbase.h

clean: 
	rm *.o *.gch *~ $(NAME)