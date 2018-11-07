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

#include "N2/CL/Synapses.hpp"
#include "N2/CL/Network.hpp"
#include "N2/CL/Provider.hpp"


using namespace CX;


namespace N2
{

namespace CL
{

Synapses::Synapses(Network *pNetwork)
{
	m_pNetwork     = pNetwork;
	m_pSynapses    = NULL;
	m_pPrevNeurons = NULL;
	m_pNextNeurons = NULL;
	m_cbMemSize    = 0;
}

Synapses::~Synapses()
{
	Uninit();
}

Status Synapses::Init(NET::Synapses *pSynapses)
{
	cl_int   nError;
	Status   status;

	Uninit();

	m_cbMemSize = sizeof(Synapses);
	for (;;)
	{
		if (NULL == m_pNetwork)
		{
			status = Status(Status_NotInitialized, "Invalid context at {1}:{2}", __FILE__, __LINE__);

			break;
		}

		m_weights = cl::Buffer(*m_pNetwork->GetProvider()->GetContext(), CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, 
		                      sizeof(Float) * pSynapses->GetWeightsCount(), 
		                      pSynapses->GetWeights(), &nError);
		if (CL_SUCCESS != nError)
		{
			status = Status(Status_MemAllocFailed, "Failed to allocate {1} bytes at {2}:{3}", 
			                sizeof(Float) * pSynapses->GetWeightsCount(), __FILE__, __LINE__);

			break;
		}
		if (pSynapses->HasBias())
		{
			m_biases = cl::Buffer(*m_pNetwork->GetProvider()->GetContext(), CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, 
			                      sizeof(Float) * pSynapses->GetBiasesCount(), pSynapses->GetBiases(), &nError);
			if (CL_SUCCESS != nError)
			{
				status = Status(Status_MemAllocFailed, "Failed to allocate {1} bytes at {2}:{3}", 
			                   sizeof(Float) * pSynapses->GetBiasesCount(), __FILE__, __LINE__);

				break;
			}
		}
		m_pSynapses   = pSynapses;
		m_cbMemSize += sizeof(Float) * pSynapses->GetWeightsCount();
		if (pSynapses->HasBias())
		{
			m_cbMemSize += sizeof(Float) * pSynapses->GetBiasesCount();
		}

		break;
	}
	if (!status)
	{
		Uninit();
	}

	return status;
}

Status Synapses::Uninit()
{
	m_pSynapses    = NULL;
	m_weights      = cl::Buffer();
	m_biases       = cl::Buffer();
	m_pPrevNeurons = NULL;
	m_pNextNeurons = NULL;
	m_cbMemSize    = 0;

	return Status();
}

Bool Synapses::IsOK() const
{
	return (NULL != m_pSynapses);
}

UInt32 Synapses::GetPrevNeuronsCount() const
{
	if (NULL == m_pSynapses)
	{
		return 0;
	}

	return m_pSynapses->GetPrevNeuronsCount();
}

UInt32 Synapses::GetNextNeuronsCount() const
{
	if (NULL == m_pSynapses)
	{
		return 0;
	}

	return m_pSynapses->GetNextNeuronsCount();
}

UInt32 Synapses::GetWeightsCount() const
{
	if (NULL == m_pSynapses)
	{
		return 0;
	}

	return m_pSynapses->GetWeightsCount();
}

UInt32 Synapses::GetBiasesCount() const
{
	if (NULL == m_pSynapses)
	{
		return 0;
	}

	return m_pSynapses->GetBiasesCount();
}

Bool Synapses::HasBias() const
{
	if (NULL == m_pSynapses)
	{
		return False;
	}

	return m_pSynapses->HasBias();
}

Float Synapses::GetBias() const
{
	if (NULL == m_pSynapses)
	{
		return 0.0f;
	}

	return m_pSynapses->GetBias();
}

const NET::Synapses *Synapses::GetSynapses() const
{
	return m_pSynapses;
}

NET::Synapses *Synapses::GetSynapses()
{
	return m_pSynapses;
}

const CE::INeurons *Synapses::GetPrevNeurons() const
{
	return m_pPrevNeurons;
}

CE::INeurons *Synapses::GetPrevNeurons()
{
	return m_pPrevNeurons;
}

const CE::INeurons *Synapses::GetNextNeurons() const
{
	return m_pNextNeurons;
}

CE::INeurons *Synapses::GetNextNeurons()
{
	return m_pNextNeurons;
}

Status Synapses::SyncToCE(Bool bWait/* = True*/)
{
	if (NULL == m_pSynapses)
	{
		return Status(Status_NotInitialized, "Not initialized at {1}:{2}", __FILE__, __LINE__);
	}

	cl_int   nError;

	if (CL_SUCCESS != (nError = m_pNetwork->GetQueue()->enqueueWriteBuffer(m_weights, bWait ? CL_TRUE : CL_FALSE, 0, 
	                            sizeof(Float) * m_pSynapses->GetWeightsCount(), 
	                            m_pSynapses->GetWeights())))
	{
		return Status(Status_OperationFailed, "Failed to write weights buffer at {1}:{2}", __FILE__, __LINE__);
	}
	if (HasBias())
	{
		if (CL_SUCCESS != (nError = m_pNetwork->GetQueue()->enqueueWriteBuffer(m_biases, bWait ? CL_TRUE : CL_FALSE, 0, 
		                                   sizeof(Float) * m_pSynapses->GetBiasesCount(), m_pSynapses->GetBiases())))
		{
			return Status(Status_OperationFailed, "Failed to write weights buffer at {1}:{2}", __FILE__, __LINE__);
		}
	}

	return Status();
}

Status Synapses::SyncFromCE(Bool bWait/* = True*/)
{
	if (NULL == m_pSynapses)
	{
		return Status(Status_NotInitialized, "Not initialized at {1}:{2}", __FILE__, __LINE__);
	}

	cl_int   nError;

	if (CL_SUCCESS != (nError = m_pNetwork->GetQueue()->enqueueReadBuffer(m_weights, bWait ? CL_TRUE : CL_FALSE, 0, 
	                            sizeof(Float) * m_pSynapses->GetWeightsCount(), 
	                            m_pSynapses->GetWeights())))
	{
		return Status(Status_OperationFailed, "Failed to read weights buffer at {1}:{2}", __FILE__, __LINE__);
	}
	if (HasBias())
	{
		if (CL_SUCCESS != (nError = m_pNetwork->GetQueue()->enqueueReadBuffer(m_biases, bWait ? CL_TRUE : CL_FALSE, 0, 
		                                   sizeof(Float) * m_pSynapses->GetBiasesCount(), m_pSynapses->GetBiases())))
		{
			return Status(Status_OperationFailed, "Failed to read weights buffer at {1}:{2}", __FILE__, __LINE__);
		}
	}

	return Status();
}

Size Synapses::GetMemSize() const
{
	return m_cbMemSize;
}

}//namespace CL

}//namespace N2
