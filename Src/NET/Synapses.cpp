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

#include "N2/NET/Synapses.hpp"
#include "N2/NET/Neurons.hpp"


using namespace CX;


namespace N2
{

namespace NET
{

Synapses::Synapses()
{
	m_cPrevNeurons = 0;
	m_cNextNeurons = 0;
	m_bHasBias     = False;
	m_fBias        = 0.0f;
	m_weigths      = NULL;
	m_biases       = NULL;
	m_pPrevNeurons = NULL;
	m_pNextNeurons = NULL;
	m_cbMemSize    = 0;
}

Synapses::~Synapses()
{
	Uninit();
}

Status Synapses::Init(UInt32 cPrevNeuronsCount, UInt32 cNextNeuronsCount, Bool bHasBias/* = False*/, 
	                   Float fBias/* = 1.0f*/)
{
	Uninit();

	Status   status;

	m_cbMemSize = sizeof(Synapses);
	for (;;)
	{
		m_cPrevNeurons = cPrevNeuronsCount;
		m_cNextNeurons = cNextNeuronsCount;
		m_bHasBias     = bHasBias;
		m_fBias        = fBias;
		if (NULL == (m_weigths = (Float *)Mem::Alloc(sizeof(Float) * cPrevNeuronsCount * cNextNeuronsCount)))
		{
			status = Status(Status_MemAllocFailed, "Failed to allocate {1} bytes at {2}:{3}", 
			                sizeof(Float) * cPrevNeuronsCount * cNextNeuronsCount, __FILE__, __LINE__);

			break;
		}
		memset(m_weigths, 0, sizeof(Float) * cPrevNeuronsCount * cNextNeuronsCount);
		if (NULL == (m_biases = (Float *)Mem::Alloc(sizeof(Float) * cNextNeuronsCount)))
		{
			status = Status(Status_MemAllocFailed, "Failed to allocate {1} bytes at {2}:{3}", 
			                sizeof(Float) * cNextNeuronsCount, __FILE__, __LINE__);

			break;
		}
		memset(m_biases, 0, sizeof(Float) * cNextNeuronsCount);
		m_pPrevNeurons = NULL;
		m_pNextNeurons = NULL;
		m_cbMemSize += sizeof(Float) * cPrevNeuronsCount * cNextNeuronsCount + sizeof(Float) * cNextNeuronsCount;

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
	if (NULL != m_biases)
	{
		Mem::Free(m_biases);
	}
	if (NULL != m_weigths)
	{
		Mem::Free(m_weigths);
	}
	m_cPrevNeurons = 0;
	m_cNextNeurons = 0;
	m_bHasBias     = False;
	m_fBias        = 0.0f;
	m_weigths      = NULL;
	m_biases       = NULL;
	m_pPrevNeurons = NULL;
	m_pNextNeurons = NULL;
	m_cbMemSize    = 0;

	return Status();
}

Bool Synapses::IsOK() const
{
	return (0 < m_cPrevNeurons && 0 < m_cNextNeurons);
}

UInt32 Synapses::GetPrevNeuronsCount() const
{
	return m_cPrevNeurons;
}

UInt32 Synapses::GetNextNeuronsCount() const
{
	return m_cNextNeurons;
}

Bool Synapses::HasBias() const
{
	return m_bHasBias;
}

Float Synapses::GetBias() const
{
	return m_fBias;
}

UInt32 Synapses::GetWeightsCount() const
{
	return m_cPrevNeurons * m_cNextNeurons;
}

const Float *Synapses::GetWeights() const
{
	return m_weigths;
}

Float *Synapses::GetWeights()
{
	return m_weigths;
}

UInt32 Synapses::GetBiasesCount() const
{
	if (m_bHasBias)
	{
		return m_cNextNeurons;
	}
	else
	{
		return 0;
	}
}

const Float *Synapses::GetBiases() const
{
	return m_biases;
}

Float *Synapses::GetBiases()
{
	return m_biases;
}

const Neurons *Synapses::GetPrevNeurons() const
{
	return m_pPrevNeurons;
}

Neurons *Synapses::GetPrevNeurons()
{
	return m_pPrevNeurons;
}

const Neurons *Synapses::GetNextNeurons() const
{
	return m_pNextNeurons;
}

Neurons *Synapses::GetNextNeurons()
{
	return m_pNextNeurons;
}

Size Synapses::GetMemSize() const
{
	return m_cbMemSize;
}

}//namespace NET

}//namespace N2
