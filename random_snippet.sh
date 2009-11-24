#!/bin/bash

############################################################
#
# This script selects a random member function from the
# Dino source code and prints it. It's quite an ugly hack,
# it relies on the Doxygen XML output to be formatted in a
# certain way. 
#
# Feel free to use and modify it for any purpose without 
# attribution.
#
#                    Lars Luthman <lars.luthman@gmail.com>
#
############################################################


FILES=(`ls -1 doxygen/xml/* | grep -E '((class)|(struct))Dino'`)
NFILES=${#FILES[@]}

for i in `seq 1 1000`; do
    XFILE=${FILES[RANDOM%NFILES]}
    LINES=(`grep -n 'kind="function"' $XFILE | sed 's/:.*//'`)
    NLINES=${#LINES[@]}
    if [[ $NLINES -ne 0 ]] ; then
	FLINE=${LINES[RANDOM%NLINES]}
	LLINE=`tail -n +$FLINE $XFILE | grep -m 1 "<location"`
	SFILE=`echo $LLINE | sed 's/.*\ bodyfile="//' | sed 's/".*//'`
	SSTART=`echo $LLINE | sed 's/.*\ bodystart="//' | sed 's/".*//'`
	SEND=`echo $LLINE | sed 's/.*\ bodyend="//' | sed 's/".*//'`
	if [[ -e "$SFILE" ]] ; then 
	    if [[ $SSTART -ge 0 ]] ; then
		if [[ $SEND -ge $SSTART ]] ; then
		    SLENGTH=$(($SEND - $SSTART + 1))
		    # Add an affinity for functions that aren't very short
		    if [[ $SLENGTH -ge $(($RANDOM%5+4)) ]] ; then
			echo '<div class="snippetlink">'`echo ${SFILE} | sed 's@/home/ll/src/git/@@'`:${SSTART}'</div>'
			echo -n '<div class="snippet context">'
			cat -n $SFILE | tail -n +$(($SSTART - 3)) | head -n 3 | sed 's/&/\&amp;/g' | sed 's/</\&lt;/g' | sed 's/>/\&gt;/g'
			echo -n '</div><div class="snippet">'
			cat -n $SFILE | tail -n +$SSTART | head -n $SLENGTH | sed 's/&/&amp;/g' | sed 's/</\&lt;/g' | sed 's/>/\&gt;/g'
			echo -n '</div><div class="snippet context">'
			cat -n $SFILE | tail -n +$(($SEND + 1)) | head -n 3 | sed 's/&/&amp;/g' | sed 's/</\&lt;/g' | sed 's/>/\&gt;/g'
			echo '</div>'
			exit 0
		    fi
		fi
	    fi
	fi
    fi
done

exit -1
    