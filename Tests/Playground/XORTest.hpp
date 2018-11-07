
#pragma once


#include "CX/Types.hpp"
#include "CX/Status.hpp"
#include "CX/Print.hpp"
#include "CX/Util/Timer.hpp"
#include "N2/NET/Network.hpp"
#include "N2/CL/Provider.hpp"


class XORTest
{
public:

	static void Run(N2::CE::IProvider *pProvider)
	{
		static const N2::NET::Layer   LAYERS[]     = 
		{
			{ 4, N2::NET::Activation::RELU,    0, { 0.0f }, CX::True, 1.0f },
			{ 1, N2::NET::Activation::Sigmoid, 0, { 0.0f }, CX::True, 1.0f }
		};
		static const CX::Size         LAYERS_COUNT = sizeof(LAYERS) / sizeof(LAYERS[0]);

		N2::NET::Network   network;
		N2::CE::INetwork   *pCENetwork;
		CX::Float          inputs[4 * 2]   = { 0.01f, 0.01f, 0.01f, 0.99f, 0.99f, 0.01f, 0.99f, 0.99f };
		CX::Float          outputs[4 * 1];
		CX::Float          expected[4 * 1] = { 0.01f, 0.99f, 0.99f, 0.01f };
		CX::Status         status;

		if ((status = network.Init(2, LAYERS_COUNT, LAYERS)))
		{
			status = network.LoadWeightsAndBiases(GetSynapses());

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
					 0.90435636f, -0.6495106f, 1.4241946f, 3.095701f, -2.2604134f,
					 -0.2627566f, 1.4255588f, -3.095972f,
				},
				{
					 2.2279282f, 0.0f, -1.4257591f, -0.00032601392f,
				},
			},
			{
				{
					 -4.1502724f, 1.0780509f, -3.9523096f, 4.2421966f,
				},
				{
					 4.5951185f,
				},
			},
		};

		return &synapses;
	}

};
