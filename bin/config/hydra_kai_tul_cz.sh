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
# $Id$
# $Revision$
# $LastChangedBy$
# $LastChangedDate: 2011-01-02 16:54:35 +0100 (ne, 02 I 2011) $
#

# NP is number of procs used to compute
# MPIEXEC is relative path to bin/mpiexec
# FLOW123D is relative path to bin/flow123d (.exe)
# FLOW_PARAMS is list of parameters of flow123d
# INI_FILE is name of .ini file


# Function that is used for running flow123d at hydra cluster
function run_flow()
{
	# Some important files
	export ERR_FILE="err.log"
	export OUT_FILE="out.log"

	rm -f /tmp/${USER}-hydra_flow.qsub
			
# Copy following text to the file /tmp/firstname.surname-hydra_flow.qsub
# ======================================================================
cat << xxEOFxx > /tmp/${USER}-hydra_flow.qsub
#!/bin/bash
#
#$ -cwd
#$ -j y
#$ -S /bin/bash
#
	
# Disable system rsh / ssh only
export OMPI_MCA_plm_rsh_disable_qrsh=1
	
# Execute Flow123d using mpiexec
"$MPIEXEC" -np $NP "$FLOW123D" $FLOW_OPT "$INI_FILE" $FLOW_PARAMS 2>${ERR_FILE} 1>${OUT_FILE}
	
# End of hydra_flow.qsub
xxEOFxx
# ======================================================================

	if [ -f /tmp/${USER}-hydra_flow.qsub ]
	then    
		# Add new PBS job to the queue
		echo "qsub -pe orte $NP /tmp/${USER}-hydra_flow.qsub"
		qsub -pe orte $NP /tmp/${USER}-hydra_flow.qsub
		# Remove obsolete script
		rm /tmp/${USER}-hydra_flow.qsub
	else
		exit 1
	fi		
	
	exit 0
}