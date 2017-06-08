#!/bin/sh

#  codegen_clean.sh
#
#  Created by Rodrigo Varas on 11/4/13.
#  Copyright (c) 2013 Rodrigo Varas. All rights reserved.

outputdir=
while getopts ":o:" opt; do
  case $opt in
    o)
      outputdir=$OPTARG
        ;;
    \?)
      echo "unknown option"
		exit 1
      ;;
  esac
done

if [ -z $outputdir ]; then
echo "missing output directory option"
exit 1
fi

if [ "$ACTION" = clean ]
then
echo "clean code generated files in dir:${outputdir}"
rm -rf $outputdir
fi
