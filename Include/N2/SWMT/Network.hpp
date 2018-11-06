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
#include "N2/CE/INetwork.hpp"
#include "N2/SWMT/Neurons.hpp"
#include "N2/SWMT/Synapses.hpp"
#include "N2/SWMT/IKernel.hpp"


namespace N2
{

namespace SWMT
{

class Provider;
class Network : public CE::INetwork
{
public:

	Network(Provider *pProvider);

	~Network();

	Provider *GetProvider();

	virtual CX::Status Init(NET::Network *pNetwork);

	virtual CX::Status Uninit();

	virtual CX::Bool IsOK() const;

	virtual CX::UInt32 GetLayersCount() const;

	virtual const NET::Network *GetNetwork() const;

	virtual NET::Network *GetNetwork();

	virtual const CE::INeurons *GetInputNeurons() const;

	virtual CE::INeurons *GetInputNeurons();

	virtual const CE::INeurons *GetOutputNeurons() const;

	virtual CE::INeurons *GetOutputNeurons();

	virtual CX::Status SyncToCE(CX::Bool bWait = CX::True, CX::UInt32 nSyncType = Sync_All);

	virtual CX::Status SyncFromCE(CX::Bool bWait = CX::True, CX::UInt32 nSyncType = Sync_All);

	virtual CX::Size GetMemSize() const;

	//assumes that weights are already transferred into device memory
	virtual CX::Status Evaluate(CX::UInt32 cCount, CX::Float *inputs, CX::Float *outputs);

protected:

	friend class Provider;

private:

	Provider           *m_pProvider;
	NET::Network       *m_pNetwork;
	Neurons            *m_pInputNeurons;
	Neurons            *m_pOutputNeurons;
	CX::Size           m_cbMemSize;

	class ComputeKernel : public IKernel
	{
	public:

		CX::Float    *prevNeurons;
		CX::UInt32   cPrevNeuronsOffset;
		CX::UInt32   cPrevNeuronsCount;
		CX::Float    *weights;
		CX::Float    *nextNeurons;
		CX::UInt32   cNextNeuronsOffset;
		CX::UInt32   cNextNeuronsCount;

		virtual void Run(CX::UInt32 cDims, const CX::UInt32 *dims, const CX::UInt32 *startIdxs, CX::UInt32 cCount)
		{
			CX::UInt32   idxs[1] = { startIdxs[0] };

			for (unsigned int i = 0; i < cCount; i++)
			{
				CX::Float   fValue = 0.0f;

				for (CX::UInt32 k = 0; k < cPrevNeuronsCount; k++)
				{
					fValue += prevNeurons[cPrevNeuronsOffset + k] * weights[k * cNextNeuronsCount + idxs[0]];
				}
				nextNeurons[cNextNeuronsOffset+ idxs[0]] = fValue;

				if (idxs[0] + 1 < dims[0])
				{
					idxs[0]++;
				}
				else
				{
					break;
				}
			}
		}

	};

	class ComputeWithBiasKernel : public IKernel
	{
	public:

		CX::Float    fBias;
		CX::Float    *prevNeurons;
		CX::UInt32   cPrevNeuronsOffset;
		CX::UInt32   cPrevNeuronsCount;
		CX::Float    *weights;
		CX::Float    *biases;
		CX::Float    *nextNeurons;
		CX::UInt32   cNextNeuronsOffset;
		CX::UInt32   cNextNeuronsCount;

		virtual void Run(CX::UInt32 cDims, const CX::UInt32 *dims, const CX::UInt32 *startIdxs, CX::UInt32 cCount)
		{
			CX::UInt32   idxs[1] = { startIdxs[0] };

			for (CX::UInt32 i = 0; i < cCount; i++)
			{
				CX::Float   fValue = 0.0f;

				for (CX::UInt32 k = 0; k < cPrevNeuronsCount; k++)
				{
					fValue += prevNeurons[cPrevNeuronsOffset + k] * weights[k * cNextNeuronsCount + idxs[0]];
				}
				fValue += fBias * biases[idxs[0]];
				nextNeurons[cNextNeuronsOffset+ idxs[0]] = fValue;

				if (idxs[0] + 1 < dims[0])
				{
					idxs[0]++;
				}
				else
				{
					break;
				}
			}
		}

	};

	class ActivateKernel : public IKernel
	{
	public:

		CX::Float             *nextNeurons;
		CX::UInt32            cNextNeuronsOffset;
		CX::UInt32            cNextNeuronsCount;
		NET::ActivationType   nActivation;
		CX::UInt32            cActivationArgs;
		const CX::Float       *activationArgs;

