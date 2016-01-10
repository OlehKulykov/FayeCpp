Pod::Spec.new do |s|

# Common settings
  s.name         = "FayeCpp+OpenSSL"
  s.version      = "0.2.1"
  s.summary      = "Faye C++ lightweight, cross-platform client with Objective-C client wrapper with SSL support"
  s.description  = <<-DESC
Faye C++ lightweight, cross-platform client with Objective-C client wrapper. 
Library created with Pure C++ (features provided by Standard C++), without heavy STL and C++11 features.
You can use Objective-C wrapper or C++ client directly.
This pod version with SSL support.
                      DESC
  s.homepage     = "https://github.com/OlehKulykov/FayeCpp"
  s.license      = { :type => 'MIT', :file => 'LICENSE' }
  s.author       = { "Oleh Kulykov" => "info@resident.name" }
  s.source       = { :git => 'https://github.com/OlehKulykov/FayeCpp.git', :tag => s.version.to_s, :submodules => "true" }

# Platforms
  s.ios.deployment_target = "5.0"
  s.osx.deployment_target = "10.7"

# Build  
  s.public_header_files = 'fayecpp.h', 'contrib/objc/FayeCppClient.h'
  s.source_files = 'contrib/objc/*.{h,mm}',
    'fayecpp.h',
    'builds/ios/*.{h}',
    'src/*.{h,cpp}',
    'jansson/src/*.{h,c}',
    'libwebsockets/lib/*.{h}',
    'libwebsockets/lib/base64-decode.c',
    'libwebsockets/lib/client-handshake.c',
    'libwebsockets/lib/client-parser.c',
    'libwebsockets/lib/client.c',
    'libwebsockets/lib/context.c',
    'libwebsockets/lib/extension-deflate-frame.c',
    'libwebsockets/lib/extension-deflate-stream.c',
    'libwebsockets/lib/extension.c',
    'libwebsockets/lib/getifaddrs.c',
    'libwebsockets/lib/handshake.c',
    'libwebsockets/lib/header.c',
    'libwebsockets/lib/libwebsockets.c',
    'libwebsockets/lib/lws-plat-unix.c',
    'libwebsockets/lib/output.c',
    'libwebsockets/lib/parsers.c',
    'libwebsockets/lib/pollfd.c',
    'libwebsockets/lib/service.c',
    'libwebsockets/lib/sha-1.c',
    'libwebsockets/lib/alloc.c',
    'libwebsockets/lib/ssl.c',
    'wolfssl/wolfssl/*.{h}',
    'wolfssl/wolfssl/openssl/*.{h}',
    'wolfssl/wolfssl/wolfcrypt/*.{h}',
    'wolfssl/src/crl.c',
    'wolfssl/src/internal.c',
    'wolfssl/src/io.c',
    'wolfssl/src/keys.c',
    'wolfssl/src/ocsp.c',
    'wolfssl/src/sniffer.c',
    'wolfssl/src/ssl.c',
    'wolfssl/src/tls.c',
    'wolfssl/wolfcrypt/src/aes.c',
    'wolfssl/wolfcrypt/src/arc4.c',
    'wolfssl/wolfcrypt/src/asn.c',
    'wolfssl/wolfcrypt/src/blake2b.c',
    'wolfssl/wolfcrypt/src/camellia.c',
    'wolfssl/wolfcrypt/src/chacha.c',
    'wolfssl/wolfcrypt/src/coding.c',
    'wolfssl/wolfcrypt/src/des3.c',
    'wolfssl/wolfcrypt/src/dh.c',
    'wolfssl/wolfcrypt/src/dsa.c',
    'wolfssl/wolfcrypt/src/ecc.c',
    'wolfssl/wolfcrypt/src/error.c',
    'wolfssl/wolfcrypt/src/hash.c',
    'wolfssl/wolfcrypt/src/hc128.c',
    'wolfssl/wolfcrypt/src/hmac.c',
    'wolfssl/wolfcrypt/src/integer.c',
    'wolfssl/wolfcrypt/src/logging.c',
    'wolfssl/wolfcrypt/src/md2.c',
    'wolfssl/wolfcrypt/src/md4.c',
    'wolfssl/wolfcrypt/src/md5.c',
    'wolfssl/wolfcrypt/src/memory.c',
    'wolfssl/wolfcrypt/src/misc.c',
    'wolfssl/wolfcrypt/src/pkcs7.c',
    'wolfssl/wolfcrypt/src/poly1305.c',
    'wolfssl/wolfcrypt/src/pwdbased.c',
    'wolfssl/wolfcrypt/src/rabbit.c',
    'wolfssl/wolfcrypt/src/random.c',
    'wolfssl/wolfcrypt/src/ripemd.c',
    'wolfssl/wolfcrypt/src/rsa.c',
    'wolfssl/wolfcrypt/src/sha.c',
    'wolfssl/wolfcrypt/src/sha256.c',
    'wolfssl/wolfcrypt/src/sha512.c',
    'wolfssl/wolfcrypt/src/tfm.c',
    'wolfssl/wolfcrypt/src/wc_encrypt.c',
    'wolfssl/wolfcrypt/src/wc_port.c'
    
  s.resources = 'contrib/objc/FayeCpp.bundle'
  s.compiler_flags = '-DHAVE_FAYECPP_CONFIG_H=1', '-DHAVE_CONFIG_H=1', '-DCMAKE_BUILD=1', '-DRE_HAVE_COREFOUNDATION_FRAMEWORK=1', '-DLWS_OPENSSL_SUPPORT=1', '-DUSE_WOLFSSL=1'
  s.xcconfig = { 'HEADER_SEARCH_PATHS' => '"${PODS_ROOT}/FayeCpp+OpenSSL/contrib/objc" "${PODS_ROOT}/FayeCpp+OpenSSL/libwebsockets/lib" "${PODS_ROOT}/FayeCpp+OpenSSL/jansson/src" "${PODS_ROOT}/FayeCpp+OpenSSL/wolfssl" "${PODS_ROOT}/FayeCpp+OpenSSL/wolfssl/wolfssl"',
  'ALWAYS_SEARCH_USER_PATHS' => 'YES',
  'USER_HEADER_SEARCH_PATHS' => '"${PODS_ROOT}/FayeCpp+OpenSSL/wolfssl" "${PODS_ROOT}/FayeCpp+OpenSSL/wolfssl/wolfssl"'
  }
  s.libraries = 'z', 'stdc++'
  s.framework = 'CoreFoundation'
  s.requires_arc = true

   

end
