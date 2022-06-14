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
#include "N2/NET/Activation.hpp"


namespace N2
{

namespace NET
{

class Synapses;
class Neurons
{
public:

	static const CX::UInt32 MIN_NEURONS               = 1;
	static const CX::UInt32 MAX_NEURONS               = 65536;
	static const CX::UInt32 MAX_ACTIVATION_ARGS_COUNT = 10;

	Neurons();

	~Neurons();

	CX::Status Init(CX::UInt32 cNeuronsCount, ActivationType nActivation = Activation::Identity,
	                CX::UInt32 cActivationArgs = 0, const CX::Float *activationArgs = NULL);

	CX::Status Uninit();

	CX::Bool IsOK() const;

	CX::UInt32 GetNeuronsCount() const;

	const CX::Float *GetValues() const;

	CX::Float *GetValues();

	ActivationType GetActivation() const;

	CX::UInt32 GetActivationArgsCount() const;

	const CX::Float *GetActivationArgs() const;

	const Synapses *GetPrevSynapses() const;

	Synapses *GetPrevSynapses();

	const Synapses *GetNextSynapses() const;

	Synapses *GetNextSynapses();

	CX::Size GetMemSize() const;

protected:

	friend class Network;

	CX::UInt32           m_cNeurons;
	CX::Float            *m_values;
	ActivationType       m_nActivation;
	CX::UInt32           m_cActivationArgs;
	CX::Float            m_activationArgs[MAX_ACTIVATION_ARGS_COUNT];
	Synapses             *m_pPrevSynapses;
	Synapses             *m_pNextSynapses;
	CX::Size             m_cbMemSize;

};

}//namespace NET

}//namespace N2
