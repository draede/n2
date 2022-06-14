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

#include "N2/SWMT/Provider.hpp"
#include "N2/SWMT/Network.hpp"
#include "N2/SWMT/Config.hpp"
#include "CX/Print.hpp"


using namespace CX;


namespace N2
{

namespace SWMT
{

Provider::Provider()
{
	InitializeSRWLock(&m_srwlThreads);
	m_stopEvents   = NULL;
	m_startEvents  = NULL;
	m_finishEvents = NULL;
	m_threads      = NULL;
	m_entries      = NULL;
	m_cThreads     = 0;
}

Provider::~Provider()
{
	Uninit();
}

CE::IConfig *Provider::CreateConfig()
{
	return new (std::nothrow) Config();
}

Status Provider::DestroyConfig(CE::IConfig *pConfig)
{
	Config *pCLConfig = dynamic_cast<Config *>(pConfig);

	if (NULL == pCLConfig)
	{
		return Status(Status_InvalidArg, "Invalid arg at {1}:{2}", __FILE__, __LINE__);
	}
	delete pCLConfig;

	return Status();
}

Status Provider::Init(const CE::IConfig *pConfig/* = NULL*/)
{
	if (NULL != pConfig)
	{
		const Config   *pCLConfig = dynamic_cast<const Config *>(pConfig);

		if (NULL == pCLConfig)
		{
			return Status(Status_InvalidArg, "Invalid arg at {1}:{2}", __FILE__, __LINE__);
		}
		m_cThreads = pCLConfig->GetThreadsCount();
	}
	else
	{
		Config   config;

		m_cThreads = config.GetThreadsCount();
	}
	if (0 >= m_cThreads)
	{
		m_cThreads = 1;
	}

	DWORD    dwID;
	Status   status;

	for (;;)
	{
		if (NULL == (m_stopEvents = (HANDLE *)Mem::Alloc(sizeof(HANDLE) * m_cThreads)))
		{
			status = Status(Status_MemAllocFailed, "Failed to allocate {1} bytes at {2}:{3}", sizeof(HANDLE) * m_cThreads, 
			                __FILE__, __LINE__);

			break;
		}
		memset(m_stopEvents, 0, sizeof(HANDLE) * m_cThreads);
		for (UInt32 i = 0; i < m_cThreads; i++)
		{
			if (NULL == (m_stopEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL)))
			{
				status = Status(Status_OperationFailed, "Failed to create stop event {1} at {2}:{3}", i, __FILE__, 
				                __LINE__);
			}
		}
		if (NULL == (m_startEvents = (HANDLE *)Mem::Alloc(sizeof(HANDLE) * m_cThreads)))
		{
			status = Status(Status_MemAllocFailed, "Failed to allocate {1} bytes at {2}:{3}", sizeof(HANDLE) * m_cThreads, 
			                __FILE__, __LINE__);

			break;
		}
		memset(m_startEvents, 0, sizeof(HANDLE) * m_cThreads);
		for (UInt32 i = 0; i < m_cThreads; i++)
		{
			if (NULL == (m_startEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL)))
			{
				status = Status(Status_OperationFailed, "Failed to create start event {1} at {2}:{3}", i, __FILE__, 
				                __LINE__);
			}
		}
		if (NULL == (m_finishEvents = (HANDLE *)Mem::Alloc(sizeof(HANDLE) * m_cThreads)))
		{
			status = Status(Status_MemAllocFailed, "Failed to allocate {1} bytes at {2}:{3}", sizeof(HANDLE) * m_cThreads, 
			                __FILE__, __LINE__);

			break;
		}
		memset(m_finishEvents, 0, sizeof(HANDLE) * m_cThreads);
		for (UInt32 i = 0; i < m_cThreads; i++)
		{
			if (NULL == (m_finishEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL)))
			{
				status = Status(Status_OperationFailed, "Failed to create finish event {1} at {2}:{3}", i, __FILE__, 
				                __LINE__);
			}
		}
		if (NULL == (m_entries = (Entry *)Mem::Alloc(sizeof(Entry) * m_cThreads)))
		{
			status = Status(Status_MemAllocFailed, "Failed to allocate {1} bytes at {2}:{3}", sizeof(Entry) * m_cThreads, 
			                __FILE__, __LINE__);

			break;
		}
		memset(m_entries, 0, sizeof(Entry) * m_cThreads);
		if (NULL == (m_threads = (HANDLE *)Mem::Alloc(sizeof(HANDLE) * m_cThreads)))
		{
			status = Status(Status_MemAllocFailed, "Failed to allocate {1} bytes at {2}:{3}", sizeof(HANDLE) * m_cThreads, 
			                __FILE__, __LINE__);

			break;
		}
		memset(m_threads, 0, sizeof(HANDLE) * m_cThreads);
		for (UInt32 i = 0; i < m_cThreads; i++)
		{
			if (NULL == (m_threads[i] = CreateThread(NULL, THREAD_STACK_SIZE, &Provider::WorkerThread, &m_entries[i], 0, 
			                                         &dwID)))
			{
				status = Status(Status_OperationFailed, "Failed to create thread {1} with error {2} at {3}:{4}", 
				                i, (int)GetLastError(), __FILE__, __LINE__);

				break;
			}
			m_entries[i].phStopEvent   = &m_stopEvents[i];
			m_entries[i].phStartEvent  = &m_startEvents[i];
			m_entries[i].phFinishEvent = &m_finishEvents[i];
		}
		if (!status)
		{
			break;
		}

