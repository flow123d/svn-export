# 
# Copyright (C) 2007 Technical University of Liberec.  All rights reserved.
#
# Please make a following refer to Flow123d on your project site if you use the program for any purpose,
# especially for academic research:
# Flow123d, Research Centre: Advanced Remedial Technologies, Technical University of Liberec, Czech Republic
#
# This program is free software; you can redistribute it and/or modify it under the terms
# of the GNU General Public License version 3 as published by the Free Software Foundation.
# 
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with this program; if not,
# write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 021110-1307, USA.
#
# $Id$
# $Revision$
# $LastChangedBy$
# $LastChangedDate$
#
# This makefile just provide main rules for: build, documentation and testing
# Build itself takes place in ./src.
#

all:  build_all install

FLOW_BIN=flow123d
INTERPOLATE_BIN=interpolation
MPIEXEC_BIN=mpiexec

ifndef N_JOBS
  N_JOBS=2
endif  

# install all binaries form build tree to './bin' dir
install: 
	if [ -e  "build/$(INTERPOLATE_BIN)" ]; then rm -f bin/$(INTERPOLATE_BIN); cp "build/$(INTERPOLATE_BIN)" bin; fi
	if [ -e  "build/$(FLOW_BIN)" ]; then rm -f bin/$(FLOW_BIN); cp "build/$(FLOW_BIN)" bin; fi
	if [ -e  "build/$(MPIEXEC_BIN)" ]; then rm -f bin/$(MPIEXEC_BIN); cp "build/$(MPIEXEC_BIN)" bin; chmod a+x bin/mpiexec; fi


# run first cmake
build/CMakeCache.txt:
	if [ ! -d build ]; then mkdir build; fi
	cd build; cmake ..

# This target builds links in directory test_units and its subdirectories 
# to generated makefiles in the build directory. 
# This way we can run tests from the source tree and do not have problems with deleted
# current directory in shell if we are forced to use make clean-all.
create_unit_test_links:
	for f in  `find test_units/ -name CMakeLists.txt`; do ln -sf "$${PWD}/build/$${f%/*}/Makefile" "$${f%/*}/makefile";done

# This target only configure the build process.
# Useful for building unit tests without actually build whole program.
cmake: build/CMakeCache.txt  create_unit_test_links



build_all: build_flow123d

flow123d:  build_flow123d  install


# timing of parallel builds (on Core 2 Duo, 4 GB ram)
# N JOBS	O3	g,O0	
# 1 		51s	46s
# 2 		30s	26s
# 4 		31s	27s
# 8 		30s
build_flow123d: cmake
	make -j $(N_JOBS) -C build flow123d

	
interpolation: build_interpolation install
	
build_interpolation: 
	make -j $(N_JOBS) -C build interpolation


	
# Remove all generated files
clean: cmake
	make -C build clean

# try to remove all
clean-all: 
	rm -f bin/${FLOW_BIN}
	rm -f bin/${MPIEXEC_BIN}
	rm -f bin/${INTERPOLATE_BIN}
	rm -rf build
	for f in  `find test_units/ -name makefile`; do rm -f "$${f}";done
	make -C third_party clean

# remove everything that is not under version control 
# BE EXTREMELY CAREFUL using this
clean_all_svn:
	bin/svnclean.sh

# Make all tests	
testall:
	make -C tests testall

# Make only certain test (eg: make 01.tst will make first test)
%.tst :
	make -C tests $*.tst

# Create doxygen documentation
online-doc:
	make -C doc/doxy doc


# create input file reference for manual
DOC_DIR=doc/reference_manual

# creates the file that defins additional information 
# for input description generated by flow123d to Latex format
# this file contains one replace rule per line in format
# \add_doc{<tag>}<replace>
# 
# this replace file is applied to input_reference.tex produced by flow123d
#

# call flow123d and make raw input_reference file
$(DOC_DIR)/input_reference_raw.tex: flow123d	 	
	bin/flow123d --latex_doc | grep -v "DBG" | \
	sed 's/->/$$\\rightarrow$$/g' > $(DOC_DIR)/input_reference_raw.tex

# make empty file with replace rules if we do not have one
$(DOC_DIR)/add_to_ref_doc.txt: 
	touch $(DOC_DIR)/add_to_ref_doc.txt
	
# update file with replace rules according to acctual patterns appearing in input_refecence		
update_add_doc: $(DOC_DIR)/input_reference_raw.tex $(DOC_DIR)/add_to_ref_doc.txt
	cat $(DOC_DIR)/input_reference_raw.tex \
	| grep 'AddDoc' |sed 's/^.*\(\\AddDoc{[^}]*}\).*/\1/' \
	> $(DOC_DIR)/add_to_ref_doc.list
	$(DOC_DIR)/add_doc_replace.sh $(DOC_DIR)/add_to_ref_doc.txt $(DOC_DIR)/add_to_ref_doc.list	
	
# make final input_reference.tex, applying replace rules
$(DOC_DIR)/input_reference.tex: $(DOC_DIR)/input_reference_raw.tex update_add_doc
	$(DOC_DIR)/add_doc_replace.sh $(DOC_DIR)/add_to_ref_doc.txt $(DOC_DIR)/input_reference_raw.tex $(DOC_DIR)/input_reference.tex	
	
gen_doc: $(DOC_DIR)/input_reference.tex
		

clean_tests:
	make -C tests clean


install_dir=installed
linux_package: all clean_tests
	# copy bin
	rm -rf $(install_dir)
	mkdir -p $(install_dir)/bin
	mpiexec=`cat bin/mpiexec |grep mpiexec |sed 's/ *".*$$//'|sed 's/"//g'`;\
	cp "$${mpiexec}" $(install_dir)/bin/mpiexec
	cp -r bin/flow123d bin/flow123d.sh bin/ndiff $(install_dir)/bin
	cp -r bin/paraview $(install_dir)/bin
	cp -r bin/tests $(install_dir)/bin
	# copy doc
	mkdir $(install_dir)/doc
	cp -r doc/articles doc/reference_manual/flow123d_doc.pdf doc/petsc_options_help $(install_dir)/doc
	# copy tests
	cp -r tests $(install_dir)
	# remove .svn
	find $(install_dir) -name ".svn" -exec rm -rf "{}" \;

CYGWIN_DLLS=cygblas-0.dll cyggcc_s-1.dll cyggfortran-3.dll cyglapack-0.dll cygstdc++-6.dll cygwin1.dll
win_package: linux_package
	for f in $(CYGWIN_DLLS);do cp -f "/bin/$${f}" $(install_dir)/bin; done
linux_pack:
	cd $(install_dir); tar -cvzf ../flow_build.tar.gz .

	