		virtual void Run(CX::UInt32 cDims, const CX::UInt32 *dims, const CX::UInt32 *startIdxs, CX::UInt32 cCount)
		{
			CX::UInt32   idxs[1] = { startIdxs[0] };

			for (CX::UInt32 i = 0; i < cCount; i++)
			{
				switch (nActivation)
				{
					case NET::Activation::Identity : break;
					case NET::Activation::Sigmoid : 
					{
						CX::Float   fValue = nextNeurons[cNextNeuronsOffset + idxs[0]];

						nextNeurons[cNextNeuronsOffset + idxs[0]] = 1.0f / (1.0f + exp(-fValue));
					}
					break;
					case NET::Activation::BinaryStep :
					{
						CX::Float   fValue = nextNeurons[cNextNeuronsOffset + idxs[0]];

						if (0.0f > fValue)
						{
							nextNeurons[cNextNeuronsOffset + idxs[0]] = 0.0f;
						}
						else
						{
							nextNeurons[cNextNeuronsOffset + idxs[0]] = 1.0f;
						}
					}
					break;
					case NET::Activation::TanH :
					{
						CX::Float   fValue = nextNeurons[cNextNeuronsOffset + idxs[0]];

						nextNeurons[cNextNeuronsOffset + idxs[0]] = tanh(fValue);
					}
					break;
					case NET::Activation::ArcTan :
					{
						CX::Float   fValue = nextNeurons[cNextNeuronsOffset + idxs[0]];

						nextNeurons[cNextNeuronsOffset + idxs[0]] = atan(fValue);
					}
					break;;
					case NET::Activation::SoftSign :
					{
						CX::Float   fValue = nextNeurons[cNextNeuronsOffset + idxs[0]];

						if (0.0f > fValue)
						{
							nextNeurons[cNextNeuronsOffset + idxs[0]] = fValue / (1.0f + -fValue);
						}
						else
						{
							nextNeurons[cNextNeuronsOffset + idxs[0]] = fValue / (1.0f + fValue);
						}
					}
					break;
					case NET::Activation::RELU :
					{
						CX::Float   fValue = nextNeurons[cNextNeuronsOffset + idxs[0]];

						if (0.0f > fValue)
						{
							nextNeurons[cNextNeuronsOffset + idxs[0]] = 0.0f;
						}
						else
						{
							nextNeurons[cNextNeuronsOffset + idxs[0]] = fValue;
						}
					}
					break;
					case NET::Activation::LeakyRELU :
					{
					}
					break;
					case NET::Activation::SoftPlus :
					{
					}
					break;
					case NET::Activation::BentIdentity :
					{
					}
					break;
					case NET::Activation::Sinusoid :
					{
					}
					break;
					case NET::Activation::SINC :
					{
					}
					break;
					case NET::Activation::Gaussian :
					{
					}
					break;
					case NET::Activation::ISRU :
					{
					}
					break;
					case NET::Activation::PRELU :
					{
					}
					break;
					case NET::Activation::ELU :
					{
					}
					break;
					case NET::Activation::SELU :
					{
					}
					break;
					case NET::Activation::SRELU :
					{
					}
					break;
					case NET::Activation::ISRLU :
					{
					}
					break;
					case NET::Activation::SoftExponential :
					{
					}
					break;
					case NET::Activation::SoftMax :
					{
					}
					break;
				}
	
				if (idxs[0] + 1 < dims[0])
				{
					idxs[0]++;
				}
				else
				{
					break;
				}
			}
		}

	};

	CX::Status Compute(CX::Float *prevNeurons, CX::UInt32 cPrevNeuronsOffset, CX::UInt32 cPrevNeuronsCount,
	                   CX::Float *weights, 
	                   CX::Float *nextNeurons, CX::UInt32 cNextNeuronsOffset, CX::UInt32 cNextNeuronsCount);

	CX::Status ComputeWithBias(CX::Float fBias, 
	                           CX::Float *prevNeurons, CX::UInt32 cPrevNeuronsOffset, CX::UInt32 cPrevNeuronsCount,
	                           CX::Float *weights, CX::Float *biases, 
	                           CX::Float *nextNeurons, CX::UInt32 cNextNeuronsOffset, CX::UInt32 cNextNeuronsCount);

	CX::Status Activate(CX::Float *nextNeurons, CX::UInt32 cNextNeuronsOffset, CX::UInt32 cNextNeuronsCount, 
	                    NET::ActivationType nActivation, CX::UInt32 cActivationArgs, const CX::Float *activationArgs);

};

}//namespace SWMT

}//namespace N2
