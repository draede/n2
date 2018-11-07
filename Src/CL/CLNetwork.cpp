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

#include "N2/CL/Network.hpp"
#include "N2/CL/Provider.hpp"


using namespace CX;


namespace N2
{

namespace CL
{

Network::Network(Provider *pProvider)
{
	m_pProvider      = pProvider;
	m_pQueue         = NULL;
	m_pNetwork       = NULL;
	m_pInputNeurons  = NULL;
	m_pOutputNeurons = NULL;
	m_cbMemSize      = 0;
}

Network::~Network()
{
	Uninit();
}

Provider *Network::GetProvider()
{
	return m_pProvider;
}

Status Network::Init(NET::Network *pNetwork)
{
	Uninit();

	NET::Neurons    *pNETNeurons;
	NET::Synapses   *pNETSynapses;
	Neurons         *pNeurons;
	Synapses        *pSynapses;
	cl_int          nError;
	Status          status;

	m_cbMemSize = sizeof(Network);
	for (;;)
	{
		if (NULL == (m_pQueue = new (std::nothrow) cl::CommandQueue(*m_pProvider->GetContext(), 0, &nError)))
		{
			status = Status(Status_MemAllocFailed, "Failed to allocate queue at {1}:{2}", __FILE__, __LINE__);

			break;
		}
		if (CL_SUCCESS != nError)
		{
			status = Status(Status_OperationFailed, "Failed to create queue at {1}:{2}", __FILE__, __LINE__);

			break;
		}

		pNETNeurons = pNetwork->GetInputNeurons();

		if (NULL == (pNeurons = new (std::nothrow) Neurons(this)))
		{
			status = Status(Status_MemAllocFailed, "Failed to allocate neurons at {1}:{2}", __FILE__, __LINE__);

			break;
		}
		if (!(status = pNeurons->Init(pNETNeurons)))
		{
			delete pNeurons;

			break;
		}
		pNeurons->m_pPrevSynapses = NULL;
		pNeurons->m_pNextSynapses = NULL;
		m_pInputNeurons           = pNeurons;
		m_pOutputNeurons          = pNeurons;
		m_cbMemSize += pNeurons->GetMemSize();

		pNETSynapses = pNETNeurons->GetNextSynapses();

		while (NULL != pNETSynapses)
		{
			pNETNeurons = pNETSynapses->GetNextNeurons();
			if (NULL == (pSynapses = new (std::nothrow) Synapses(this)))
			{
				status = Status(Status_MemAllocFailed, "Failed to allocate synapses at {1}:{2}", __FILE__, __LINE__);

				break;
			}
			if (!(status = pSynapses->Init(pNETSynapses)))
			{
				delete pSynapses;

				break;
			}
			if (NULL == (pNeurons = new (std::nothrow) Neurons(this)))
			{
				status = Status(Status_MemAllocFailed, "Failed to allocate neurons at {1}:{2}", __FILE__, __LINE__);

				break;
			}
			if (!(status = pNeurons->Init(pNETNeurons)))
			{
				delete pSynapses;
				delete pNeurons;

				break;
			}

			pSynapses->m_pPrevNeurons         = m_pOutputNeurons;
			pSynapses->m_pNextNeurons         = pNeurons;
			pNeurons->m_pPrevSynapses         = pSynapses;
			pNeurons->m_pNextSynapses         = NULL;

			m_pOutputNeurons->m_pNextSynapses = pSynapses;

			m_pOutputNeurons                  = pNeurons;

			m_cbMemSize += pSynapses->GetMemSize() + pNeurons->GetMemSize();
		
			pNETSynapses = pNETNeurons->GetNextSynapses();
		}
		if (!status)
		{
			break;
		}
		m_pNetwork      = pNetwork;

		break;
	}
	if (!status)
	{
		Uninit();
	}

	return status;
}

Status Network::Uninit()
{
	if (NULL != m_pQueue)
	{
		delete m_pQueue;
	}

	Neurons    *pNeurons;
	Synapses   *pSynapses;

	if (NULL != m_pInputNeurons)
	{
		pSynapses = m_pInputNeurons->m_pNextSynapses;
		delete m_pInputNeurons;

		while (NULL != pSynapses)
		{
			pNeurons = pSynapses->m_pNextNeurons;
			delete pSynapses;
			pSynapses = pNeurons->m_pNextSynapses;
			delete pNeurons;
		}
	}

	m_pQueue         = NULL;
	m_pNetwork       = NULL;
	m_pInputNeurons  = NULL;
	m_pOutputNeurons = NULL;
	m_cbMemSize      = 0;

	return Status();
}

Bool Network::IsOK() const
{
	return (NULL != m_pNetwork);
}

UInt32 Network::GetLayersCount() const
{
	if (NULL == m_pNetwork)
	{
		return 0;
	}

	return m_pNetwork->GetLayersCount();
}

const NET::Network *Network::GetNetwork() const
{
	return m_pNetwork;
}

NET::Network *Network::GetNetwork()
{
	return m_pNetwork;
}

const CE::INeurons *Network::GetInputNeurons() const
{
	return m_pInputNeurons;
}

CE::INeurons *Network::GetInputNeurons()
{
	return m_pInputNeurons;
}

const CE::INeurons *Network::GetOutputNeurons() const
{
	return m_pOutputNeurons;
}

CE::INeurons *Network::GetOutputNeurons()
{
	return m_pOutputNeurons;
}

Status Network::SyncToCE(Bool bWait/* = True*/, UInt32 nSyncType/* = Sync_All*/)
{
	if (NULL == m_pNetwork)
	{
		return Status(Status_NotInitialized, "Not initialized at {1}:{2}", __FILE__, __LINE__);
	}

	Synapses   *pSynapses;
	Neurons    *pNeurons;

	pNeurons = m_pInputNeurons;
	while (NULL != pNeurons)
	{
		if (Sync_Neurons == (nSyncType & Sync_Neurons))
		{
			pNeurons->SyncToCE(False);
		}
		pSynapses = pNeurons->m_pNextSynapses;
		if (NULL != pSynapses)
		{
			break;
		}
		else
		{
			if (Sync_Synapse == (nSyncType & Sync_Synapse))
			{
				pSynapses ->SyncToCE(False);
			}
			pNeurons = pSynapses->m_pNextNeurons;
		}
	}
	if (bWait)
	{
		cl_int   nError;

		if (CL_SUCCESS != (nError = m_pQueue->finish()))
		{
			return Status(Status_OperationFailed, "Failed to sync at {1}:{2}", __FILE__, __LINE__);
		}
	}

	return Status();
}

Status Network::SyncFromCE(Bool bWait/* = True*/, UInt32 nSyncType/* = Sync_All*/)
{
	if (NULL == m_pNetwork)
	{
		return Status(Status_NotInitialized, "Not initialized at {1}:{2}", __FILE__, __LINE__);
	}

	Synapses   *pSynapses;
	Neurons    *pNeurons;

	pNeurons = m_pInputNeurons;
	while (NULL != pNeurons)
	{
		if (Sync_Neurons == (nSyncType & Sync_Neurons))
		{
			pNeurons->SyncFromCE(False);
		}
		pSynapses = pNeurons->m_pNextSynapses;
		if (NULL == pSynapses)
		{
			break;
		}
		else
		{
			if (Sync_Synapse == (nSyncType & Sync_Synapse))
			{
				pSynapses ->SyncFromCE(False);
			}
			pNeurons = pSynapses->m_pNextNeurons;
		}
	}
	if (bWait)
	{
		cl_int   nError;

		if (CL_SUCCESS != (nError = m_pQueue->finish()))
		{
			return Status(Status_OperationFailed, "Failed to sync at {1}:{2}", __FILE__, __LINE__);
		}
	}

	return Status();
}

Size Network::GetMemSize() const
{
	return m_cbMemSize;
}

//assumes that weights are already transferred into device memory
Status Network::Evaluate(UInt32 cCount, Float *inputs, Float *outputs)
{
	if (0 == m_pNetwork)
	{
		return Status(Status_NotInitialized, "Not initialized at {1}:{2}", __FILE__, __LINE__);
	}
	if (0 == cCount)
	{
		return Status(Status_InvalidArg, "Invalid arg at {1}:{2}", __FILE__, __LINE__);
	}

	Synapses           *pSynapses;
	cl::Buffer         *prevNeurons;
	UInt32             cPrevNeuronsOffset;
	cl::Buffer         *nextNeurons;
	UInt32             cNextNeuronsOffset;
	UInt32             cInputsOffset;
	UInt32             cOutputsOffset;
	cl_int             nError;
	Status             status;

	cl::Buffer         bufInputs(*m_pProvider->GetContext(), CL_MEM_READ_WRITE, 
	                             sizeof(Float) * cCount * m_pInputNeurons->GetNeuronsCount(), NULL, &nError);
	if (CL_SUCCESS != nError)
	{
		return Status(Status_OperationFailed, "Failed to init inputs buffer at {1}:{2}", __FILE__, __LINE__);
	}

	cl::Buffer         bufOutputs(*m_pProvider->GetContext(), CL_MEM_READ_WRITE, 
	                              sizeof(Float) * cCount * m_pOutputNeurons->GetNeuronsCount(), NULL, &nError);
	if (CL_SUCCESS != nError)
	{
		return Status(Status_OperationFailed, "Failed to init outputs buffer at {1}:{2}", __FILE__, __LINE__);
	}

	if (CL_SUCCESS != (nError = m_pQueue->enqueueWriteBuffer(bufInputs, CL_FALSE, 0, 
	                                             sizeof(Float) * cCount * m_pInputNeurons->GetNeuronsCount(), inputs)))
	{
		return Status(Status_OperationFailed, "Failed to write inputs buffer at {1}:{2}", __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = m_pQueue->enqueueWriteBuffer(bufOutputs, CL_FALSE, 0, 
	                                             sizeof(Float) * cCount * m_pOutputNeurons->GetNeuronsCount(), outputs)))
	{
		return Status(Status_OperationFailed, "Failed to write outputs buffer at {1}:{2}", __FILE__, __LINE__);
	}

	cInputsOffset  = 0;
	cOutputsOffset = 0;
	for (UInt32 i = 0; i < cCount; i++)
	{
		pSynapses = m_pInputNeurons->m_pNextSynapses;
		while (NULL != pSynapses)
		{
			if (pSynapses->m_pPrevNeurons == m_pInputNeurons)
			{
				prevNeurons        = &bufInputs;
				cPrevNeuronsOffset = cInputsOffset;
			}
			else
			{
				prevNeurons        = &pSynapses->m_pPrevNeurons->m_values;
				cPrevNeuronsOffset = 0;
			}
			if (pSynapses->m_pNextNeurons == m_pOutputNeurons)
			{
				nextNeurons        = &bufOutputs;
				cNextNeuronsOffset = cOutputsOffset;
			}
			else
			{
				nextNeurons        = &pSynapses->m_pNextNeurons->m_values;
				cNextNeuronsOffset = 0;
			}
			if (!pSynapses->HasBias())
			{
				if (!(status = Compute(prevNeurons, cPrevNeuronsOffset, pSynapses->m_pPrevNeurons->GetNeuronsCount(), 
				                       &pSynapses->m_weights, 
				                       nextNeurons, cNextNeuronsOffset, pSynapses->m_pNextNeurons->GetNeuronsCount())))
				{
					break;
				}
			}
			else
			{
				if (!(status = ComputeWithBias(pSynapses->GetBias(), 
				                            prevNeurons, cPrevNeuronsOffset, pSynapses->m_pPrevNeurons->GetNeuronsCount(), 
				                            &pSynapses->m_weights, &pSynapses->m_biases, 
				                            nextNeurons, cNextNeuronsOffset, pSynapses->m_pNextNeurons->GetNeuronsCount())))
				{
					break;
				}
			}

			if (!(status = Activate(nextNeurons, cNextNeuronsOffset, pSynapses->m_pNextNeurons->GetNeuronsCount(), 
			                        pSynapses->m_pNextNeurons->GetActivationFunction(), 
			                        pSynapses->m_pNextNeurons->GetActivation(), 
			                        pSynapses->m_pNextNeurons->GetActivationArgsCount(), 
			                        pSynapses->m_pNextNeurons->GetActivationArgs())))
			{
				break;
			}

			pSynapses = pSynapses->m_pNextNeurons->m_pNextSynapses;
		}
		if (!status)
		{
			break;
		}
		cInputsOffset += m_pInputNeurons->GetNeuronsCount();
		cOutputsOffset += m_pOutputNeurons->GetNeuronsCount();
	}
	if (!status)
	{
		return status;
	}
	if (CL_SUCCESS != (nError = m_pQueue->enqueueReadBuffer(bufOutputs, CL_FALSE, 0, 
	                                             sizeof(Float) * cCount * m_pOutputNeurons->GetNeuronsCount(), outputs)))
	{
		return Status(Status_OperationFailed, "Failed to read outputs buffer at {1}:{2}", __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = m_pQueue->finish()))
	{
		return Status(Status_OperationFailed, "Failed to read outputs buffer at {1}:{2}", __FILE__, __LINE__);
	}

	return Status();
}

cl::CommandQueue *Network::GetQueue()
{
	return m_pQueue;
}

Status Network::Compute(cl::Buffer *prevNeurons, UInt32 cPrevNeuronsOffset, UInt32 cPrevNeuronsCount,
                        cl::Buffer *weights, 
                        cl::Buffer *nextNeurons, UInt32 cNextNeuronsOffset, UInt32 cNextNeuronsCount)
{
	cl_int   nError;

	cl::Kernel   kernelCompute = cl::Kernel(*m_pProvider->GetProgram(), "Compute");

	if (CL_SUCCESS != (nError = kernelCompute.setArg(0, *prevNeurons)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = kernelCompute.setArg(1, cPrevNeuronsOffset)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = kernelCompute.setArg(2, cPrevNeuronsCount)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = kernelCompute.setArg(3, *weights)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = kernelCompute.setArg(4, *nextNeurons)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = kernelCompute.setArg(5, cNextNeuronsOffset)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = kernelCompute.setArg(6, cNextNeuronsCount)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = m_pQueue->enqueueNDRangeKernel(kernelCompute, cl::NullRange, 
	                                                           cl::NDRange(cNextNeuronsCount))))
	{
		return Status(Status_OperationFailed, "enqueueNDRangeKernel failed with error {1} at {2}:{3}", nError, __FILE__, 
		              __LINE__);
	}

	return Status();
}

Status Network::ComputeWithBias(Float fBias, 
                                cl::Buffer *prevNeurons, UInt32 cPrevNeuronsOffset, UInt32 cPrevNeuronsCount,
                                cl::Buffer *weights, cl::Buffer *biases, 
                                cl::Buffer *nextNeurons, UInt32 cNextNeuronsOffset, UInt32 cNextNeuronsCount)
{
	cl_int   nError;

	cl::Kernel   kernelCompute = cl::Kernel(*m_pProvider->GetProgram(), "ComputeWithBias");

	if (CL_SUCCESS != (nError = kernelCompute.setArg(0, fBias)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = kernelCompute.setArg(1, *prevNeurons)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = kernelCompute.setArg(2, cPrevNeuronsOffset)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = kernelCompute.setArg(3, cPrevNeuronsCount)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = kernelCompute.setArg(4, *weights)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = kernelCompute.setArg(5, *biases)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = kernelCompute.setArg(6, *nextNeurons)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = kernelCompute.setArg(7, cNextNeuronsOffset)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = kernelCompute.setArg(8, cNextNeuronsCount)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = m_pQueue->enqueueNDRangeKernel(kernelCompute, cl::NullRange, 
	                                                           cl::NDRange(cNextNeuronsCount))))
	{
		return Status(Status_OperationFailed, "enqueueNDRangeKernel failed with error {1} at {2}:{3}", nError, __FILE__, 
		              __LINE__);
	}
	if (CL_SUCCESS != (nError = m_pQueue->finish()))
	{
		return Status(Status_OperationFailed, "Finish failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}

	return Status();
}

Status Network::Activate(cl::Buffer *nextNeurons, UInt32 cNextNeuronsOffset, UInt32 cNextNeuronsCount, 
                         const Char *szActivation, NET::ActivationType nActivation, UInt32 cActivationArgs, 
                         const Float *activationArgs)
{
	if (NET::Activation::Identity == nActivation)
	{
		return Status();
	}

	cl_int       nError;
	cl::Kernel   kernelActivate = cl::Kernel(*m_pProvider->GetProgram(), szActivation);

	if (CL_SUCCESS != (nError = kernelActivate.setArg(0, *nextNeurons)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (nError = kernelActivate.setArg(1, cNextNeuronsOffset)))
	{
		return Status(Status_OperationFailed, "setArg failed with error {1} at {2}:{3}", nError, __FILE__, __LINE__);
	}
	for (UInt32 i = 0; i < cActivationArgs; i++)
	{
		if (CL_SUCCESS != (nError = kernelActivate.setArg(i + 2, activationArgs[i])))
		{
			return Status(Status_OperationFailed, "setArg failed with error {1} ({2}) at {3}:{4}", nError, i, __FILE__, 
			              __LINE__);
		}
	}
	if (CL_SUCCESS != (nError = m_pQueue->enqueueNDRangeKernel(kernelActivate, cl::NullRange, 
	                                                           cl::NDRange(cNextNeuronsCount))))
	{
		return Status(Status_OperationFailed, "enqueueNDRangeKernel failed with error {1} at {2}:{3}", nError, __FILE__, 
		              __LINE__);
	}

	return Status();
}

}//namespace CL

}//namespace N2
