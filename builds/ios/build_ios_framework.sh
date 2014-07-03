rm -rf FayeCpp.framework
rm -f *.a

xcodebuild -configuration Release -project fayecpp.xcodeproj -target fayecpp -arch i386 -sdk iphonesimulator OBJROOT=obj SYMROOT=sym clean build
cd obj
find . -type f -iregex '.*\.o$' | xargs -I @ ar -q -v ../all-i386.a @
cd ..
rm -rf obj
rm -rf sym

xcodebuild -configuration Release -project fayecpp.xcodeproj -target fayecpp -arch x86_64 -sdk iphonesimulator OBJROOT=obj SYMROOT=sym clean build
cd obj
find . -type f -iregex '.*\.o$' | xargs -I @ ar -q -v ../all-x86_64.a @
cd ..
rm -rf obj
rm -rf sym

xcodebuild -configuration Release -project fayecpp.xcodeproj -target fayecpp -arch armv7 -sdk iphoneos OBJROOT=obj SYMROOT=sym clean build
cd obj
find . -type f -iregex '.*\.o$' | xargs -I @ ar -q -v ../all-armv7.a @
cd ..
rm -rf obj
rm -rf sym

xcodebuild -configuration Release -project fayecpp.xcodeproj -target fayecpp -arch armv7s -sdk iphoneos OBJROOT=obj SYMROOT=sym clean build
cd obj
find . -type f -iregex '.*\.o$' | xargs -I @ ar -q -v ../all-armv7s.a @
cd ..
rm -rf obj
rm -rf sym

xcodebuild -configuration Release -project fayecpp.xcodeproj -target fayecpp -arch arm64 -sdk iphoneos OBJROOT=obj SYMROOT=sym IPHONEOS_DEPLOYMENT_TARGET=7.0.0 clean build
cd obj
find . -type f -iregex '.*\.o$' | xargs -I @ ar -q -v ../all-arm64.a @
cd ..
rm -rf obj
rm -rf sym


lipo -create all-i386.a all-x86_64.a all-armv7.a all-armv7s.a all-arm64.a -output all.a
rm -f all-i386.a
rm -f all-x86_64.a
rm -f all-armv7.a
rm -f all-armv7s.a
rm -f all-arm64.a


rm -rf FayeCpp.framework
mkdir -p FayeCpp.framework/Versions/A/Headers

cp ../../fayecpp.h FayeCpp.framework/Versions/A/Headers/
cp ../../contrib/objc/FayeCppClient.h FayeCpp.framework/Versions/A/Headers/

mkdir -p FayeCpp.framework/Versions/A/Resources
cp all.a FayeCpp
cp FayeCpp FayeCpp.framework/Versions/A

ln -s A FayeCpp.framework/Versions/Current
ln -s Versions/Current/Headers FayeCpp.framework/Headers
ln -s Versions/Current/Resources FayeCpp.framework/Resources
ln -s Versions/Current/FayeCpp FayeCpp.framework/FayeCpp

rm -f FayeCpp
rm -f all.a