		break;
	}
	if (!status)
	{
		Uninit();
	}

	return status;
}

Status Provider::Uninit()
{
	if (NULL != m_threads)
	{
		DWORD   dwCount = 0;

		for (UInt32 i = 0; i < m_cThreads; i++)
		{
			if (NULL != m_threads[i])
			{
				SetEvent(m_stopEvents[i]);
				dwCount++;
			}
		}
		if (0 < dwCount)
		{
			WaitForMultipleObjects(dwCount, m_threads, TRUE, INFINITE);
		}
		for (UInt32 i = 0; i < m_cThreads; i++)
		{
			if (NULL != m_threads[i])
			{
				CloseHandle(m_threads[i]);
			}
		}
		Mem::Free(m_threads);
	}
	if (NULL != m_entries)
	{
		Mem::Free(m_entries);
	}
	if (NULL != m_finishEvents)
	{
		for (UInt32 i = 0; i < m_cThreads; i++)
		{
			if (NULL != m_finishEvents[i])
			{
				CloseHandle(m_finishEvents[i]);
			}
		}
		Mem::Free(m_finishEvents);
	}
	if (NULL != m_startEvents)
	{
		for (UInt32 i = 0; i < m_cThreads; i++)
		{
			if (NULL != m_startEvents[i])
			{
				CloseHandle(m_startEvents[i]);
			}
		}
		Mem::Free(m_startEvents);
	}
	if (NULL != m_stopEvents)
	{
		for (UInt32 i = 0; i < m_cThreads; i++)
		{
			if (NULL != m_stopEvents[i])
			{
				CloseHandle(m_stopEvents[i]);
			}
		}
		Mem::Free(m_stopEvents);
	}
	m_threads      = NULL;
	m_stopEvents   = NULL;
	m_startEvents  = NULL;
	m_finishEvents = NULL;
	m_entries      = NULL;
	m_cThreads     = 0;


	return Status();
}

Bool Provider::IsOK() const
{
	return True;
}

CE::INetwork *Provider::CreateNetwork()
{
	return new (std::nothrow) Network(this);
}

Status Provider::DestroyNetwork(CE::INetwork *pNetwork)
{
	Network *pCLNetwork = dynamic_cast<Network *>(pNetwork);

	if (NULL == pCLNetwork)
	{
		return Status(Status_InvalidArg, "Invalid arg at {1}:{2}", __FILE__, __LINE__);
	}

	delete pCLNetwork;

	return Status();
}

UInt32 Provider::GetThreadsCount() const
{
	return m_cThreads;
}

