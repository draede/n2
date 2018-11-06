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

#include "N2/CL/Neurons.hpp"
#include "N2/CL/Network.hpp"
#include "N2/CL/Provider.hpp"
#include "N2/CL/Synapses.hpp"


using namespace CX;


namespace N2
{

namespace CL
{

Neurons::Neurons(Network *pNetwork)
{
	m_pNetwork             = pNetwork;
	m_pNeurons             = NULL;
	m_pPrevSynapses        = NULL;
	m_pNextSynapses        = NULL;
	m_cbMemSize            = 0;
	m_szActivationFunction = "";
}

Neurons::~Neurons()
{
	Uninit();
}

Status Neurons::Init(NET::Neurons *pNeurons)
{
	Uninit();

	UInt32   cNeurons;
	cl_int   nError;
	Status   status;

	m_cbMemSize = sizeof(Neurons);
	for (;;)
	{
		if (NULL == m_pNetwork)
		{
			status = Status(Status_NotInitialized, "Invalid context at {1}:{2}", __FILE__, __LINE__);

			break;
		}
		switch (pNeurons->GetActivation())
		{
			case NET::Activation::Identity        : m_szActivationFunction = "ActivateIdentity"; break;
			case NET::Activation::Sigmoid         : m_szActivationFunction = "ActivateSigmoid"; break;
			case NET::Activation::BinaryStep      : m_szActivationFunction = "ActivateBinaryStep"; break;
			case NET::Activation::TanH            : m_szActivationFunction = "ActivateTanH"; break;
			case NET::Activation::ArcTan          : m_szActivationFunction = "ActivateArcTan"; break;
			case NET::Activation::SoftSign        : m_szActivationFunction = "ActivateSoftSign"; break;;
			case NET::Activation::RELU            : m_szActivationFunction = "ActivateRELU"; break;
			case NET::Activation::LeakyRELU       : m_szActivationFunction = "ActivateLeakyRELU"; break;
			case NET::Activation::SoftPlus        : m_szActivationFunction = "ActivateSoftPlus"; break;
			case NET::Activation::BentIdentity    : m_szActivationFunction = "ActivateBentIdentity"; break;
			case NET::Activation::Sinusoid        : m_szActivationFunction = "ActivateSinusoid"; break;
			case NET::Activation::SINC            : m_szActivationFunction = "ActivateSINC"; break;
			case NET::Activation::Gaussian        : m_szActivationFunction = "ActivateGaussian"; break;
			case NET::Activation::ISRU            : m_szActivationFunction = "ActivateISRU"; break;
			case NET::Activation::PRELU           : m_szActivationFunction = "ActivatePRELU"; break;
			case NET::Activation::ELU             : m_szActivationFunction = "ActivateELU"; break;
			case NET::Activation::SELU            : m_szActivationFunction = "ActivateSELU"; break;
			case NET::Activation::SRELU           : m_szActivationFunction = "ActivateSRELU"; break;
			case NET::Activation::ISRLU           : m_szActivationFunction = "ActivateISRLU"; break;
			case NET::Activation::SoftExponential : m_szActivationFunction = "ActivateSoftExponential"; break;
			case NET::Activation::SoftMax         : m_szActivationFunction = "ActivateSoftMax"; break;
		}
		if (0 == *m_szActivationFunction)
		{
			status = Status(Status_MemAllocFailed, "Invalid activation {1} at {2}:{3}", pNeurons->GetActivation(), 
			                __FILE__, __LINE__);

			break;
		}
		cNeurons = pNeurons->GetNeuronsCount();
		m_values = cl::Buffer(*m_pNetwork->GetProvider()->GetContext(), CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, 
		                      sizeof(Float) * cNeurons, pNeurons->GetValues(), &nError);
		if (CL_SUCCESS != nError)
		{
			status = Status(Status_MemAllocFailed, "Failed to allocate {1} bytes at {2}:{3}", sizeof(Float) * cNeurons, 
			                __FILE__, __LINE__);

			break;
		}
		m_pNeurons      = pNeurons;
		m_pPrevSynapses = NULL;
		m_pNextSynapses = NULL;
		m_cbMemSize += sizeof(Float) * pNeurons->GetNeuronsCount();

		break;
	}
	if (!status)
	{
		Uninit();
	}

	return status;
}

Status Neurons::Uninit()
{
	m_pNeurons             = NULL;
	m_pPrevSynapses        = NULL;
	m_pNextSynapses        = NULL;
	m_values               = cl::Buffer();
	m_cbMemSize            = 0;
	m_szActivationFunction = "";

	return Status();
}

Bool Neurons::IsOK() const
{
	return (NULL != m_pNeurons);
}

UInt32 Neurons::GetNeuronsCount() const
{
	if (NULL == m_pNeurons)
	{
		return 0;
	}

	return m_pNeurons->GetNeuronsCount();
}

NET::ActivationType Neurons::GetActivation() const
{
	if (NULL == m_pNeurons)
	{
		return NET::Activation::Identity;
	}

	return m_pNeurons->GetActivation();
}

UInt32 Neurons::GetActivationArgsCount() const
{
	if (NULL == m_pNeurons)
	{
		return 0;
	}

	return m_pNeurons->GetActivationArgsCount();
}

const Float *Neurons::GetActivationArgs() const
{
	if (NULL == m_pNeurons)
	{
		return NULL;
	}

	return m_pNeurons->GetActivationArgs();
}

const NET::Neurons *Neurons::GetNeurons() const
{
	return m_pNeurons;
}

NET::Neurons *Neurons::GetNeurons()
{
	return m_pNeurons;
}

const CE::ISynapses *Neurons::GetPrevSynapses() const
{
	return m_pPrevSynapses;
}

CE::ISynapses *Neurons::GetPrevSynapses()
{
	return m_pPrevSynapses;
}

const CE::ISynapses *Neurons::GetNextSynapses() const
{
	return m_pNextSynapses;
}

CE::ISynapses *Neurons::GetNextSynapses()
{
	return m_pNextSynapses;
}

Status Neurons::SyncToCE(Bool bWait/* = True*/)
{
	if (NULL == m_pNeurons)
	{
		return Status(Status_NotInitialized, "Not initialized at {1}:{2}", __FILE__, __LINE__);
	}

	cl_int   nError;

	if (CL_SUCCESS != (nError = m_pNetwork->GetQueue()->enqueueWriteBuffer(m_values, bWait ? CL_TRUE : CL_FALSE, 0, 
	                                            sizeof(Float) * m_pNeurons->GetNeuronsCount(), m_pNeurons->GetValues())))
	{
		return Status(Status_OperationFailed, "Failed to write values buffer at {1}:{2}", __FILE__, __LINE__);
	}

	return Status();
}

Status Neurons::SyncFromCE(Bool bWait/* = True*/)
{
	if (NULL == m_pNeurons)
	{
		return Status(Status_NotInitialized, "Not initialized at {1}:{2}", __FILE__, __LINE__);
	}

	cl_int   nError;

	if (CL_SUCCESS != (nError = m_pNetwork->GetQueue()->enqueueReadBuffer(m_values, bWait ? CL_TRUE : CL_FALSE, 0, 
	                                            sizeof(Float) * m_pNeurons->GetNeuronsCount(), m_pNeurons->GetValues())))
	{
		return Status(Status_OperationFailed, "Failed to read values buffer at {1}:{2}", __FILE__, __LINE__);
	}

	return Status();
}

Size Neurons::GetMemSize() const
{
	return m_cbMemSize;
}

const Char *Neurons::GetActivationFunction() const
{
	return m_szActivationFunction;
}

}//namespace CL

}//namespace N2
