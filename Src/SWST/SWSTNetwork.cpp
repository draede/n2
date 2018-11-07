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

#include "N2/SWST/Network.hpp"
#include "N2/SWST/Provider.hpp"


using namespace CX;


namespace N2
{

namespace SWST
{

Network::Network(Provider *pProvider)
{
	m_pProvider      = pProvider;
	m_pNetwork       = NULL;
	m_pInputNeurons  = NULL;
	m_pOutputNeurons = NULL;
	m_cbMemSize      = 0;
}

Network::~Network()
{
	Uninit();
}

Provider *Network::GetProvider()
{
	return m_pProvider;
}

Status Network::Init(NET::Network *pNetwork)
{
	Uninit();

	NET::Neurons    *pNETNeurons;
	NET::Synapses   *pNETSynapses;
	Neurons         *pNeurons;
	Synapses        *pSynapses;
	Status          status;

	m_cbMemSize = sizeof(Network);
	for (;;)
	{
		pNETNeurons = pNetwork->GetInputNeurons();

		if (NULL == (pNeurons = new (std::nothrow) Neurons(this)))
		{
			status = Status(Status_MemAllocFailed, "Failed to allocate neurons at {1}:{2}", __FILE__, __LINE__);

			break;
		}
		if (!(status = pNeurons->Init(pNETNeurons)))
		{
			delete pNeurons;

			break;
		}
		pNeurons->m_pPrevSynapses = NULL;
		pNeurons->m_pNextSynapses = NULL;
		m_pInputNeurons           = pNeurons;
		m_pOutputNeurons          = pNeurons;
		m_cbMemSize += pNeurons->GetMemSize();

		pNETSynapses = pNETNeurons->GetNextSynapses();

		while (NULL != pNETSynapses)
		{
			pNETNeurons = pNETSynapses->GetNextNeurons();
			if (NULL == (pSynapses = new (std::nothrow) Synapses(this)))
			{
				status = Status(Status_MemAllocFailed, "Failed to allocate synapses at {1}:{2}", __FILE__, __LINE__);

				break;
			}
			if (!(status = pSynapses->Init(pNETSynapses)))
			{
				delete pSynapses;

				break;
			}
			if (NULL == (pNeurons = new (std::nothrow) Neurons(this)))
			{
				status = Status(Status_MemAllocFailed, "Failed to allocate neurons at {1}:{2}", __FILE__, __LINE__);

				break;
			}
			if (!(status = pNeurons->Init(pNETNeurons)))
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
		
			pNETSynapses = pNETNeurons->GetNextSynapses();
		}
		if (!status)
		{
			break;
		}
		m_pNetwork      = pNetwork;

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

	m_pNetwork       = NULL;
	m_pInputNeurons  = NULL;
	m_pOutputNeurons = NULL;
	m_cbMemSize      = 0;

	return Status();
}

Bool Network::IsOK() const
{
	return (NULL != m_pNetwork);
}

UInt32 Network::GetLayersCount() const
{
	if (NULL == m_pNetwork)
	{
		return 0;
	}

	return m_pNetwork->GetLayersCount();
}

const NET::Network *Network::GetNetwork() const
{
	return m_pNetwork;
}

NET::Network *Network::GetNetwork()
{
	return m_pNetwork;
}

const CE::INeurons *Network::GetInputNeurons() const
{
	return m_pInputNeurons;
}

CE::INeurons *Network::GetInputNeurons()
{
	return m_pInputNeurons;
}

const CE::INeurons *Network::GetOutputNeurons() const
{
	return m_pOutputNeurons;
}

CE::INeurons *Network::GetOutputNeurons()
{
	return m_pOutputNeurons;
}

Status Network::SyncToCE(Bool bWait/* = True*/, UInt32 nSyncType/* = Sync_All*/)
{
	if (NULL == m_pNetwork)
	{
		return Status(Status_NotInitialized, "Not initialized at {1}:{2}", __FILE__, __LINE__);
	}

	CX_UNUSED(bWait);

	Synapses   *pSynapses;
	Neurons    *pNeurons;

	pNeurons = m_pInputNeurons;
	while (NULL != pNeurons)
	{
		if (Sync_Neurons == (nSyncType & Sync_Neurons))
		{
			pNeurons->SyncToCE(True);
		}
		pSynapses = pNeurons->m_pNextSynapses;
		if (NULL != pSynapses)
		{
			break;
		}
		else
		{
			if (Sync_Synapse == (nSyncType & Sync_Synapse))
			{
				pSynapses ->SyncToCE(True);
			}
			pNeurons = pSynapses->m_pNextNeurons;
		}
	}

	return Status();
}

Status Network::SyncFromCE(Bool bWait/* = True*/, UInt32 nSyncType/* = Sync_All*/)
{
	if (NULL == m_pNetwork)
	{
		return Status(Status_NotInitialized, "Not initialized at {1}:{2}", __FILE__, __LINE__);
	}

	CX_UNUSED(bWait);

	Synapses   *pSynapses;
	Neurons    *pNeurons;

	pNeurons = m_pInputNeurons;
	while (NULL != pNeurons)
	{
		if (Sync_Neurons == (nSyncType & Sync_Neurons))
		{
			pNeurons->SyncFromCE(True);
		}
		pSynapses = pNeurons->m_pNextSynapses;
		if (NULL == pSynapses)
		{
			break;
		}
		else
		{
			if (Sync_Synapse == (nSyncType & Sync_Synapse))
			{
				pSynapses ->SyncFromCE(True);
			}
			pNeurons = pSynapses->m_pNextNeurons;
		}
	}

	return Status();
}

Size Network::GetMemSize() const
{
	return m_cbMemSize;
}

//assumes that weights are already transferred into device memory
Status Network::Evaluate(UInt32 cCount, Float *inputs, Float *outputs)
{
	if (0 == m_pNetwork)
	{
		return Status(Status_NotInitialized, "Not initialized at {1}:{2}", __FILE__, __LINE__);
	}
	if (0 == cCount)
	{
		return Status(Status_InvalidArg, "Invalid arg at {1}:{2}", __FILE__, __LINE__);
	}

	Synapses           *pSynapses;
	Float              *prevNeurons;
	UInt32             cPrevNeuronsOffset;
	Float              *nextNeurons;
	UInt32             cNextNeuronsOffset;
	UInt32             cInputsOffset;
	UInt32             cOutputsOffset;
	Status             status;

	cInputsOffset  = 0;
	cOutputsOffset = 0;
	for (UInt32 i = 0; i < cCount; i++)
	{
		pSynapses = m_pInputNeurons->m_pNextSynapses;
		while (NULL != pSynapses)
		{
			if (pSynapses->m_pPrevNeurons == m_pInputNeurons)
			{
				prevNeurons        = inputs;
				cPrevNeuronsOffset = cInputsOffset;
			}
			else
			{
				prevNeurons        = pSynapses->m_pPrevNeurons->m_values;
				cPrevNeuronsOffset = 0;
			}
			if (pSynapses->m_pNextNeurons == m_pOutputNeurons)
			{
				nextNeurons        = outputs;
				cNextNeuronsOffset = cOutputsOffset;
			}
			else
			{
				nextNeurons        = pSynapses->m_pNextNeurons->m_values;
				cNextNeuronsOffset = 0;
			}
			if (!pSynapses->HasBias())
			{
				if (!(status = Compute(prevNeurons, cPrevNeuronsOffset, pSynapses->m_pPrevNeurons->GetNeuronsCount(), 
				                       pSynapses->m_weights, 
				                       nextNeurons, cNextNeuronsOffset, pSynapses->m_pNextNeurons->GetNeuronsCount())))
				{
					break;
				}
			}
			else
			{
				if (!(status = ComputeWithBias(pSynapses->GetBias(), 
				                            prevNeurons, cPrevNeuronsOffset, pSynapses->m_pPrevNeurons->GetNeuronsCount(), 
				                            pSynapses->m_weights, pSynapses->m_biases, 
				                            nextNeurons, cNextNeuronsOffset, pSynapses->m_pNextNeurons->GetNeuronsCount())))
				{
					break;
				}
			}

			if (!(status = Activate(nextNeurons, cNextNeuronsOffset, pSynapses->m_pNextNeurons->GetNeuronsCount(), 
			                        pSynapses->m_pNextNeurons->GetActivation(), 
			                        pSynapses->m_pNextNeurons->GetActivationArgsCount(), 
			                        pSynapses->m_pNextNeurons->GetActivationArgs())))
			{
				break;
			}

			pSynapses = pSynapses->m_pNextNeurons->m_pNextSynapses;
		}
		if (!status)
		{
			break;
		}
		cInputsOffset += m_pInputNeurons->GetNeuronsCount();
		cOutputsOffset += m_pOutputNeurons->GetNeuronsCount();
	}
	if (!status)
	{
		return status;
	}

	return Status();
}

Status Network::Compute(Float *prevNeurons, UInt32 cPrevNeuronsOffset, UInt32 cPrevNeuronsCount,
                        Float *weights, 
                        Float *nextNeurons, UInt32 cNextNeuronsOffset, UInt32 cNextNeuronsCount)
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNextNeuronsCount; idx++)
	{
		fValue = 0.0f;
		for (UInt32 k = 0; k < cPrevNeuronsCount; k++)
		{
			fValue += prevNeurons[cPrevNeuronsOffset + k] * weights[k * cNextNeuronsCount + idx];
		}
		nextNeurons[cNextNeuronsOffset+ idx] = fValue;
	}

