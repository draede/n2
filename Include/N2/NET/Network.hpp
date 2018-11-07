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
#include "CX/Print.hpp"
#include "CX/Str/StringHelper.hpp"
#include "CX/Vector.hpp"
#include "N2/NET/Activation.hpp"
#include "N2/NET/Layer.hpp"
#include "N2/NET/Neurons.hpp"
#include "N2/NET/Synapses.hpp"


namespace N2
{

namespace NET
{

class Network
{
public:

	static const CX::UInt32   MIN_LAYERS = 2;

	typedef CX::Vector<CX::Float>::Type       FloatsVector;

	struct SynapsesData
	{
		FloatsVector   vectorWeights;
		FloatsVector   vectorBiases;
	};

	typedef CX::Vector<SynapsesData>::Type    SynapsesDataVector;

	Network();

	~Network();

	CX::Status Init(CX::UInt32 cInputNeuronsCount, CX::UInt32 cLayersCount, const Layer *layers);

	CX::Status Uninit();

	CX::Bool IsOK() const;

	CX::UInt32 GetLayersCount() const;

	const Neurons *GetInputNeurons() const;

	Neurons *GetInputNeurons();

	const Neurons *GetOutputNeurons() const;

	Neurons *GetOutputNeurons();

	CX::Status LoadWeightsAndBiases(const SynapsesDataVector *pVectorSynapsesData);

	CX::Size GetMemSize() const;

	template <typename OUTPUT>
	void Dump(OUTPUT out) const
	{
		if (0 == m_cLayers)
		{
			return;
		}

		CX::String   sMemSize;

		CX::Str::StringHelper::GetNiceSize(m_cbMemSize, &sMemSize);

		CX::Print(out, "Network:\n");
		CX::Print(out, "  Layers: {1}\n", m_cLayers);
		CX::Print(out, "  Memory: {1}\n", sMemSize);

		const Neurons     *pNeurons;
		const Synapses    *pSynapses;
		CX::Size          cLayer;
		CX::UInt32        cCount;
		const CX::Float   *values;

		cLayer   = 0;
		pNeurons = m_pInputNeurons;
		while (NULL!= pNeurons)
		{
			cLayer++;
			
			cCount = pNeurons->GetNeuronsCount();
			CX::Print(out, "Layer {1}:\n", cLayer);
			CX::Print(out, "  Neurons: {1}:\n", cCount);
			CX::Print(out, "    Activation: {1}:\n", pNeurons->GetActivation());
			cCount = pNeurons->GetActivationArgsCount();
			CX::Print(out, "    Activation args: {1}\n", cCount);
			if (0 < cCount)
			{
				values = pNeurons->GetActivationArgs();
				CX::Print(out, "     ");
				for (CX::UInt32 i = 0; i < cCount; i++)
				{
					CX::Print(out, " {1:.6}", values[i]);
				}
				CX::Print(out, "\n");
			}
			cCount = pNeurons->GetNeuronsCount();
			values = pNeurons->GetValues();
			if (20 >= cCount)
			{
				CX::Print(out, "    Values:\n", cCount);
				CX::Print(out, "     ");
				for (CX::UInt32 i = 0; i < cCount; i++)
				{
					CX::Print(out, " {1:.6}", values[i]);
				}
				CX::Print(out, "\n");
			}

			pSynapses = pNeurons->GetNextSynapses();
			if (NULL == pSynapses)
			{
				break;
			}
			CX::Print(out, "  Synapses\n");
			if (pSynapses->HasBias())
			{
				CX::Print(out, "    Bias: {1:.6}\n", pSynapses->GetBias());
			}
			CX::Print(out, "    Weights: {1} x {2}\n", pSynapses->GetPrevNeuronsCount(), pSynapses->GetNextNeuronsCount());
			values = pSynapses->GetWeights();
			if (50 >= pSynapses->GetWeightsCount())
			{
				for (CX::UInt32 i = 0; i < pSynapses->GetPrevNeuronsCount(); i++)
				{
					CX::Print(out, "   ");
					for (CX::UInt32 j = 0; j < pSynapses->GetNextNeuronsCount(); j++)
					{
						CX::Print(out, " {1:.6}", values[i * pSynapses->GetNextNeuronsCount() + j]);
					}
					CX::Print(out, "\n");
				}
			}
			if (pSynapses->HasBias())
			{
				values = pSynapses->GetBiases();
				CX::Print(out, "    Biases: 1 x {1}\n", pSynapses->GetNextNeuronsCount());
				CX::Print(out, "     ");
				if (20 >= pSynapses->GetBiasesCount())
				{
					for (CX::UInt32 i = 0; i < pSynapses->GetBiasesCount(); i++)
					{
						CX::Print(out, " {1:.6}", values[i]);
					}
					CX::Print(out, "\n");
				}
			}
			pNeurons = pSynapses->GetNextNeurons();
		}
	}

private:

	CX::UInt32           m_cLayers;
	Neurons              *m_pInputNeurons;
	Neurons              *m_pOutputNeurons;
	CX::Size             m_cbMemSize;
};

}//namespace NET

}//namespace N2