Status Provider::RunKernel(IKernel *pKernel, UInt32 cDims, const UInt32 *dims)
{
	if (0 == m_cThreads)
	{
		return Status(Status_NotInitialized, "Invalid arg at {1}:{2}", __FILE__, __LINE__);
	}
	if (MAX_DIMS < cDims)
	{
		return Status(Status_InvalidArg, "Invalid arg at {1}:{2}", __FILE__, __LINE__);
	}

	AcquireSRWLockExclusive(&m_srwlThreads);

	for (UInt32 i = 0; i < m_cThreads; i++)
	{
		m_entries[i].pKernel = pKernel;
	}

	unsigned int   cThread;
	unsigned int   cTotalItems;
	unsigned int   cItemsPerThread;
	unsigned int   cItemsCount;

	cTotalItems = 0;
	if (3 == cDims)
	{
		cTotalItems     = dims[0] * dims[1] * dims[2];
		cItemsPerThread = cTotalItems / m_cThreads;
		if (0 < cTotalItems % m_cThreads)
		{
			cItemsPerThread++;
		}
		cItemsCount = 0;
		cThread     = 0;
		for (unsigned int cIdx1 = 0; cIdx1 < dims[0]; cIdx1++)
		{
			for (unsigned int cIdx2 = 0; cIdx2 < dims[1]; cIdx2++)
			{
				for (unsigned int cIdx3 = 0; cIdx3 < dims[2]; cIdx3++)
				{
					if (0 == cItemsCount)
					{
						m_entries[cThread].cDims   = cDims;
						memcpy(m_entries[cThread].dims, dims, sizeof(dims));
						m_entries[cThread].cCount  = 1;
						m_entries[cThread].idxs[0] = cIdx1;
						m_entries[cThread].idxs[1] = cIdx2;
						m_entries[cThread].idxs[2] = cIdx3;
					}
					else
					{
						m_entries[cThread].cCount++;
					}
					cItemsCount++;
					cTotalItems--;
					if (0 == cTotalItems || cItemsPerThread == cItemsCount)
					{
						cItemsCount = 0;
						cThread++;
					}
				}
			}
		}
	}
	else
	if (2 == cDims)
	{
		cTotalItems     = dims[0] * dims[1];
		cItemsPerThread = cTotalItems / m_cThreads;
		if (0 < cTotalItems % m_cThreads)
		{
			cItemsPerThread++;
		}
		cItemsCount  = 0;
		cThread = 0;
		for (unsigned int cIdx1 = 0; cIdx1 < dims[0]; cIdx1++)
		{
			for (unsigned int cIdx2 = 0; cIdx2 < dims[1]; cIdx2++)
			{
				if (0 == cItemsCount)
				{
					m_entries[cThread].cDims   = cDims;
					memcpy(m_entries[cThread].dims, dims, sizeof(dims));
					m_entries[cThread].cCount  = 1;
					m_entries[cThread].idxs[0] = cIdx1;
					m_entries[cThread].idxs[1] = cIdx2;
				}
				else
				{
					m_entries[cThread].cCount++;
				}
				cItemsCount++;
				cTotalItems--;
				if (0 == cTotalItems || cItemsPerThread == cItemsCount)
				{
					cItemsCount = 0;
					cThread++;
				}
			}
		}
	}
	else
	{
		cTotalItems     = dims[0];
		cItemsPerThread = cTotalItems / m_cThreads;
		if (0 < cTotalItems % m_cThreads)
		{
			cItemsPerThread++;
		}
		cItemsCount  = 0;
		cThread = 0;
		for (unsigned int cIdx1 = 0; cIdx1 < dims[0]; cIdx1++)
		{
			if (0 == cItemsCount)
			{
				m_entries[cThread].cDims   = cDims;
				memcpy(m_entries[cThread].dims, dims, sizeof(dims));
				m_entries[cThread].cCount  = 1;
				m_entries[cThread].idxs[0] = cIdx1;
			}
			else
			{
				m_entries[cThread].cCount++;
			}
			cItemsCount++;
			cTotalItems--;
			if (0 == cTotalItems || cItemsPerThread == cItemsCount)
			{
				cItemsCount = 0;
				cThread++;
			}
		}
	}
	for (UInt32 i = cThread; i < m_cThreads; i++)
	{
		m_entries[i].cCount = 0;
	}

	for (UInt32 i = 0; i < m_cThreads; i++)
	{
		SetEvent(m_startEvents[i]);
	}
	WaitForMultipleObjects((DWORD)m_cThreads, m_finishEvents, TRUE, INFINITE);

	ReleaseSRWLockExclusive(&m_srwlThreads);

	return Status();
}

DWORD WINAPI Provider::WorkerThread(void *pArg)
{
	Entry    *pEntry    = (Entry *)pArg;
	HANDLE   handles[2] = { *pEntry->phStopEvent, *pEntry->phStartEvent };
	DWORD    dwRet;

	for (;;)
	{
		dwRet = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
		if (WAIT_OBJECT_0 == dwRet)
		{
			break;
		}
		else
		if (WAIT_OBJECT_0 + 1 == dwRet)
		{
			if (0 < pEntry->cCount)
			{
				pEntry->pKernel->Run(pEntry->cDims, pEntry->dims, pEntry->idxs, pEntry->cCount);
			}
			SetEvent(*pEntry->phFinishEvent);
		}
	}

	return 0;
}

}//namespace SWMT

}//namespace N2
