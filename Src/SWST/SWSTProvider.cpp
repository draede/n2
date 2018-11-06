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

#include "N2/SWST/Provider.hpp"
#include "N2/SWST/Network.hpp"
#include "N2/SWST/Config.hpp"


using namespace CX;


namespace N2
{

namespace SWST
{

Provider::Provider()
{
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
	CX_UNUSED(pConfig);

	return Status();
}

Status Provider::Uninit()
{
	return Status();
}

Bool Provider::IsOK() const
{
	return True;
}

CE::INetwork *Provider::CreateNetwork()
{
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

}//namespace SWST

}//namespace N2
