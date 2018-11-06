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
#include "N2/NET/Synapses.hpp"


namespace N2
{

namespace SWST
{

class Network;
class Neurons;
class Synapses : public CE::ISynapses
{
public:

	Synapses(Network *pNetwork);

	~Synapses();

	virtual CX::Status Init(NET::Synapses *pSynapses);

	virtual CX::Status Uninit();

	virtual CX::Bool IsOK() const;

	virtual CX::UInt32 GetPrevNeuronsCount() const;

	virtual CX::UInt32 GetNextNeuronsCount() const;

	virtual CX::Bool HasBias() const;

	virtual CX::Float GetBias() const;

	virtual const NET::Synapses *GetSynapses() const;

	virtual NET::Synapses *GetSynapses();

	virtual const CE::INeurons *GetPrevNeurons() const;

	virtual CE::INeurons *GetPrevNeurons();

	virtual const CE::INeurons *GetNextNeurons() const;

	virtual CE::INeurons *GetNextNeurons();

	virtual CX::Status SyncToCE(CX::Bool bWait = CX::True);

	virtual CX::Status SyncFromCE(CX::Bool bWait = CX::True);

	virtual CX::Size GetMemSize() const;

protected:

	friend class Network;

	Network              *m_pNetwork;
	NET::Synapses        *m_pSynapses;
	CX::Float            *m_weights;
	CX::Float            *m_biases;
	Neurons              *m_pPrevNeurons;
	Neurons              *m_pNextNeurons;
	CX::Size             m_cbMemSize;

};

}//namespace SWST

}//namespace N2
