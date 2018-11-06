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


namespace N2
{

namespace SWMT
{

class IKernel
{
public:

	virtual ~IKernel() { }

	virtual void Run(CX::UInt32 cDims, const CX::UInt32 *dims, const CX::UInt32 *startIdxs, CX::UInt32 cCount)
	{
		if (3 == cDims)
		{
			CX::UInt32   idxs[3] = { startIdxs[0], startIdxs[1], startIdxs[2] };

			for (unsigned int k = 0; k < cCount; k++)
			{
				Run(cDims, idxs);
				if (idxs[2] + 1 < dims[2])
				{
					idxs[2]++;
				}
				else
				{
					idxs[2] = 0;
					if (idxs[1] + 1 < dims[1])
					{
						idxs[1]++;
					}
					else
					{
						idxs[1] = 0;
						if (idxs[0] + 1 < dims[0])
						{
							idxs[0]++;
						}
						else
						{
							break;
						}
					}
				}
			}
		}
		else
		if (2 == cDims)
		{
			CX::UInt32   idxs[2] = { startIdxs[0], startIdxs[1] };

			for (unsigned int k = 0; k < cCount; k++)
			{
				Run(cDims, idxs);
				if (idxs[1] + 1 < dims[1])
				{
					idxs[1]++;
				}
				else
				{
					idxs[1] = 0;
					if (idxs[0] + 1 < dims[0])
					{
						idxs[0]++;
					}
					else
					{
						break;
					}
				}
			}
		}
		else
		{
			CX::UInt32   idxs[1] = { startIdxs[0] };

			for (unsigned int k = 0; k < cCount; k++)
			{
				Run(cDims, idxs);
				if (idxs[0] + 1 < dims[0])
				{
					idxs[0]++;
				}
				else
				{
					break;
				}
			}
		}
	}

	virtual void Run(CX::UInt32 cDims, const CX::UInt32 *idxs)
	{
	}

};

}//namespace SWMT

}//namespace N2
