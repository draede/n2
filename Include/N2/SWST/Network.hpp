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

#pragma once


#include "CX/Types.hpp"
#include "CX/Status.hpp"
#include "N2/CE/INetwork.hpp"
#include "N2/SWST/Neurons.hpp"
#include "N2/SWST/Synapses.hpp"


namespace N2
{

namespace SWST
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

	CX::Status Compute(CX::Float *prevNeurons, CX::UInt32 cPrevNeuronsOffset, CX::UInt32 cPrevNeuronsCount,
	                   CX::Float *weights, 
	                   CX::Float *nextNeurons, CX::UInt32 cNextNeuronsOffset, CX::UInt32 cNextNeuronsCount);

	CX::Status ComputeWithBias(CX::Float fBias, 
	                           CX::Float *prevNeurons, CX::UInt32 cPrevNeuronsOffset, CX::UInt32 cPrevNeuronsCount,
	                           CX::Float *weights, CX::Float *biases, 
	                           CX::Float *nextNeurons, CX::UInt32 cNextNeuronsOffset, CX::UInt32 cNextNeuronsCount);

	CX::Status Activate(CX::Float *nextNeurons, CX::UInt32 cNextNeuronsOffset, CX::UInt32 cNextNeuronsCount, 
	                    NET::ActivationType nActivation, CX::UInt32 cActivationArgs, const CX::Float *activationArgs);

	static void ActivateSigmoid(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount);

	static void ActivateBinaryStep(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount);

	static void ActivateTanH(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount);

	static void ActivateArcTan(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount);

	static void ActivateSoftSign(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount);

	static void ActivateISRU(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount, CX::Float fAlpha);

	static void ActivateRELU(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount);

	static void ActivateLeakyRELU(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount);

	static void ActivatePRELU(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount, CX::Float fAlpha);

	static void ActivateELU(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount, CX::Float fAlpha);

	static void ActivateSELU(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount, CX::Float fAlpha, 
	                         CX::Float fLambda);

	static void ActivateSRELU(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount, CX::Float fTl, 
	                          CX::Float fAl, 
	                          CX::Float fTr, CX::Float fAr);

	static void ActivateISRLU(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount, CX::Float fAlpha);

	static void ActivateSoftPlus(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount);

	static void ActivateBentIdentity(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount);

	static void ActivateSoftExponential(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount, 
	                                    CX::Float fAlpha);

	static void ActivateSinusoid(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount);

	static void ActivateSINC(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount);

	static void ActivateGaussian(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount);

	static void ActivateSoftMax(CX::Float *neurons, CX::UInt32 cNeuronsOffset, 
	                            CX::UInt32 cNeuronsCount, CX::Float fExpSum);

	static void ActivateSoftMaxExpSum(CX::Float *neurons, CX::UInt32 cNeuronsOffset, CX::UInt32 cNeuronsCount, 
	                                  CX::Float *expsum);

};

}//namespace SWST

}//namespace N2
