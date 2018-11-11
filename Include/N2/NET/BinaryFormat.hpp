/* 
 * N2
 *
 * https://github.com/draede/n2
 * 
 * Copyright (C) 2018 draede - draede [at] outlook [dot] com
 *
 * Released under the MIT License.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once


#include "CX/Types.hpp"
#include "CX/Status.hpp"
#include "CX/IO/IInputStream.hpp"
#include "CX/IO/IOutputStream.hpp"
#include "N2/NET/Network.hpp"


namespace N2
{

namespace NET
{

class BinaryFormat
{
public:

	static const CX::UInt32   NEURONS_MAGIC    = 0x444E324E;
	static const CX::UInt32   NEURONS_VERSION  = 0x00000001;

	static const CX::UInt32   SYNAPSES_MAGIC   = 0x4453324E;
	static const CX::UInt32   SYNAPSES_VERSION = 0x00000001;

	static CX::Status LoadNeurons(Network *pNetwork, const CX::Char *szPath);

	static CX::Status SaveNeurons(const Network *pNetwork, const CX::Char *szPath);

	static CX::Status LoadSynapses(Network *pNetwork, const CX::Char *szPath);

	static CX::Status SaveSynapses(const Network *pNetwork, const CX::Char *szPath);

private:

	BinaryFormat();

	~BinaryFormat();

	template <typename T>
	static CX::Status Write(CX::IO::IOutputStream *pOutputStream, T val)
	{
		return Write(pOutputStream, &val, sizeof(T));
	}

	static CX::Status Write(CX::IO::IOutputStream *pOutputStream, const void *pData, CX::Size cbSize);

	template <typename T>
	static CX::Status Read(CX::IO::IInputStream *pInputStream, T *pVal)
	{
		return Read(pInputStream, pVal, sizeof(T));
	}

	static CX::Status Read(CX::IO::IInputStream *pInputStream, void *pData, CX::Size cbSize);

};

}//namespace NET

}//namespace N2