	return Status();
}

Status Network::ComputeWithBias(Float fBias, 
                                Float *prevNeurons, UInt32 cPrevNeuronsOffset, UInt32 cPrevNeuronsCount,
                                Float *weights, Float *biases, 
                                Float *nextNeurons, UInt32 cNextNeuronsOffset, UInt32 cNextNeuronsCount)
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNextNeuronsCount; idx++)
	{
		fValue = 0.0f;
		for (UInt32 k = 0; k < cPrevNeuronsCount; k++)
		{
			fValue += prevNeurons[cPrevNeuronsOffset + k] * weights[k * cNextNeuronsCount + idx];
		}
		fValue += fBias * biases[idx];
		nextNeurons[cNextNeuronsOffset+ idx] = fValue;
	}

	return Status();
}

Status Network::Activate(Float *nextNeurons, UInt32 cNextNeuronsOffset, UInt32 cNextNeuronsCount, 
                         NET::ActivationType nActivation, UInt32 cActivationArgs, const Float *activationArgs)
{
	if (NET::Activation::Identity == nActivation)
	{
		return Status();
	}

	switch (nActivation)
	{
		case NET::Activation::Identity        : break;
		case NET::Activation::Sigmoid         : ActivateSigmoid(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount); break;
		case NET::Activation::BinaryStep      : ActivateBinaryStep(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount); break;
		case NET::Activation::TanH            : ActivateTanH(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount); break;
		case NET::Activation::ArcTan          : ActivateArcTan(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount); break;
		case NET::Activation::SoftSign        : ActivateSoftSign(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount); break;;
		case NET::Activation::RELU            : ActivateRELU(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount); break;
		case NET::Activation::LeakyRELU       : ActivateLeakyRELU(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount); break;
		case NET::Activation::SoftPlus        : ActivateSoftPlus(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount); break;
		case NET::Activation::BentIdentity    : ActivateBentIdentity(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount); break;
		case NET::Activation::Sinusoid        : ActivateSinusoid(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount); break;
		case NET::Activation::SINC            : ActivateSINC(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount); break;
		case NET::Activation::Gaussian        : ActivateGaussian(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount); break;
		case NET::Activation::ISRU            : ActivateISRU(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount, activationArgs[0]); break;
		case NET::Activation::PRELU           : ActivatePRELU(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount, activationArgs[0]); break;
		case NET::Activation::ELU             : ActivateELU(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount, activationArgs[0]); break;
		case NET::Activation::SELU            : ActivateSELU(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount, activationArgs[0], activationArgs[1]); break;
		case NET::Activation::SRELU           : ActivateSRELU(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount, activationArgs[0], activationArgs[1], activationArgs[2], activationArgs[3]); break;
		case NET::Activation::ISRLU           : ActivateISRLU(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount, activationArgs[0]); break;
		case NET::Activation::SoftExponential : ActivateSoftExponential(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount, activationArgs[0]); break;
		case NET::Activation::SoftMax         : ActivateSoftMax(nextNeurons, cNextNeuronsOffset, cNextNeuronsCount, activationArgs[0]); break;
	}

	return Status();
}

