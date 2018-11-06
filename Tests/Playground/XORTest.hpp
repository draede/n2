
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
			{ 8, N2::NET::Activation::RELU,    0, { 0.0f }, CX::True, 0.1f },
			{ 1, N2::NET::Activation::Sigmoid, 0, { 0.0f }, CX::True, 0.1f }
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
			{
				auto s = network.GetInputNeurons()->GetNextSynapses();

				memcpy(s->GetWeights(), GetLayer1Weights(), 
				       sizeof(CX::Float) * s->GetPrevNeuronsCount() * s->GetNextNeuronsCount());
				memcpy(s->GetBiases(), GetLayer1Biases(), 
				       sizeof(CX::Float) * s->GetNextNeuronsCount());
			}
			{
				auto s = network.GetInputNeurons()->GetNextSynapses()->GetNextNeurons()->GetNextSynapses();

				memcpy(s->GetWeights(), GetLayer2Weights(), 
				       sizeof(CX::Float) * s->GetPrevNeuronsCount() * s->GetNextNeuronsCount());
				memcpy(s->GetBiases(), GetLayer2Biases(), 
				       sizeof(CX::Float) * s->GetNextNeuronsCount());
			}

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

	static const CX::Float *GetLayer1Weights()
	{
		static const CX::Float weights[] = 
		{
			-0.38944697f,  1.80160800f,  1.528211200f,  0.87701064f, 
			 1.61045200f, -1.80729030f,  1.093430900f, -1.24896350f, 
			-0.49617475f, -1.80160780f, -0.046994260f, -0.14546493f, 
			-1.61045250f,  1.80729020f, -0.210317060f, -1.24896320f, 
		};

		return weights;
	}

	static const CX::Float *GetLayer1Biases()
	{
		static const CX::Float biases[] = 
		{
			-7.5695477e-04f, -6.1914193e-07f,  5.4668248e-01f,  1.8350729e-01f,
			-6.3862166e-07f, -6.8108091e-07f,  2.1720655e-01f,  1.2489622e+00f, 
		};

		return biases;
	}

	static const CX::Float *GetLayer2Weights()
	{
		static const CX::Float weights[] = 
		{
			 0.42521372f,  2.72128840f, -1.84937610f, -0.67103493f,
			 2.91516540f,  3.19844580f, -0.16693713f, -2.70436200f,
		};

		return weights;
	}

	static const CX::Float *GetLayer2Biases()
	{
		static const CX::Float biases[] = 
		{
			-0.08082663f,
		};

		return biases;
	}

};
