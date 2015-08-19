/*
 *   Copyright (c) 2014 - 2015 Kulykov Oleh <info@resident.name>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */


#include <stdlib.h>
#include <assert.h>


#if defined(CMAKE_BUILD)
#undef CMAKE_BUILD
#endif

#if defined(__BUILDING_RECORE_DYNAMIC_LIBRARY__)
#undef __BUILDING_RECORE_DYNAMIC_LIBRARY__
#endif


#include <fayecpp.h>

using namespace FayeCpp;

int testVariantMessage1()
{
	REVariant var1("Hello");

	if (!var1.isEqualToVariant("Hello")) return 1;
	if (var1.isEqualToVariant("Hello1")) return 2;

	REVariantList list1;
	list1 += 1;
	list1 += true;

	REVariantList list2;
	list2 += true;
	list2 += 1;

	if (!list1.isEqualToList(list2)) return 3;
	var1 = list1;
	REVariant var2 = list2;

	if (var1.isEqualToVariant("das")) return 4;
	if (var1.isEqualToVariant(true)) return 5;
	if (!var1.isEqualToVariant(var2)) return 6;

	list2 += 3;
	if (list1.isEqualToList(list2)) return 7;

	list2.clear();
	if (list1.isEqualToList(list2)) return 8;


	REVariantMap map1, map2;
	map1["k1"] = true;
	map2["k1"] = true;
	if (!map1.isEqualToMap(map2)) return 9;
	map2["k2"] = 45;
	if (map1.isEqualToMap(map2)) return 10;

	return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
	RELog::log("Client info: %s", Client::info());
	RELog::log("Start test");


	RELog::log("Test test variant message #1 ...");
	int test = testVariantMessage1();
	assert(test == EXIT_SUCCESS);
	RELog::log("Test test variant message #1 OK");

	RELog::log("All done.");
	return EXIT_SUCCESS;
}
