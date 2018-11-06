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
#include "N2/NET/Synapses.hpp"
#include "N2/NET/Synapses.hpp"


namespace N2
{

namespace CE
{

class INeurons;
class ISynapses
{
public:

	virtual CX::Status Init(NET::Synapses *pSynapses) = 0;

	virtual CX::Status Uninit() = 0;

	virtual CX::UInt32 GetPrevNeuronsCount() const = 0;

	virtual CX::UInt32 GetNextNeuronsCount() const = 0;

	virtual CX::Bool HasBias() const = 0;

	virtual CX::Float GetBias() const = 0;

	virtual const NET::Synapses *GetSynapses() const = 0;

	virtual NET::Synapses *GetSynapses() = 0;

	virtual const INeurons *GetPrevNeurons() const = 0;

	virtual INeurons *GetPrevNeurons() = 0;

	virtual const INeurons *GetNextNeurons() const = 0;

	virtual INeurons *GetNextNeurons() = 0;

	virtual CX::Status SyncToCE(CX::Bool bWait = CX::True) = 0;

	virtual CX::Status SyncFromCE(CX::Bool bWait = CX::True) = 0;

	virtual CX::Size GetMemSize() const = 0;

protected:

	virtual ~ISynapses() { }

};

}//namespace CE

}//namespace N2
