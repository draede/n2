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
#include "N2/NET/Network.hpp"


namespace N2
{

namespace CE
{

class INetwork
{
public:

	enum SyncType
	{
		Sync_Neurons = 1,
		Sync_Synapse = 2,
		Sync_All     = 3,
	};

	virtual CX::Status Init(NET::Network *pNetwork) = 0;

	virtual CX::Status Uninit() = 0;

	virtual CX::Bool IsOK() const = 0;

	virtual CX::UInt32 GetLayersCount() const = 0;

	virtual const NET::Network *GetNetwork() const = 0;

	virtual NET::Network *GetNetwork() = 0;

	virtual const INeurons *GetInputNeurons() const = 0;

	virtual INeurons *GetInputNeurons() = 0;

	virtual const INeurons *GetOutputNeurons() const = 0;

	virtual INeurons *GetOutputNeurons() = 0;

	virtual CX::Status SyncToCE(CX::Bool bWait = CX::True, CX::UInt32 nSyncType = Sync_All) = 0;

	virtual CX::Status SyncFromCE(CX::Bool bWait = CX::True, CX::UInt32 nSyncType = Sync_All) = 0;

	virtual CX::Size GetMemSize() const = 0;

	//assumes that weights are already transferred into device memory
	virtual CX::Status Evaluate(CX::UInt32 cCount, CX::Float *inputs, CX::Float *outputs) = 0;

protected:

	virtual ~INetwork() { }

};

}//namespace CE

}//namespace N2
