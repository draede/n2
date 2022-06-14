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

#include "N2/NET/Network.hpp"


using namespace CX;


namespace N2
{

namespace NET
{

Network::Network()
{
	m_cLayers        = 0;
	m_pInputNeurons  = NULL;
	m_pOutputNeurons = NULL;
	m_cbMemSize      = 0;
}

Network::~Network()
{
	Uninit();
}

Status Network::Init(UInt32 cInputNeuronsCount, UInt32 cLayersCount, const Layer *layers)
{
	if (Neurons::MIN_NEURONS > cInputNeuronsCount)
	{
		return Status(Status_InvalidArg, "Invalid input neurons count ({1}) at {2}:{3}", cInputNeuronsCount, __FILE__, 
		              __LINE__);
	}
	if (MIN_LAYERS > cLayersCount)
	{
		return Status(Status_InvalidArg, "Invalid layers count ({1}) at {2}:{3}", cLayersCount, __FILE__, __LINE__);
	}

	Uninit();

	Neurons    *pNeurons;
	Synapses   *pSynapses;
	Status     status;

	m_cbMemSize = sizeof(Network);
	for (;;)
	{
		if (NULL == (pNeurons = new (std::nothrow) Neurons()))
		{
			status = Status(Status_MemAllocFailed, "Failed to allocate neurons at {1}:{2}", __FILE__, __LINE__);

			break;
		}
		if (!(status = pNeurons->Init(cInputNeuronsCount)))
		{
			delete pNeurons;

			break;
		}
		pNeurons->m_pPrevSynapses = NULL;
		pNeurons->m_pNextSynapses = NULL;
		m_pInputNeurons           = pNeurons;
		m_pOutputNeurons          = pNeurons;
		m_cbMemSize += pNeurons->GetMemSize();

		for (UInt32 i = 0; i < cLayersCount; i++)
		{
			if (NULL == (pSynapses = new (std::nothrow) Synapses()))
			{
				status = Status(Status_MemAllocFailed, "Failed to allocate synapses at {1}:{2}", __FILE__, __LINE__);

				break;
			}
			if (!(status = pSynapses->Init(m_pOutputNeurons->m_cNeurons, layers[i].cNeuronsCount, layers[i].bHasBias, 
				                              layers[i].fBiasValue)))
			{
				delete pSynapses;

				break;
			}

			if (NULL == (pNeurons = new (std::nothrow) Neurons()))
			{
				status = Status(Status_MemAllocFailed, "Failed to allocate neurons at {1}:{2}", __FILE__, __LINE__);

				break;
			}
			if (!(status = pNeurons->Init(layers[i].cNeuronsCount, layers[i].nActivation, layers[i].cActivationArgs, 
			                              layers[i].activationArgs)))
			{
				delete pSynapses;
				delete pNeurons;

				break;
			}

			pSynapses->m_pPrevNeurons         = m_pOutputNeurons;
			pSynapses->m_pNextNeurons         = pNeurons;
			pNeurons->m_pPrevSynapses         = pSynapses;
			pNeurons->m_pNextSynapses         = NULL;

			m_pOutputNeurons->m_pNextSynapses = pSynapses;

			m_pOutputNeurons                  = pNeurons;

			m_cbMemSize += pSynapses->GetMemSize() + pNeurons->GetMemSize();
		}
		if (!status)
		{
			break;
		}
		m_cLayers = cLayersCount;

		break;
	}
	if (!status)
	{
		Uninit();
	}

	return status;
}

Status Network::Uninit()
{
	Neurons    *pNeurons;
	Synapses   *pSynapses;

	if (NULL != m_pInputNeurons)
	{
		pSynapses = m_pInputNeurons->m_pNextSynapses;
		delete m_pInputNeurons;

		while (NULL != pSynapses)
		{
			pNeurons = pSynapses->m_pNextNeurons;
			delete pSynapses;
			pSynapses = pNeurons->m_pNextSynapses;
			delete pNeurons;
		}
	}
	m_cLayers        = 0;
	m_pInputNeurons  = NULL;
	m_pOutputNeurons = NULL;
	m_cbMemSize      = 0;

	return Status();
}

Bool Network::IsOK() const
{
	return (0 < m_cLayers);
}

UInt32 Network::GetLayersCount() const
{
	return m_cLayers;
}

const Neurons *Network::GetInputNeurons() const
{
	return m_pInputNeurons;
}

Neurons *Network::GetInputNeurons()
{
	return m_pInputNeurons;
}

const Neurons *Network::GetOutputNeurons() const
{
	return m_pOutputNeurons;
}

Neurons *Network::GetOutputNeurons()
{
	return m_pOutputNeurons;
}

Status Network::LoadWeightsAndBiases(const SynapsesDataVector *pVectorSynapsesData)
{
	if (0 == m_cLayers)
	{
		return Status(Status_NotInitialized, "Not initialized at {1}:{2}", __FILE__, __LINE__);
	}
	if (pVectorSynapsesData->size() != m_cLayers)
	{
		return Status(Status_InvalidArg, "Invalid arg at {1}:{2}", __FILE__, __LINE__);
	}

	Synapses *pSynapses = m_pInputNeurons->GetNextSynapses();

	for (auto iterSynapses = pVectorSynapsesData->begin(); iterSynapses != pVectorSynapsesData->end(); ++iterSynapses)
	{
		if (iterSynapses->vectorWeights.size() != pSynapses->GetWeightsCount())
		{
			return Status(Status_InvalidArg, "Invalid arg at {1}:{2}", __FILE__, __LINE__);
		}
		if (pSynapses->HasBias())
		{
			if (iterSynapses->vectorBiases.size() != pSynapses->GetBiasesCount())
			{
				return Status(Status_InvalidArg, "Invalid arg at {1}:{2}", __FILE__, __LINE__);
			}
		}
		memcpy(pSynapses->GetWeights(), iterSynapses->vectorWeights.data(), sizeof(Float) * pSynapses->GetWeightsCount());
		if (pSynapses->HasBias())
		{
			memcpy(pSynapses->GetBiases(), iterSynapses->vectorBiases.data(), sizeof(Float) * pSynapses->GetBiasesCount());
		}
		if (NULL != pSynapses->GetNextNeurons())
		{
			pSynapses = pSynapses->GetNextNeurons()->GetNextSynapses();
		}
	}

	return Status();
}

Size Network::GetMemSize() const
{
	return m_cbMemSize;
}

}//namespace NET

}//namespace N2
