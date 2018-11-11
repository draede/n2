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
#include "N2/NET/BinaryFormat.hpp"


class XORTest
{
public:

	static void Run(N2::CE::IProvider *pProvider)
	{
		static const CX::UInt32       INPUTS_COUNT =  2 ;
		static const N2::NET::Layer   LAYERS[]     = 
		{
			{  4 ,  N2::NET::Activation::RELU, 0, { 0.0f } ,  CX::True , 1.0f },
			{  1 ,  N2::NET::Activation::Sigmoid, 0, { 0.0f } ,  CX::True , 1.0f }
		};
		static const CX::Size         LAYERS_COUNT = sizeof(LAYERS) / sizeof(LAYERS[0]);

		N2::NET::Network   network;
		N2::CE::INetwork   *pCENetwork;
		CX::Float          inputs[4 * 2]   = { 0.01f, 0.01f, 0.01f, 0.99f, 0.99f, 0.01f, 0.99f, 0.99f };
		CX::Float          outputs[4 * 1];
		CX::Float          expected[4 * 1] = { 0.01f, 0.99f, 0.99f, 0.01f };
		CX::Status         status;

		//if ((status = N2::NET::BinaryFormat::LoadNeurons(&network, "neurons.n2n.bin")))
		if ((status = network.Init(INPUTS_COUNT, LAYERS_COUNT, LAYERS)))
		{
			//if ((status = N2::NET::BinaryFormat::LoadSynapses(&network, "synapses.n2s.bin")))
			if ((status = status = network.LoadWeightsAndBiases(GetSynapses())))
			{
				if ((status = pProvider->Init()))
				{
					if (NULL != (pCENetwork = pProvider->CreateNetwork()))
					{
						if ((status = pCENetwork->Init(&network)))
						{
							status = pCENetwork->Evaluate(4, inputs, outputs);

							for (CX::Size i = 0; i < 4; i++)
							{
								CX::Print(stdout, "Expected : {1}, Computed : {2}\n", expected[i], outputs[i]);
							}

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
			}
			else
			{
				CX::Print(stdout, "N2::NET::Network::LoadWeightsAndBiases : {1}\n", status.GetMsg());
			}

			status = network.Uninit();
		}
		else
		{
			CX::Print(stdout, "N2::NET::Network::Init : {1}\n", status.GetMsg());
		}
	}

private:

	XORTest()
	{
	}

	~XORTest()
	{
	}

	static const N2::NET::Network::SynapsesDataVector *GetSynapses()
	{
		static const N2::NET::Network::SynapsesDataVector   synapses =
		{
			{
				{
						-3.4795704f, -0.08041801f, 2.7649038f, -0.87902904f, 3.4795713f, 
						0.08041863f, -2.7649038f, -0.1809907f, 
				}, 
				{
						-3.8648506e-07f, 0.54109365f, 1.6844277e-08f, 0.0f, 
				}, 
			}, 
			{
				{
						2.7363226f, -1.783619f, 3.3398473f, -0.16329283f, 
				}, 
				{
						-3.6300147f, 
				}, 
			}, 
		};

		return &synapses;
	}

};
