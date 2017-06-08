#!/bin/sh

#  build.sh

flavor=
clean=
build_target=
iphonesimulator_sdk=iphonesimulator10.2

while getopts ":t:c:b:s" opt; do
    case "${opt}" in
    c)
        clean=clean
        echo "clean build!" >&2
        ;;
    t)
        echo "flavor selected: $OPTARG" >&2
        flavor=$OPTARG
        ;;
    b)
        echo "build target selected: $OPTARG" >&2
        build_target=$OPTARG
        ;;
    s)
        echo "sdk simulator selected: $OPTARG" >&2
        iphonesimulator_sdk=$OPTARG
        ;;
    \?)
        echo "Invalid option: -$OPTARG" >&2
    ;;
    esac
done
echo "clean=${clean}"
echo "flavor=${flavor}"
echo "build_target=${build_target}"
#exit 1;

projects=( "pmod" "pmod_lib" )

for project in "${projects[@]}"
do
xcodeproj=$project"/"$project".xcodeproj"
target_iOS=$project"_iOS"

if [ -z $flavor ] || [ "$flavor" == "Debug" ]; then
echo "building debug flavor for project:" $xcodeproj
xcodebuild -project $xcodeproj -target $project -configuration Debug $clean build

if [ -z $build_target ] || [ "$build_target" == "os" ]; then
xcodebuild -project $xcodeproj -target $target_iOS -configuration Debug -sdk iphoneos $clean build
fi

if [ -z $build_target ] || [ "$build_target" == "simulator" ]; then
xcodebuild -project $xcodeproj -target $target_iOS -configuration Debug -sdk $iphonesimulator_sdk -arch x86_64 $clean build
fi

fi

if [ -z $flavor ] || [ "$flavor" == "Release" ]; then
echo "building release flavor for project:" $xcodeproj
xcodebuild -project $xcodeproj -target $project -configuration Release $clean build

if [ -z $build_target ] || [ "$build_target" == "os" ]; then
xcodebuild -project $xcodeproj -target $target_iOS -configuration Release -sdk iphoneos $clean build
fi

if [ -z $build_target ] || [ "$build_target" == "simulator" ]; then
xcodebuild -project $xcodeproj -target $target_iOS -configuration Release -sdk $iphonesimulator_sdk -arch x86_64 $clean build
fi

fi

done

