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

#include "N2/CL/KernelSources.hpp"
#include "CX/IO/FileInputStream.hpp"


using namespace CX;


namespace N2
{

namespace CL
{

KernelSources::KernelSources()
{
}

KernelSources::~KernelSources()
{
}

Status KernelSources::Register(const Char *szSource, Size cSourceLen/* = (Size) - 1*/)
{
	if ((Size)-1 == cSourceLen)
	{
		cSourceLen = cx_strlen(szSource);
	}
	if (MAX_SOURCE_LEN < (UInt64)cSourceLen)
	{
		return Status(Status_InvalidArg, "Invalid source length {1} at {2}:{3}", cSourceLen, __FILE__,__LINE__);
	}

	SourcesVector   *pVectorSources = GetSources();
	Source          source;

	source.sSource.assign(szSource, cSourceLen);

	pVectorSources->push_back(source);

	return Status();
}

Size KernelSources::GetCount()
{
	return GetSources()->size();
}

const Char *KernelSources::GetSource(Size cIndex)
{
	SourcesVector   *pVectorSources = GetSources();

	if (pVectorSources->size() <= cIndex)
	{
		return "";
	}

	return (*pVectorSources)[cIndex].sSource.c_str();
}

Size KernelSources::GetSourceLen(Size cIndex)
{
	SourcesVector   *pVectorSources = GetSources();

	if (pVectorSources->size() <= cIndex)
	{
		return 0;
	}

	return (*pVectorSources)[cIndex].sSource.size();
}

Status KernelSources::LoadSources(cl::Program::Sources *pSources)
{
	SourcesVector   *pVectorSources = GetSources();
	UInt64          cbFileSize;
	Size            cbAckSize;
	Status          status;

	for (auto iter = pVectorSources->begin(); iter != pVectorSources->end(); ++iter)
	{
#pragma warning(push)
#pragma warning(disable: 4996)
		if (0 == cx_strnicmp(iter->sSource.c_str(), "file://", 7))
#pragma warning(pop)
		{
			String                sPath(iter->sSource.c_str() + 7);
			IO::FileInputStream   fis(sPath.c_str());
			String                sBackup;

			if (!fis.IsOK())
			{
				return Status(Status_OpenFailed, "Failed to open '{1}' at {2}:{3}", sPath, __FILE__, __LINE__);
			}
			if (!(status = fis.GetSize(&cbFileSize)))
			{
				return Status(Status_OpenFailed, "Failed to get file size for '{1}' at {2}:{3}", sPath, __FILE__, __LINE__);
			}
			if (MAX_SOURCE_LEN < cbFileSize)
			{
				return Status(Status_InvalidArg, "Invalid source length {1} for file '{2}' at {3}:{4}", cbFileSize, 
				              sPath, __FILE__, __LINE__);
			}
			sBackup = iter->sSource;
			iter->sSource.clear();
			iter->sSource.resize((Size)cbFileSize, '\0');
			if (!(status = fis.Read((void *)iter->sSource.data(), (Size)cbFileSize, &cbAckSize)))
			{
				return Status(Status_ReadFailed, "Failed to read '{1}' at {2}:{3}", sPath, __FILE__, __LINE__);
			}
			if ((Size)cbFileSize != cbAckSize)
			{
				return Status(Status_ReadFailed, "Failed to read '{1}' at {2}:{3}", sPath, __FILE__, __LINE__);
			}
		}
		pSources->push_back({ iter->sSource.c_str(), iter->sSource.size() });
	}

	return Status();
}

Status KernelSources::Build(cl::Device &device, cl::Context &context, cl::Program **ppProgram, 
                            String *psBuildLog/* = NULL*/)
{
	cl::Program::Sources    sources;
	Status                  status;

	if (!(status = KernelSources::LoadSources(&sources)))
	{
		return status;
	}
	if (sources.empty())
	{
		return Status(Status_NotInitialized, "No sources registered at {1}:{2}", __FILE__, __LINE__);
	}
	if (NULL == (*ppProgram = new (std::nothrow) cl::Program(context, sources)))
	{
		return Status(Status_ReadFailed, "Failed to create program at {1}:{2}", __FILE__, __LINE__);
	}
	if (CL_SUCCESS != (*ppProgram)->build({ device }))
	{
		if (NULL != psBuildLog)
		{
			*psBuildLog = (*ppProgram)->getBuildInfo<CL_PROGRAM_BUILD_LOG>(device).c_str();
		}
		delete *ppProgram;
		*ppProgram = NULL;

		return Status(Status_OperationFailed, "Failed to build program at {1}:{2}", __FILE__, __LINE__);
	}

	return Status();
}

KernelSources::SourcesVector *KernelSources::GetSources()
{
	static SourcesVector   vectorSources;

	return &vectorSources;
}

}//namespace CL

}//namespace N2
