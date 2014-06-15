# FayeCpp - C++ Faye client.


**FayeCpp** (C++) client library for desktop & mobile platforms, such as **Mac**, **Windows**, **Linux**, **iOS**, **Android**. Library created with "Pure C++" (features provided by Standard C++), so **no STL** and **no C++11** features.


# Installation
--------------


## Getting the sources
```sh
git clone https://github.com/OlehKulykov/FayeCpp.git
cd FayeCpp
git submodule init
git submodule update
```

## Dependencies
 * [Libwebsockets] - "lightweight pure C library built to use minimal CPU and memory resources".
 * [Jansson] - "C library for encoding, decoding and manipulating JSON data".
 * [pthreads] OR [Windows Threads] which used by [RECore] threads, or use **FayeCpp** with [Qt SDK] (see below).


## Building
-----------
> Use (install or update) latest [CMake] build system, need version 2.8.12 or later.

### Build on Unix like platforms

```sh
cd FayeCpp
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

> During configuration phase, [Libwebsockets] and [Jansson] will be also configured, so, you can add cmake flags for this libs. Something like:
> ```sh
> cmake -DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX:PATH=/usr \
-DJANSSON_BUILD_DOCS:BOOL=OFF \
-DLWS_WITHOUT_DEBUG:BOOL=ON ..
> ```
> For more options read [Libwebsockets] and [Jansson] documentation.

### Build on Windows with Microsoft Visual Studio
 * Execute **Start** -> **Microsoft Visual Studio ....** -> **Visual Studio Tools** -> **... Tools Command Prompt** with administrative permissions (Context menu: **Run as administrator** ).
 * Do the same as on **Build on Unix like platforms**, BUT use **nmake** to build and/or install library.


## Integration
--------------


### Integration with Qt
Just include file ```fayecpp.pri``` to your ```*.pro``` file.


### Integrating with Xcode iOS project
For easy integration there is Xcode static iOS library project ```fayecpp.xcodeproj``` located in ```builds/ios/``` folder, full path is: ```builds/ios/fayecpp.xcodeproj```. So integration is the same as any other third party static library.
 * On you **main project** use context menu ```Add Files to"<main project>"...```, locate ```fayecpp.xcodeproj``` and add. Or drag ```fayecpp.xcodeproj``` to your **main project**.
 * On **main project** target locate ```Build Phases``` and expand ```Link Binary With Libraries``` group.
 * Click **plus button**, locate & select ```libfayecpp.a``` and press **Add**.
 * On the project/taget ```Build Settings``` locate ```Search Paths``` group.
  * Change option ```Always Search User Paths``` to **YES**.
  * Add to ```Header Search Paths```  folder where main header ```fayecpp.h``` located (root of the repository).

> Now you can include FayeCpp header as ```#include <fayecpp.h>```.
> When you including FayeCpp header to Objective-C code - don't forget change file extension from ```*.m``` to ```*.mm```.


### Using with Objective-C code.
For **Mac** & **iOS** there is **Objective-C client wrapper** located in the folder ```contrib/objc/```. Just add ```FayeCppClient.h```, ```FayeCppClient.mm``` files to your project and use. This wrapper can be used **with** or **without** Automatic Reference Counting (ARC). 


# License
---------

The MIT License (MIT)

Copyright (c) 2014 Kulykov Oleh <nonamedemail@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


[Libwebsockets]:http://libwebsockets.org
[Jansson]:http://www.digip.org/jansson
[CMake]:http://www.cmake.org
[pthreads]:https://computing.llnl.gov/tutorials/pthreads
[Windows Threads]:http://msdn.microsoft.com/en-us/library/windows/desktop/ms686937
[Qt SDK]:http://qt-project.org
[RECore]:https://github.com/OlehKulykov/recore

