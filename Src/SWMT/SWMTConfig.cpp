/* 
 * N2
 *
 * https://github.com/draede/n2
 * 
 * Copyright (C) 2018 draede
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

#include "N2/SWMT/Config.hpp"
#include "CX/C/Platform/Windows/windows.h"


using namespace CX;


namespace N2
{

namespace SWMT
{

Config::Config()
{
	SYSTEM_INFO                            sysinfo;
	void                                   *pData;
	SYSTEM_LOGICAL_PROCESSOR_INFORMATION   *cpuinfo;
	DWORD                                  dwSize;
	UInt32                                 cCores;

	GetSystemInfo(&sysinfo);
	m_cThreads = (UInt32)sysinfo.dwNumberOfProcessors;

	dwSize = 0;
	if (!GetLogicalProcessorInformation(NULL, &dwSize))
	{
		if (NULL != (pData = Mem::Alloc(dwSize)))
		{
			cpuinfo = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *)pData;
			if (GetLogicalProcessorInformation(cpuinfo, &dwSize))
			{
				cCores = 0;
				while (0 < dwSize)
				{
					if (RelationProcessorCore == cpuinfo->Relationship)
					{
						cCores++;
					}
					dwSize -= sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
					cpuinfo++;
				}
				if (0 < cCores && cCores <= m_cThreads)
				{
					m_cThreads = cCores;
				}
			}
			Mem::Free(pData);
		}
	}
}

Config::~Config()
{
}

void Config::SetThreadsCount(UInt32 cThreads)
{
	m_cThreads = cThreads;
}

UInt32 Config::GetThreadsCount() const
{
	return m_cThreads;
}

}//namespace SWMT

}//namespace N2
