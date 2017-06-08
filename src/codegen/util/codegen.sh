#!/bin/sh

#  codegen.sh
#
#  Created by Rodrigo Varas on 12/13/12.
#  Copyright (c) 2012 Rodrigo Varas. All rights reserved.


outputdir=
productname=
codegen_bin=
schema_project=
other_attributes=
config_file=codegen.json
javapackagename=
genjava=false

while getopts ":o:p:s:g:a:j:b:" opt; do
    case "${opt}" in
    p)
        echo "product name:$OPTARG" >&2
        productname=$OPTARG
        ;;
    o)
        echo "output dir:$OPTARG" >&2
        outputdir=$OPTARG
        ;;
    b)
        echo "code gen bin:$OPTARG" >&2
        codegen_bin=$OPTARG
        ;;
    s)
        echo "schema project:$OPTARG" >&2
        schema_project=$OPTARG
        ;;
    g)
        echo "config_file:$OPTARG" >&2
        config_file=$OPTARG
        ;;
    a)
        echo "other attributes:$OPTARG" >&2
        other_attributes=$OPTARG
        ;;
    j)
        echo "java package name:$OPTARG" >&2
        javapackagename=$OPTARG
        ;;
    \?)
        echo "Invalid option: -$OPTARG" >&2
    ;;
    esac
done

if [ -z $productname ]; then
echo "missing product name option"
exit 1
fi
if [ -z $outputdir ]; then
echo "missing output dir option"
exit 1
fi
if [ -z $codegen_bin ]; then
echo "missing code gen dir option"
exit 1
fi
if [ -z $schema_project ]; then
echo "missing schema project option"
exit 1
fi

echo "productname=${productname}"
echo "outputdir=${outputdir}"
echo "codegen_bin=${codegen_bin}"
echo "schema project=${schema_project}"
echo "config file=${config_file}"
echo "java package name=${javapackagename}"


if [ -d "../gpengine" ]
then
	grammarsdir=../gpengine/grammars/
else
	grammarsdir=../grammars/
fi
echo "grammars dir=${grammarsdir}"

pmodschemaparser=$codegen_bin/pmodschemaparser
pmodcodegen=$codegen_bin/pmodcodegen

# make tools executable
chmod +x $pmodschemaparser
chmod +x $pmodcodegen

schema_json=$outputdir/${productname}.schema.json

mkdir -p $outputdir

if ! [ -z $javapackagename ]; then
genjava=true
javasourcedir=$outputdir/java/$productname/"${javapackagename//.//}"
echo $javasourcedir
mkdir -p $javasourcedir
fi

echo "generating Unified schema json for:${productname}"
$pmodschemaparser -pschema=${schema_project} -pgrammars=$grammarsdir -poutput=$schema_json -pcheckdependencies=true -pgenerated_dir=$outputdir -pschema_attributes=$other_attributes

OUT=$?

if [ $OUT -eq 0 ]; then

	$pmodcodegen -pconfig_file=$config_file -pmodelschema=$schema_json -poutputdir=$outputdir/ -pproductname=${productname} -pgenjava.run=$genjava -pjavapackagename=$javapackagename

elif [ $OUT -eq 1 ]; then  
   echo "schema is up to date.."
elif [ $OUT -eq 255 ]; then
   echo "code generation error"
   exit 1
fi

