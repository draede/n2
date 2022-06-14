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

#include "N2/NET/Neurons.hpp"
#include "N2/NET/Synapses.hpp"


using namespace CX;


namespace N2
{

namespace NET
{

Neurons::Neurons()
{
	m_cNeurons        = 0;
	m_values          = NULL;
	m_nActivation     = Activation::Identity;
	m_cActivationArgs = 0;
	memset(m_activationArgs, 0, sizeof(m_activationArgs));
	m_pPrevSynapses   = NULL;
	m_pNextSynapses   = NULL;
	m_cbMemSize       = 0;
}

Neurons::~Neurons()
{
	Uninit();
}

Status Neurons::Init(UInt32 cNeuronsCount, ActivationType nActivation/* = Activation::Identity*/,
	                  UInt32 cActivationArgs/* = 0*/, const Float *activationArgs/* = NULL*/)
{
	if (MIN_NEURONS > cNeuronsCount)
	{
		return Status(Status_InvalidArg, "Invalid neurons count ({1}) at {2}:{3}", cNeuronsCount, __FILE__, __LINE__);
	}
	if (Activation::MIN_VALUE > nActivation || Activation::MAX_VALUE < nActivation)
	{
		return Status(Status_InvalidArg, "Invalid activation ({1}) at {2}:{3}", nActivation, __FILE__, __LINE__);
	}
	if (MAX_ACTIVATION_ARGS_COUNT < cActivationArgs)
	{
		return Status(Status_InvalidArg, "Invalid activation args count ({1}) at {2}:{3}", cActivationArgs, __FILE__, 
		              __LINE__);
	}

	Uninit();

	Status   status;

	m_cbMemSize = sizeof(Neurons);
	for (;;)
	{
		m_cNeurons        = cNeuronsCount;
		if (NULL == (m_values = (Float *)Mem::Alloc(sizeof(Float) * cNeuronsCount)))
		{
			status = Status(Status_MemAllocFailed, "Failed to allocate {1} bytes at {2}:{3}", 
			                sizeof(Float) * cNeuronsCount, __FILE__, __LINE__);

			break;
		}
		memset(m_values, 0, sizeof(Float) * cNeuronsCount);
		m_nActivation     = nActivation;
		m_cActivationArgs = cActivationArgs;
		if (0 < cActivationArgs)
		{
			memcpy(m_activationArgs, activationArgs, cActivationArgs * sizeof(Float));
		}
		m_pPrevSynapses   = NULL;
		m_pNextSynapses   = NULL;
		m_cbMemSize += sizeof(Float) * cNeuronsCount;

		break;
	}

	return Status();
}

Status Neurons::Uninit()
{
	if (NULL != m_values)
	{
		Mem::Free(m_values);
	}
	m_cNeurons        = 0;
	m_values          = NULL;
	m_nActivation     = Activation::Identity;
	m_cActivationArgs = 0;
	memset(m_activationArgs, 0, sizeof(m_activationArgs));
	m_pPrevSynapses   = NULL;
	m_pNextSynapses   = NULL;
	m_cbMemSize       = 0;

	return Status();
}

Bool Neurons::IsOK() const
{
	return (0 < m_cNeurons);
}

UInt32 Neurons::GetNeuronsCount() const
{
	return m_cNeurons;
}

const Float *Neurons::GetValues() const
{
	return m_values;
}

Float *Neurons::GetValues()
{
	return m_values;
}

ActivationType Neurons::GetActivation() const
{
	return m_nActivation;
}

UInt32 Neurons::GetActivationArgsCount() const
{
	return m_cActivationArgs;
}

const Float *Neurons::GetActivationArgs() const
{
	return m_activationArgs;
}

const Synapses *Neurons::GetPrevSynapses() const
{
	return m_pPrevSynapses;
}

Synapses *Neurons::GetPrevSynapses()
{
	return m_pPrevSynapses;
}

const Synapses *Neurons::GetNextSynapses() const
{
	return m_pNextSynapses;
}

Synapses *Neurons::GetNextSynapses()
{
	return m_pNextSynapses;
}

Size Neurons::GetMemSize() const
{
	return m_cbMemSize;
}

}//namespace NET

}//namespace N2
