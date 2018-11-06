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
#include "N2/CE/INeurons.hpp"
#include "N2/CE/ISynapses.hpp"
#include "N2/NET/Neurons.hpp"
#include "N2/CL/OpenCL.hpp"


namespace N2
{

namespace CL
{

class Network;
class Synapses;
class Neurons : public CE::INeurons
{
public:

	Neurons(Network *pNetwork);

	~Neurons();

	virtual CX::Status Init(NET::Neurons *pNeurons);

	virtual CX::Status Uninit();

	virtual CX::Bool IsOK() const;

	virtual CX::UInt32 GetNeuronsCount() const;

	virtual NET::ActivationType GetActivation() const;

	virtual CX::UInt32 GetActivationArgsCount() const;

	virtual const CX::Float *GetActivationArgs() const;

	virtual const NET::Neurons *GetNeurons() const;

	virtual NET::Neurons *GetNeurons();

	virtual const CE::ISynapses *GetPrevSynapses() const;

	virtual CE::ISynapses *GetPrevSynapses();

	virtual const CE::ISynapses *GetNextSynapses() const;

	virtual CE::ISynapses *GetNextSynapses();

	virtual CX::Status SyncToCE(CX::Bool bWait = CX::True);

	virtual CX::Status SyncFromCE(CX::Bool bWait = CX::True);

	virtual CX::Size GetMemSize() const;

	const CX::Char *GetActivationFunction() const;

protected:

	friend class Network;

	Network               *m_pNetwork;
	NET::Neurons          *m_pNeurons;
	Synapses              *m_pPrevSynapses;
	Synapses              *m_pNextSynapses;
	cl::Buffer            m_values;
	CX::Size              m_cbMemSize;
	const CX::Char       *m_szActivationFunction;

};

}//namespace CL

}//namespace N2
