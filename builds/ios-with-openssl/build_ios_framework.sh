rm -rf FayeCpp.framework
rm -f *.a

git clone git://github.com/krzyzanowskim/OpenSSL.git openssl
cd openssl
./build.sh
cd ..

lipo -extract i386 openssl/lib-ios/libcrypto.a -o libcrypto-i386.a
lipo -extract x86_64 openssl/lib-ios/libcrypto.a -o libcrypto-x86_64.a
lipo -extract armv7 openssl/lib-ios/libcrypto.a -o libcrypto-armv7.a
lipo -extract armv7s openssl/lib-ios/libcrypto.a -o libcrypto-armv7s.a
lipo -extract arm64 openssl/lib-ios/libcrypto.a -o libcrypto-arm64.a

lipo -extract i386 openssl/lib-ios/libssl.a -o libssl-i386.a
lipo -extract x86_64 openssl/lib-ios/libssl.a -o libssl-x86_64.a
lipo -extract armv7 openssl/lib-ios/libssl.a -o libssl-armv7.a
lipo -extract armv7s openssl/lib-ios/libssl.a -o libssl-armv7s.a
lipo -extract arm64 openssl/lib-ios/libssl.a -o libssl-arm64.a


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

libtool -static all-i386.a libssl-i386.a libcrypto-i386.a -o all-ssl-i386.a
libtool -static all-x86_64.a libssl-x86_64.a libcrypto-x86_64.a -o all-ssl-x86_64.a
libtool -static all-armv7.a libssl-armv7.a libcrypto-armv7.a -o all-ssl-armv7.a
libtool -static all-armv7s.a libssl-armv7s.a libcrypto-armv7s.a -o all-ssl-armv7s.a
libtool -static all-arm64.a libssl-arm64.a libcrypto-arm64.a -o all-ssl-arm64.a

lipo -create all-ssl-i386.a all-ssl-x86_64.a all-ssl-armv7.a all-ssl-armv7s.a all-ssl-arm64.a -output all.a

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
rm -f *.a
