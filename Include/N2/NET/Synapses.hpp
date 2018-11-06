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


namespace N2
{

namespace NET
{

class Neurons;
class Synapses
{
public:

	Synapses();

	~Synapses();

	CX::Status Init(CX::UInt32 cPrevNeuronsCount, CX::UInt32 cNextNeuronsCount, CX::Bool bHasBias = CX::False, 
	                CX::Float fBias = 1.0f);

	CX::Status Uninit();

	CX::Bool IsOK() const;

	CX::UInt32 GetPrevNeuronsCount() const;

	CX::UInt32 GetNextNeuronsCount() const;

	CX::Bool HasBias() const;

	CX::Float GetBias() const;

	const CX::Float *GetWeights() const;

	CX::Float *GetWeights();

	const CX::Float *GetBiases() const;

	CX::Float *GetBiases();

	const Neurons *GetPrevNeurons() const;

	Neurons *GetPrevNeurons();

	const Neurons *GetNextNeurons() const;

	Neurons *GetNextNeurons();

	CX::Size GetMemSize() const;

protected:

	friend class Network;

	CX::UInt32           m_cPrevNeurons;
	CX::UInt32           m_cNextNeurons;
	CX::Bool             m_bHasBias;
	CX::Float            m_fBias;
	CX::Float            *m_weigths;
	CX::Float            *m_biases;
	Neurons              *m_pPrevNeurons;
	Neurons              *m_pNextNeurons;
	CX::Size             m_cbMemSize;

};

}//namespace NET

}//namespace N2
