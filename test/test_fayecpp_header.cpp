/*
 *   Copyright (c) 2014 Kulykov Oleh <nonamedemail@gmail.com>
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
#include <fayecpp.h>

using namespace FayeCpp;

int testDefines()
{
	void * ptr;
	ptr = NULL;
	if (ptr != NULL) return 1;
	
	REUInt32 vUI32 = UINT32_MAX;
	if (vUI32 != UINT32_MAX) return 2;
	
	vUI32 = REUInt32Max;
	if (vUI32 != REUInt32Max) return 3;
	
	unsigned short vUS = USHRT_MAX;
	if (vUS != USHRT_MAX) return 4;
	
	REUInt16 vUI16 = USHRT_MAX;
	if (vUI16 != USHRT_MAX) return 5;
	
	vUI16 = REUInt16Max;
	if (vUI16 != REUInt16Max) return 6;
	
	int vI = INT_MAX;
	if (vI != INT_MAX) return 7;
	
	vI = INT_MIN;
	if (vI != INT_MIN) return 8;
	
	vUI32 = RENotFound;
	if (vUI32 != RENotFound) return 9;
	
	vUI32 = REIndexNotFound;
	if (vUI32 != REIndexNotFound) return 10;

	int m = MIN(1, 2);
	if (m != 1) return 11;
	
	m = MIN(-1, -2);
	if (m != -2) return 12;
	
	m = MAX(1, 2);
	if (m != 2) return 13;
	
	m = MAX(-1, -2);
	if (m != -1) return 14;
	
	return EXIT_SUCCESS;
}

int testTypes()
{
	if (sizeof(REByte) != 1) return 1;
	if (sizeof(REUByte) != 1) return 2;
	
	if (sizeof(REInt16) != 2) return 3;
	if (sizeof(REUInt16) != 2) return 4;
	
	if (sizeof(REInt32) != 4) return 5;
	if (sizeof(REUInt32) != 4) return 6;
	
	if (sizeof(REInt64) != 8) return 7;
	if (sizeof(REUInt64) != 8) return 8;
	
	if (sizeof(REFloat32) != 4) return 9;
	if (sizeof(REFloat64) != 8) return 10;
	
	if (sizeof(REUIdentifier) != sizeof(void*)) return 11;
	
	return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) 
{
	RELog::log("Start test");

	
	RELog::log("Test defines ...");
	assert(testDefines() == EXIT_SUCCESS);
	RELog::log("Test defines OK");
	
	RELog::log("Test types ...");
	assert(testTypes() == EXIT_SUCCESS);
	RELog::log("Test types OK");


	RELog::log("All done.");
	return EXIT_SUCCESS;
}
