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

#include "N2/SWST/Neurons.hpp"
#include "N2/SWST/Network.hpp"
#include "N2/SWST/Provider.hpp"
#include "N2/SWST/Synapses.hpp"


using namespace CX;


namespace N2
{

namespace SWST
{

Neurons::Neurons(Network *pNetwork)
{
	m_pNetwork             = pNetwork;
	m_pNeurons             = NULL;
	m_pPrevSynapses        = NULL;
	m_pNextSynapses        = NULL;
	m_values               = NULL;
	m_cbMemSize            = 0;
}

Neurons::~Neurons()
{
	Uninit();
}

Status Neurons::Init(NET::Neurons *pNeurons)
{
	Uninit();

	UInt32   cNeurons;
	Status   status;

	m_cbMemSize = sizeof(Neurons);
	for (;;)
	{
		if (NULL == m_pNetwork)
		{
			status = Status(Status_NotInitialized, "Invalid context at {1}:{2}", __FILE__, __LINE__);

			break;
		}
		cNeurons = pNeurons->GetNeuronsCount();
		if (NULL == (m_values = (Float *)Mem::Alloc(sizeof(Float) * cNeurons)))
		{
			status = Status(Status_MemAllocFailed, "Failed to allocate {1} bytes at {2}:{3}", sizeof(Float) * cNeurons, 
			                __FILE__, __LINE__);

			break;
		}
		memcpy(m_values, pNeurons->GetValues(), sizeof(Float) * cNeurons);
		m_pNeurons      = pNeurons;
		m_pPrevSynapses = NULL;
		m_pNextSynapses = NULL;
		m_cbMemSize += sizeof(Float) * pNeurons->GetNeuronsCount();

		break;
	}
	if (!status)
	{
		Uninit();
	}

	return status;
}

Status Neurons::Uninit()
{
	if (NULL != m_values)
	{
		Mem::Free(m_values);
	}
	m_pNeurons             = NULL;
	m_pPrevSynapses        = NULL;
	m_pNextSynapses        = NULL;
	m_values               = NULL;
	m_cbMemSize            = 0;

	return Status();
}

Bool Neurons::IsOK() const
{
	return (NULL != m_pNeurons);
}

UInt32 Neurons::GetNeuronsCount() const
{
	if (NULL == m_pNeurons)
	{
		return 0;
	}

	return m_pNeurons->GetNeuronsCount();
}

NET::ActivationType Neurons::GetActivation() const
{
	if (NULL == m_pNeurons)
	{
		return NET::Activation::Identity;
	}

	return m_pNeurons->GetActivation();
}

UInt32 Neurons::GetActivationArgsCount() const
{
	if (NULL == m_pNeurons)
	{
		return 0;
	}

	return m_pNeurons->GetActivationArgsCount();
}

const Float *Neurons::GetActivationArgs() const
{
	if (NULL == m_pNeurons)
	{
		return NULL;
	}

	return m_pNeurons->GetActivationArgs();
}

const NET::Neurons *Neurons::GetNeurons() const
{
	return m_pNeurons;
}

NET::Neurons *Neurons::GetNeurons()
{
	return m_pNeurons;
}

const CE::ISynapses *Neurons::GetPrevSynapses() const
{
	return m_pPrevSynapses;
}

CE::ISynapses *Neurons::GetPrevSynapses()
{
	return m_pPrevSynapses;
}

const CE::ISynapses *Neurons::GetNextSynapses() const
{
	return m_pNextSynapses;
}

CE::ISynapses *Neurons::GetNextSynapses()
{
	return m_pNextSynapses;
}

Status Neurons::SyncToCE(Bool bWait/* = True*/)
{
	if (NULL == m_pNeurons)
	{
		return Status(Status_NotInitialized, "Not initialized at {1}:{2}", __FILE__, __LINE__);
	}

	CX_UNUSED(bWait);

	memcpy(m_values, m_pNeurons->GetValues(), sizeof(Float) * m_pNeurons->GetNeuronsCount());

	return Status();
}

Status Neurons::SyncFromCE(Bool bWait/* = True*/)
{
	if (NULL == m_pNeurons)
	{
		return Status(Status_NotInitialized, "Not initialized at {1}:{2}", __FILE__, __LINE__);
	}

	CX_UNUSED(bWait);

	memcpy(m_pNeurons->GetValues(), m_values, sizeof(Float) * m_pNeurons->GetNeuronsCount());

	return Status();
}

Size Neurons::GetMemSize() const
{
	return m_cbMemSize;
}

}//namespace SWST

}//namespace N2
