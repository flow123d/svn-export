#!/bin/bash
# 
# Copyright (C) 2007 Technical University of Liberec.  All rights reserved.
#
# Please make a following refer to Flow123d on your project site if you use the program for any purpose,
# especially for academic research:
# Flow123d, Research Center: Advanced Remedial Technologies, Technical University of Liberec, Czech Republic
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
# $Id: run_test.sh 1270 2011-08-09 11:37:24Z jiri.hnidek $
# $Revision: 1270 $
# $LastChangedBy: jiri.hnidek $
# $LastChangedDate: 2011-08-09 13:37:24 +0200 (Út, 09 srp 2011) $
#
# Author(s): Jiri Hnidek <jiri.hnidek@tul.cz>
#

# Relative path to Flow123d binary from the directory,
# where this script is placed
FLOW123D="./flow123d"
# Relative path to Flow123d binary from current/working directory
FLOW123D="${0%/*}/${FLOW123D}"

# Print help to this script
function print_help {
	echo "SYNTAX: flow123d.sh [OPTIONS] -i INI_FILE [ -f FLOW_PARAMS]"
	echo ""
	echo "OPTIONS:"
	echo "    -h              Print this help"
	echo "    -t TIMEOUT      Flow123d can be executed only TIMEOU seconds"
	echo "    -m MEM          Flow123d can use only MEM bytes"
	echo "    -n NICE         Run Flow123d with changed (lower) priority"
	echo "    -o OUT_FILE     Stdout and Stderr will be redirected to OUT_FILE"
	echo "    -i INI_FILE     Flow123d will load configuration from INI_FILE"
	echo "    -f FLOW_PARAMS  Flow123d will use it's specific params"
	echo ""
}

# Make sure that INI_FILE is not set
unset INI_FILE

# Parse arguments with bash builtin command getopts
while getopts ":ht:m:n:o:i:f:" opt
do
	case ${opt} in
	h)
		print_help
		exit 0
		;;
	t)
		TIMEOUT="${OPTARG}"
		;;
	m)
		MEM="${OPTARG}"
		;;
	n)
		NICE="${OPTARG}"
		;;
	o)
		OUT_FILE="${OPTARG}"
		;;
	i)
		# -i has to be followed by ini file; e.g.: "flow.ini"
		INI_FILE="${OPTARG}"
		;;
	f)
		# Additional flow parametres; e.g.: "-i input -o output"
		FLOW_PARAMS="${OPTARG}"
		;;
	\?)
		echo ""
		echo "Error: Invalid option: -$OPTARG"
		echo ""
		print_help
		exit 1
		;;
	esac
done

# Check if Flow123d exists and it is executable file
if ! [ -x "${FLOW123D}" ]
then
	echo "Error: can't execute ${FLOW123D}"
	exit 1
fi

# Was any ini file set?
if [ ! -n "${INI_FILE}" ]
then
	echo ""
	echo "Error: No ini file"
	echo ""
	print_help
	exit 1
else

	# Was memory limit set?
	if [ -n "${MEM}" ]
	then
		# Set up memory limits that prevent too allocate too much memory.
		# The limit of virtual memory is 200MB (memory, that could be allocated)
		MEM_LIMIT=`expr ${MEM} \* 1000`
		ulimit -S -v 200000
	fi
	
	# Was nice set?
	if [ ! -n "${NICE}" ]
	then
		NICE=0
	fi
	
	# Was output file set?
	if [ -n "${OUT_FILE}" ]
	then
		FLOW123D_OUTPUT="${OUT_FILE}"
		# Clear output file.
		echo "" > "${FLOW123D_OUTPUT}"
	else
		FLOW_OUTPUT=&1
	fi
	
	echo "timeout: ${TIMEOUT}"
	echo "mem_limit: ${MEM}"
	echo "nice: ${NICE}"
	echo "out_file: ${OUT_FILE}"
	echo "ini_file: ${INI_FILE}"
	echo "flow_params: ${FLOW_PARAMS}"
	
	# WIP exit :-)
	exit 0
		
	# Was timeout set?
	if [ -n "${TIMEOT}" ]
	then
		# Flow123d runs with changed priority (19 is the lowest priority)
		nice --adjustment="${NICE}" "${FLOW123D}" -S "${INI_FILE}" ${FLOW_PARAMS} > "${FLOW123D_OUTPUT}" 2>&1 &
		FLOW123D_PID=$!
		
		echo -n "Running flow123d [proc:${NP}] ${INI_FILE} ."
		IS_RUNNING=1

		# Wait max TIMEOUT seconds, then kill flow123d processes
		while [ ${TIMER} -lt ${TIMEOUT} ]
		do
			TIMER=`expr ${TIMER} + 1`
			echo -n "."
			#ps -o "%P %p"
			sleep 1

			# Is mpiexec and flow123d still running?
			ps | gawk '{ print $1 }' | grep -q "${PARENT_MPIEXEC_PID}"
			if [ $? -ne 0 ]
			then
				# set up, that flow123d was finished in time
				IS_RUNNING="0"
				break 1
			fi
		done
		
		# Was Flow123d finished during TIMEOUT or is it still running?
		if [ ${IS_RUNNING} -eq 1 ]
		then
			echo " [Failed:loop]"
			# Send SIGTERM to flow123d.
			kill -s SIGTERM ${FLOW123D_PID} #> /dev/null 2>&1
			EXIT_STATUS=2
			# No other test will be executed
			break 2
		else
			# Get exit status variable of flow123d
			wait ${FLOW123D_PID}
			FLOW123D_EXIT_STATUS=$?

			# Was Flow123d finished correctly?
			if [ ${FLOW123D_EXIT_STATUS} -eq 0 ]
			then
				echo " [Success:${TIMER}s]"
			fi
		fi
	else
		nice --adjustment="${NICE}" "${FLOW123D}" -S "${INI_FILE}" ${FLOW_PARAMS} > "${FLOW123D_OUTPUT}" 2>&1 &
	fi
	
fi

