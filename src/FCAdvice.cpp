/*
 *   Copyright (c) 2014 - 2016 Kulykov Oleh <info@resident.name>
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


#include "../fayecpp.h"

#if defined(HAVE_FAYECPP_CONFIG_H)
#include "fayecpp_config.h"
#endif

namespace FayeCpp {

	int Advice::interval() const
	{
		return _interval;
	}

	int Advice::timeout() const
	{
		return _timeout;
	}

	Advice::ReconnectType Advice::reconnect() const
	{
		return (ReconnectType)_reconnect;
	}
	
	void Advice::setInterval(const int value)
	{
		_interval = value;
	}

	void Advice::setTimeout(const int value)
	{
		_timeout = value;
	}

	void Advice::setReconnect(const Advice::ReconnectType value)
	{
		_reconnect = value;
	}

	Advice & Advice::operator=(const Advice & advice)
	{
		_interval = advice._interval;
		_timeout = advice._timeout;
		_reconnect = advice._reconnect;

		return *this;
	}

	Advice::Advice() :
		_interval(-1),
		_timeout(-1),
		_reconnect(Advice::ReconnectNone)
	{

	}

	Advice::Advice(const Advice & advice) :
		_interval(advice._interval),
		_timeout(advice._timeout),
		_reconnect(advice._reconnect)
	{

	}

	Advice::~Advice()
	{
		
	}

}



