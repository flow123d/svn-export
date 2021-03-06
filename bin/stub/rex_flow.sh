#!/bin/bash
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
# $Id: hydra_make_pbs.sh 834 2011-01-02 15:54:35Z michal.nekvasil $
# $Revision: 834 $
# $LastChangedBy: michal.nekvasil $
# $LastChangedDate: 2011-01-02 16:54:35 +0100 (ne, 02 I 2011) $
#

#NPROC is defined in run_flow.sh and its number of procs used to compute
#MPI_RUN is defined in run_flow.sh and its relative path to bin/mpiexec
#EXECUTABLE is defined in run_flow.sh and its relative path to bin/flow123d (.exe)
#FLOW_PARAMS is defined in run_flow.sh
# INI is name of inifile and its defined in run_flow.sh
# SOURCE_DIR is path to test dir and is defined in run_flow.sh

cd "$SOURCE_DIR"

echo "
#PBS -S /bin/bash
#PBS -j oe
#PBS -N flow
#PBS -m bae
#PBS -l walltime=100000
#PBS -l select=1:ncpus=$NPROC:host=rex
#PBS -l place=free:shared


. /opt/intel/Compiler/11.1/046/bin/iccvars.sh ia64 
. /usr/share/modules/init/bash
module add mpt


export KMP_MONITOR_STACKSIZE=64K


#NPROC is defined in run_flow.sh and its number of procs used to compute
#MPI_RUN is defined in run_flow.sh and its relative path to bin/mpiexec
#EXECUTABLE is defined in run_flow.sh and its relative path to bin/flow123d (.exe)
#FLOW_PARAMS is defined in run_flow.sh
# INI is name of inifile and its defined in run_flow.sh
# SOURCE_DIR is path to test dir and is defined in run_flow.sh

cd "$SOURCE_DIR"
 

$MPI_RUN -np $NPROC dplace $EXECUTABLE -S $INI $FLOW_PARAMS 2>err 1>out
rm lock" >rex_flow.qsub

if test -e ./lock 
then
  echo "Error: the working directory is locked by onother PBS job!"
  exit 1
else
  touch lock
fi
chmod u+x rex_flow.qsub
qsub rex_flow.qsub