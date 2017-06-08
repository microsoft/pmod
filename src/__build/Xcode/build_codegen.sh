#!/bin/sh

#  build.sh

flavor=
clean=

while getopts ":t:c" opt; do
    case "${opt}" in
    c)
        clean=clean
        echo "clean build!" >&2
        ;;
    t)
        echo "flavor selected: $OPTARG" >&2
        flavor=$OPTARG
        ;;
    \?)
        echo "Invalid option: -$OPTARG" >&2
    ;;
    esac
done
echo "clean=${clean}"
echo "flavor=${flavor}"

projects=( "codegen_shared" "pmodschemaparser" "pmodcodegen" "pmodcodegentask" )

for project in "${projects[@]}"
do
xcodeproj="codegen/"$project"/"$project".xcodeproj"

if [ -z $flavor ] || [ "$flavor" == "Debug" ]; then
echo "building debug flavor for project:" $xcodeproj
xcodebuild -project $xcodeproj  -configuration Debug $clean build
fi

if [ -z $flavor ] || [ "$flavor" == "Release" ]; then
echo "building release flavor for project:" $xcodeproj
xcodebuild -project $xcodeproj -configuration Release $clean build
fi

done


