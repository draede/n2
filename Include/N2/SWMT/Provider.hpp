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
#include "N2/CE/IProvider.hpp"
#include "N2/SWMT/IKernel.hpp"
#include "CX/C/Platform/Windows/windows.h"


namespace N2
{

namespace SWMT
{

class Provider : public CE::IProvider
{
public:

	static const DWORD   MAX_DIMS          = 3;
	static const DWORD   THREAD_STACK_SIZE = 65536;

	Provider();

	~Provider();

	virtual CE::IConfig *CreateConfig();

	virtual CX::Status DestroyConfig(CE::IConfig *pConfig);

	virtual CX::Status Init(const CE::IConfig *pConfig = NULL);

	virtual CX::Status Uninit();

	virtual CX::Bool IsOK() const;

	virtual CE::INetwork *CreateNetwork();

	virtual CX::Status DestroyNetwork(CE::INetwork *pNetwork);

	CX::UInt32 GetThreadsCount() const;

	CX::Status RunKernel(IKernel *pKernel, CX::UInt32 cDims, const CX::UInt32 *dims);

private:

	struct Entry
	{
		IKernel      *pKernel;
		HANDLE       *phStopEvent;
		HANDLE       *phStartEvent;
		HANDLE       *phFinishEvent;
		CX::UInt32   cDims;
		CX::UInt32   dims[MAX_DIMS];
		CX::UInt32   idxs[MAX_DIMS];
		CX::UInt32   cCount;
	};

	SRWLOCK      m_srwlThreads;
	HANDLE       *m_stopEvents;
	HANDLE       *m_startEvents;
	HANDLE       *m_finishEvents;
	HANDLE       *m_threads;
	Entry        *m_entries;
	CX::UInt32   m_cThreads;

	static DWORD WINAPI WorkerThread(void *pArg);

};

}//namespace SWMT

}//namespace N2
