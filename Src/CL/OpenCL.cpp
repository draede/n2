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

#include "N2/CL/OpenCL.hpp"


using namespace CX;


namespace N2
{

namespace CL
{

OpenCL::OpenCL()
{
}

OpenCL::~OpenCL()
{
}

Status OpenCL::GetPlatforms(std::vector<cl::Platform> *pVectorPlatforms)
{
	cl_int   nError;

	pVectorPlatforms->clear();
	if (CL_SUCCESS != (nError = cl::Platform::get(pVectorPlatforms)))
	{
		return Status(Status_OperationFailed, "Failed to get platforms with error {1} at {2}:{3}", nError, __FILE__, 
		              __LINE__);
	}

	return Status();
}

Status OpenCL::GetDevices(cl::Platform *pPlatform, std::vector<cl::Device> *pVectorDevices, 
                          cl_device_type nDeviceTypes/* = CL_DEVICE_TYPE_ALL*/)
{
	cl_int   nError;

	pVectorDevices->clear();
	if (CL_SUCCESS != (nError = pPlatform->getDevices(nDeviceTypes, pVectorDevices)))
	{
		return Status(Status_OperationFailed, "Failed to get devices with error {1} at {2}:{3}", nError, __FILE__, 
		              __LINE__);
	}

	return Status();
}

//get the first GPU device; if none then first CPU; if none then first ACCELERATOR; if none then the first anything!
Status OpenCL::ChooseDeviceByTypeAndIndex(cl::Device *pDevice)
{
	std::vector<cl::Platform>   vectorPlatforms;
	Status                      status;

	if (!(status = GetPlatforms(&vectorPlatforms)))
	{
		return status;
	}

	return ChooseDeviceByTypeAndIndex(&vectorPlatforms, pDevice);
}

Status OpenCL::ChooseDeviceByTypeAndIndex(std::vector<cl::Platform> *pVectorPlatforms, cl::Device *pDevice)
{
	cl::Device   *pGPUDevice = NULL;
	cl::Device   *pCPUDevice = NULL;
	cl::Device   *pACCDevice = NULL;
	cl::Device   *pANYDevice = NULL;
	Status       status;

	for (auto iterPlatforms = pVectorPlatforms->begin(); iterPlatforms != pVectorPlatforms->end(); ++iterPlatforms)
	{
		std::vector<cl::Device>   vectorDevices;

		if ((status = GetDevices(&*iterPlatforms, &vectorDevices)))
		{
			if ((status = ChooseDeviceByTypeAndIndex(&vectorDevices, pDevice)))
			{
				if (CL_DEVICE_TYPE_GPU == pDevice->getInfo<CL_DEVICE_TYPE>())
				{
					pGPUDevice = pDevice;

					break;
				}
				else
				if (CL_DEVICE_TYPE_CPU == pDevice->getInfo<CL_DEVICE_TYPE>())
				{
					if (NULL == pCPUDevice)
					{
						pCPUDevice = pDevice;
					}
				}
				else
				if (CL_DEVICE_TYPE_ACCELERATOR == pDevice->getInfo<CL_DEVICE_TYPE>())
				{
					if (NULL == pACCDevice)
					{
						pACCDevice = pDevice;
					}
				}
				else
				{
					if (NULL == pANYDevice)
					{
						pANYDevice = pDevice;
					}
				}
			}
		}
		if (NULL != pGPUDevice)
		{
			break;
		}
	}
	if (NULL != pGPUDevice)
	{
		*pDevice = *pGPUDevice;
	}
	else
	if (NULL != pCPUDevice)
	{
		*pDevice = *pCPUDevice;
	}
	else
	if (NULL != pACCDevice)
	{
		*pDevice = *pACCDevice;
	}
	else
	if (NULL != pANYDevice)
	{
		*pDevice = *pANYDevice;
	}
	else
	{
		return Status(Status_NotFound, "No device found at {1}:{2}", __FILE__, __LINE__);
	}

	return Status();
}

//get the first GPU device; if none then first CPU; if none then first ACCELERATOR; if none then the first anything!
Status OpenCL::ChooseDeviceByTypeAndIndex(std::vector<cl::Device> *pVectorDevices, cl::Device *pDevice)
{
	cl::Device   *pGPUDevice = NULL;
	cl::Device   *pCPUDevice = NULL;
	cl::Device   *pACCDevice = NULL;
	cl::Device   *pANYDevice = NULL;

	for (auto iter = pVectorDevices->begin(); iter != pVectorDevices->end(); ++iter)
	{
		if (CL_DEVICE_TYPE_GPU == iter->getInfo<CL_DEVICE_TYPE>())
		{
			pGPUDevice = &*iter;

			break;
		}
		else
		if (CL_DEVICE_TYPE_CPU == iter->getInfo<CL_DEVICE_TYPE>())
		{
			if (NULL == pCPUDevice)
			{
				pCPUDevice = &*iter;
			}
		}
		else
		if (CL_DEVICE_TYPE_CPU == iter->getInfo<CL_DEVICE_TYPE>())
		{
			if (NULL == pCPUDevice)
			{
				pCPUDevice = &*iter;
			}
		}
		else
		if (CL_DEVICE_TYPE_ACCELERATOR == iter->getInfo<CL_DEVICE_TYPE>())
		{
			if (NULL == pACCDevice)
			{
				pACCDevice = &*iter;
			}
		}
		else
		{
			if (NULL == pANYDevice)
			{
				pANYDevice = &*iter;
			}
		}
	}
	if (NULL != pGPUDevice)
	{
		*pDevice = *pGPUDevice;
	}
	else
	if (NULL != pCPUDevice)
	{
		*pDevice = *pCPUDevice;
	}
	else
	if (NULL != pACCDevice)
	{
		*pDevice = *pACCDevice;
	}
	else
	if (NULL != pANYDevice)
	{
		*pDevice = *pANYDevice;
	}
	else
	{
		return Status(Status_NotFound, "No device found at {1}:{2}", __FILE__, __LINE__);
	}

	return Status();
}

}//namespace CL

}//namespace N2
