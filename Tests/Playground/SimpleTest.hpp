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
#include "CX/Util/Timer.hpp"
#include "N2/NET/Network.hpp"
#include "N2/CL/Provider.hpp"


class SimpleTest
{
public:

	static void Run(N2::CE::IProvider *pProvider)
	{
		static const CX::UInt32       INPUTS_COUNT = 2;
		static const N2::NET::Layer   LAYERS[]     = 
		{
			{ 3, N2::NET::Activation::Sigmoid,  0, { 0.0f }, CX::False, 0.0f },
			{ 1, N2::NET::Activation::Sigmoid,  0, { 0.0f }, CX::False, 0.0f }
		};
		static const CX::Size             LAYERS_COUNT = sizeof(LAYERS) / sizeof(LAYERS[0]);

		N2::NET::Network   network;
		N2::CE::INetwork   *pCENetwork;
		float              inputs[4 * 2]  = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f };
		float              outputs[4 * 1];
		CX::Status         status;

		if ((status = network.Init(INPUTS_COUNT, LAYERS_COUNT, LAYERS)))
		{
			{
				auto v = network.GetInputNeurons()->GetNextSynapses()->GetWeights();

				v[0] = 0.8f; v[1] = 0.4f; v[2] = 0.3f; 
				v[3] = 0.2f; v[4] = 0.9f; v[5] = 0.5f; 
			}

			{
				auto v = network.GetInputNeurons()->GetNextSynapses()->GetNextNeurons()->GetNextSynapses()->GetWeights();

				v[0] = 0.3f; 
				v[1] = 0.5f; 
				v[2] = 0.9f; 
			}

			network.Dump(stdout);

			if ((status = pProvider->Init()))
			{
				if (NULL != (pCENetwork = pProvider->CreateNetwork()))
				{
					if ((status = pCENetwork->Init(&network)))
					{
						status = pCENetwork->Evaluate(4, inputs, outputs);

						status = pCENetwork->SyncFromCE(CX::True);

						network.Dump(stdout);

						pCENetwork->Uninit();
					}
					else
					{
						CX::Print(stdout, "N2::CL::Network::Init : {1}\n", status.GetMsg());
					}
					pProvider->DestroyNetwork(pCENetwork);
				}
				else
				{
					CX::Print(stdout, "N2::CL::Provider::CreateNetwork : {1}\n", status.GetMsg());
				}

				pProvider->Uninit();
			}
			else
			{
				CX::Print(stdout, "N2::CL::Provider::Init : {1}\n", status.GetMsg());

				N2::CL::Provider   *pCLProvider = dynamic_cast<N2::CL::Provider *>(pProvider);

				if (NULL != pCLProvider)
				{
					CX::Print(stdout, "Build log : \n{1}\n", pCLProvider->GetBuildLog());
				}
			}

			status = network.Uninit();
		}
		else
		{
			CX::Print(stdout, "N2::NET::Network::Init : {1}\n", status.GetMsg());
		}
	}

private:

	SimpleTest()
	{
	}

	~SimpleTest()
	{
	}

};
