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
#include "CX/Vector.hpp"
#include "N2/CL/OpenCL.hpp"


namespace N2
{

namespace CL
{

class KernelSources
{
public:

	static const CX::UInt64   MAX_SOURCE_LEN = 16777216;

	//to load from file use file://<path> for szSource
	static CX::Status Register(const CX::Char *szSource, CX::Size cSourceLen = (CX::Size) - 1);

	static CX::Size GetCount();

	static const CX::Char *GetSource(CX::Size cIndex);

	static CX::Size GetSourceLen(CX::Size cIndex);

	static CX::Status Build(cl::Device &device, cl::Context &context, cl::Program **ppProgram, 
	                        CX::String *psBuildLog = NULL);

private:

	struct Source
	{
		CX::String   sSource;
	};

	typedef CX::Vector<Source>::Type   SourcesVector;

	KernelSources();

	~KernelSources();

	static SourcesVector *GetSources();

	static CX::Status LoadSources(cl::Program::Sources *pSources);

};

}//namespace CL

}//namespace N2