void Network::ActivateSigmoid(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		neurons[cNeuronsOffset + idx] = 1.0f / (1.0f + exp(-fValue));
	}
}

void Network::ActivateBinaryStep(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		if (0.0f > fValue)
		{
			neurons[cNeuronsOffset + idx] = 0.0f;
		}
		else
		{
			neurons[cNeuronsOffset + idx] = 1.0f;
		}
	}
}

void Network::ActivateTanH(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		neurons[cNeuronsOffset + idx] = tanh(fValue);
	}
}

void Network::ActivateArcTan(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		neurons[cNeuronsOffset + idx] = atan(fValue);
	}
}

void Network::ActivateSoftSign(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		if (0.0f > fValue)
		{
			neurons[cNeuronsOffset + idx] = fValue / (1.0f + -fValue);
		}
		else
		{
			neurons[cNeuronsOffset + idx] = fValue / (1.0f + fValue);
		}
	}
}

void Network::ActivateISRU(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount, Float fAlpha) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		neurons[cNeuronsOffset + idx] = fValue / sqrt(1.0f + fAlpha * fValue * fValue);
	}
}

void Network::ActivateRELU(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		if (0.0f > fValue)
		{
			neurons[cNeuronsOffset + idx] = 0.0f;
		}
		else
		{
			neurons[cNeuronsOffset + idx] = fValue;
		}
	}
}

