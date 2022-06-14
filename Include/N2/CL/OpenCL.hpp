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

#pragma once


#include "CX/Types.hpp"
#include "CX/Status.hpp"
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/cl.hpp>
#include <vector>


namespace N2
{

namespace CL
{

class OpenCL
{
public:

	static CX::Status GetPlatforms(std::vector<cl::Platform> *pVectorPlatforms);

	static CX::Status GetDevices(cl::Platform *pPlatform, std::vector<cl::Device> *pVectorDevices, 
	                             cl_device_type nDeviceTypes = CL_DEVICE_TYPE_ALL);

	//get the first GPU device; if none then first CPU; if none then first ACCELERATOR; if none then the first anything!
	static CX::Status ChooseDeviceByTypeAndIndex(cl::Device *pDevice);

	//get the first GPU device; if none then first CPU; if none then first ACCELERATOR; if none then the first anything!
	static CX::Status ChooseDeviceByTypeAndIndex(std::vector<cl::Platform> *pVectorPlatforms, cl::Device *pDevice);

	//get the first GPU device; if none then first CPU; if none then first ACCELERATOR; if none then the first anything!
	static CX::Status ChooseDeviceByTypeAndIndex(std::vector<cl::Device> *pVectorDevices, cl::Device *pDevice);

private:

	OpenCL();

	~OpenCL();

};

}//namespace CL

}//namespace N2
