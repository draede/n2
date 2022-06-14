/* 
 * N2
 *
 * https://github.com/draede/n2
 * 
 * Copyright (C) 2018 draede
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

#include "N2/CL/Provider.hpp"
#include "N2/CL/Network.hpp"
#include "N2/CL/Config.hpp"
#include "N2/CL/KernelSources.hpp"


using namespace CX;


namespace N2
{

namespace CL
{

Provider::Provider()
{
	m_pDevice  = NULL;
	m_pContext = NULL;
	m_pProgram = NULL;
}

Provider::~Provider()
{
	Uninit();
}

CE::IConfig *Provider::CreateConfig()
{
	return new (std::nothrow) Config();
}

Status Provider::DestroyConfig(CE::IConfig *pConfig)
{
	Config *pCLConfig = dynamic_cast<Config *>(pConfig);

	if (NULL == pCLConfig)
	{
		return Status(Status_InvalidArg, "Invalid arg at {1}:{2}", __FILE__, __LINE__);
	}
	delete pCLConfig;

	return Status();
}

Status Provider::Init(const CE::IConfig *pConfig/* = NULL*/)
{
	cl_device_id   nDevID = NULL;
	Status         status;

	if (NULL != pConfig)
	{
		const Config *pCLConfig = dynamic_cast<const Config *>(pConfig);

		if (NULL == pCLConfig)
		{
			return Status(Status_MemAllocFailed, "Invalid arg at {1}:{2}", __FILE__, __LINE__);
		}
	
		nDevID = pCLConfig->GetDeviceID();
	}
	if (NULL == nDevID)
	{
		cl::Device   device;

		if (!(status = OpenCL::ChooseDeviceByTypeAndIndex(&device)))
		{
			return status;
		}
		nDevID = device();
	}
	if (NULL == nDevID)
	{
		return Status(Status_MemAllocFailed, "No device available at {1}:{2}", __FILE__, __LINE__);
	}

	Uninit();

	m_sBuildLog.clear();
	status.Clear();
	for (;;)
	{
		if (NULL== (m_pDevice = new (std::nothrow) cl::Device(nDevID)))
		{
			status = Status(Status_MemAllocFailed, "Failed to create device at {1}:{2}", __FILE__, __LINE__);
		}
		if (NULL== (m_pContext = new (std::nothrow) cl::Context(*m_pDevice)))
		{
			status = Status(Status_MemAllocFailed, "Failed to create context at {1}:{2}", __FILE__, __LINE__);
		}
		if (!(status = KernelSources::Build(*m_pDevice, *m_pContext, &m_pProgram, &m_sBuildLog)))
		{
			break;
		}

		break;
	}
	if (!status)
	{
		Uninit();
	}

	return status;
}

Status Provider::Uninit()
{
	if (NULL != m_pProgram)
	{
		delete m_pProgram;
	}
	if (NULL != m_pContext)
	{
		delete m_pContext;
	}
	if (NULL != m_pDevice)
	{
		delete m_pDevice;
	}
	m_pProgram = NULL;
	m_pDevice  = NULL;
	m_pContext = NULL;

	return Status();
}

Bool Provider::IsOK() const
{
	return (NULL != m_pContext);
}

CE::INetwork *Provider::CreateNetwork()
{
	if (NULL == m_pContext)
	{
		return NULL;
	}

	return new (std::nothrow) Network(this);
}

Status Provider::DestroyNetwork(CE::INetwork *pNetwork)
{
	Network *pCLNetwork = dynamic_cast<Network *>(pNetwork);

	if (NULL == pCLNetwork)
	{
		return Status(Status_MemAllocFailed, "Invalid arg at {1}:{2}", __FILE__, __LINE__);
	}

	delete pCLNetwork;

	return Status();
}

cl::Device *Provider::GetDevice()
{
	return m_pDevice;
}

cl::Context *Provider::GetContext()
{
	return m_pContext;
}

cl::Program *Provider::GetProgram()
{
	return m_pProgram;
}

const Char *Provider::GetBuildLog() const
{
	return m_sBuildLog.c_str();
}

Status Provider::COMPUTE_NEURONS_REGISTERED_STATUS  = KernelSources::Register("file://Compute.cl");

Status Provider::ACTIVATE_NEURONS_REGISTERED_STATUS = KernelSources::Register("file://Activate.cl");

}//namespace CL

}//namespace N2