void Network::ActivateLeakyRELU(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		neurons[cNeuronsOffset + idx] = 0.01f * fValue;
	}
}

void Network::ActivatePRELU(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount, Float fAlpha) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		if (0.0f > fValue)
		{
			neurons[cNeuronsOffset + idx] = fAlpha * fValue;
		}
		else
		{
			neurons[cNeuronsOffset + idx] = fValue;
		}
	}
}

void Network::ActivateELU(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount, Float fAlpha) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		if (0.0f > fValue)
		{
			neurons[cNeuronsOffset + idx] = fAlpha * (exp(fValue) - 1.0f);
		}
		else
		{
			neurons[cNeuronsOffset + idx] = fValue;
		}
	}
}

void Network::ActivateSELU(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount, Float fAlpha, Float fLambda) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		if (0.0f > fValue)
		{
			neurons[cNeuronsOffset + idx] = fLambda * fAlpha * (exp(fValue) - 1.0f);
		}
		else
		{
			neurons[cNeuronsOffset + idx] = fLambda * fValue;
		}
	}
}

void Network::ActivateSRELU(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount, Float fTl, Float fAl, 
                            Float fTr, Float fAr) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		if (fValue <= fTl)
		{
			neurons[cNeuronsOffset + idx] = fTl + fAl * (fValue - fTl);
		}
		else
		if (fValue >= fTr)
		{
			neurons[cNeuronsOffset + idx] = fTr + fAr * (fValue - fTr);
		}
		else
		{
			neurons[cNeuronsOffset + idx] = fValue;
		}
	}
}

void Network::ActivateISRLU(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount, Float fAlpha) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		if (0.0f > fValue)
		{
			neurons[cNeuronsOffset + idx] = fValue / sqrt(1.0f + fAlpha * fValue * fValue);
		}
		else
		{
			neurons[cNeuronsOffset + idx] = fValue;
		}
	}
}

void Network::ActivateSoftPlus(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		neurons[cNeuronsOffset + idx] = log(1.0f + fValue);
	}
}

void Network::ActivateBentIdentity(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		neurons[cNeuronsOffset + idx] = (sqrt(fValue * fValue + 1.0f) - 1.0f) / 2.0f + fValue;
	}
}

void Network::ActivateSoftExponential(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount, Float fAlpha) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		if (0.0f > fAlpha)
		{
			neurons[cNeuronsOffset + idx] = -log(1.0f - fAlpha * (fValue + fAlpha)) / fAlpha;
		}
		else
		if (0.0f < fAlpha)
		{
			neurons[cNeuronsOffset + idx] = (exp(fAlpha * fValue) - 1.0f) / fAlpha + fAlpha;
		}
		else
		{
			neurons[cNeuronsOffset + idx] = fValue;
		}
	}
}

void Network::ActivateSinusoid(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		neurons[cNeuronsOffset + idx] = sin(fValue);
	}
}

void Network::ActivateSINC(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		if (0.0f == fValue)
		{
			neurons[cNeuronsOffset + idx] = 1.0f;
		}
		else
		{
			neurons[cNeuronsOffset + idx] = sin(fValue) / fValue;
		}
	}
}

void Network::ActivateGaussian(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		neurons[cNeuronsOffset + idx] = exp(- fValue * fValue);
	}
}

void Network::ActivateSoftMax(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount, Float fExpSum) 
{
	Float   fValue;

	for (UInt32 idx = 0; idx < cNeuronsCount; idx++)
	{
		fValue = neurons[cNeuronsOffset + idx];

		neurons[cNeuronsOffset + idx] = exp(fValue) / fExpSum;
	}
}

void Network::ActivateSoftMaxExpSum(Float *neurons, UInt32 cNeuronsOffset, UInt32 cNeuronsCount, Float *expsum) 
{
	float   fExpSum = 0.0f;

	for (UInt32 i = 0; i < cNeuronsCount; i++)
	{
		fExpSum += exp(neurons[cNeuronsOffset + i]);
	}
	expsum[0] = fExpSum;
}

}//namespace SWST

}//namespace N2
