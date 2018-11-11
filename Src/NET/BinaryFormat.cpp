
#include "N2/NET/BinaryFormat.hpp"
#include "CX/IO/FileInputStream.hpp"
#include "CX/IO/FileOutputStream.hpp"
#include "CX/Vector.hpp"


using namespace CX;


namespace N2
{

namespace NET
{

BinaryFormat::BinaryFormat()
{
}

BinaryFormat::~BinaryFormat()
{
}

Status BinaryFormat::LoadNeurons(Network *pNetwork, const Char *szPath)
{
	IO::FileInputStream    is(szPath);
	UInt8                  uInt8;
	UInt32                 uInt32;
	UInt32                 cInputNeurons;
	UInt32                 cLayers;
	Vector<Layer>::Type    vectorLayers;
	Status                 status;

	if (!is.IsOK())
	{
		return Status(Status_OpenFailed, "Failed to open '{1}' at {2}:{3}", szPath, __FILE__, __LINE__);
	}
	if (!(status = Read(&is, &uInt32)))
	{
		return status;
	}
	if (NEURONS_MAGIC != uInt32)
	{
		return Status(Status_OpenFailed, "Invalid magic {1} at {2}:{3}", uInt32, __FILE__, __LINE__);
	}
	if (!(status = Read(&is, &uInt32)))
	{
		return status;
	}
	if (NEURONS_VERSION != uInt32)
	{
		return Status(Status_OpenFailed, "Invalid version {1} at {2}:{3}", uInt32, __FILE__, __LINE__);
	}
	if (!(status = Read(&is, &uInt32)))
	{
		return status;
	}
	if (!(status = Read(&is, &cInputNeurons)))
	{
		return status;
	}
	if (!(status = Read(&is, &cLayers)))
	{
		return status;
	}
	if (Layer::MAX_LAYERS < cLayers)
	{
		return Status(Status_InvalidArg, "Invalid layers count {1} at {2}:{3}", cLayers, __FILE__, __LINE__);
	}
	for (UInt32 i = 0; i < cLayers; i++)
	{
		Layer   layer;

		if (!(status = Read(&is, &layer.cNeuronsCount)))
		{
			return status;
		}
		if (Neurons::MAX_NEURONS < uInt32)
		{
			return Status(Status_InvalidArg, "Invalid neurons count {1} at {2}:{3}", layer.cNeuronsCount, __FILE__, 
			              __LINE__);
		}
		if (!(status = Read(&is, &uInt8)))
		{
			return status;
		}
		if (6 == uInt8)
		{
			layer.nActivation     = Activation::RELU;
			layer.cActivationArgs = 0;
		}
		else
		if (8 == uInt8)
		{
			layer.nActivation     = Activation::Sigmoid;
			layer.cActivationArgs = 0;
		}
		else
		{
			return Status(Status_InvalidArg, "Invalid activation {1} at {2}:{3}", uInt8, __FILE__, __LINE__);
		}
		if (!(status = Read(&is, &uInt8)))
		{
			return status;
		}
		if (0 == uInt8)
		{
			layer.bHasBias   = False;
			layer.fBiasValue = 0.0f;
		}
		else
		if (1 == uInt8)
		{
			layer.bHasBias   = True;
			layer.fBiasValue = 1.0f;
		}
		else
		{
			return Status(Status_InvalidArg, "Invalid bias {1} at {2}:{3}", uInt8, __FILE__, __LINE__);
		}
		vectorLayers.push_back(layer);
	}

	return pNetwork->Init(cInputNeurons, cLayers, &vectorLayers[0]);
}

Status BinaryFormat::SaveNeurons(const Network *pNetwork, const Char *szPath)
{
	if (!pNetwork->IsOK())
	{
		return Status(Status_InvalidArg, "Invalid arg at {1}:{2}", __FILE__, __LINE__);
	}

	IO::FileOutputStream   os(szPath);
	const Neurons          *pNeurons;
	UInt8                  nAct;
	Status                 status;

	if (!os.IsOK())
	{
		return Status(Status_CreateFailed, "Failed to create '{1}' at {2}:{3}", szPath, __FILE__, __LINE__);
	}
	if (!(status = Write(&os, NEURONS_MAGIC)))
	{
		return status;
	}
	if (!(status = Write(&os, NEURONS_VERSION)))
	{
		return status;
	}
	if (!(status = Write(&os, (UInt32)0)))
	{
		return status;
	}
	if (!(status = Write(&os, pNetwork->GetInputNeurons()->GetNeuronsCount())))
	{
		return status;
	}
	if (!(status = Write(&os, pNetwork->GetLayersCount())))
	{
		return status;
	}
	pNeurons = pNetwork->GetInputNeurons()->GetNextSynapses()->GetNextNeurons();
	while (NULL != pNeurons)
	{
		if (!(status = Write(&os, pNeurons->GetNeuronsCount())))
		{
			return status;
		}
		nAct = 0;
		switch (pNeurons->GetActivation())
		{
			case Activation::RELU:    nAct = 6; break;
			case Activation::Sigmoid: nAct = 8; break;
		}
		if (!(status = Write(&os, nAct)))
		{
			return status;
		}
		if (pNeurons->GetPrevSynapses()->HasBias())
		{
			if (!(status = Write(&os, (UInt8)1)))
			{
				return status;
			}
		}
		else
		{
			if (!(status = Write(&os, (UInt8)0)))
			{
				return status;
			}
		}
		if (NULL == pNeurons->GetNextSynapses())
		{
			break;
		}
		pNeurons = pNeurons->GetNextSynapses()->GetNextNeurons();
	}

	return Status();
}

Status BinaryFormat::LoadSynapses(Network *pNetwork, const Char *szPath)
{
	if (!pNetwork->IsOK())
	{
		return Status(Status_InvalidArg, "Invalid arg at {1}:{2}", __FILE__, __LINE__);
	}

	IO::FileInputStream    is(szPath);
	Synapses               *pSynapses;
	UInt8                  uInt8;
	UInt32                 uInt32;
	UInt32                 cLayers;
	Bool                   bHasBias;
	UInt32                 cPrevNeurons;
	UInt32                 cNextNeurons;
	Status                 status;

	if (!is.IsOK())
	{
		return Status(Status_OpenFailed, "Failed to open '{1}' at {2}:{3}", szPath, __FILE__, __LINE__);
	}
	if (!(status = Read(&is, &uInt32)))
	{
		return status;
	}
	if (SYNAPSES_MAGIC != uInt32)
	{
		return Status(Status_OpenFailed, "Invalid magic {1} at {2}:{3}", uInt32, __FILE__, __LINE__);
	}
	if (!(status = Read(&is, &uInt32)))
	{
		return status;
	}
	if (SYNAPSES_VERSION != uInt32)
	{
		return Status(Status_OpenFailed, "Invalid version {1} at {2}:{3}", uInt32, __FILE__, __LINE__);
	}
	if (!(status = Read(&is, &uInt32)))
	{
		return status;
	}
	if (!(status = Read(&is, &cLayers)))
	{
		return status;
	}
	if (cLayers != pNetwork->GetLayersCount())
	{
		return Status(Status_InvalidArg, "Invalid layers count {1} at {2}:{3}", cLayers, __FILE__, __LINE__);
	}
	pSynapses = pNetwork->GetInputNeurons()->GetNextSynapses();
	for (UInt32 i = 0; i < cLayers; i++)
	{
		if (!(status = Read(&is, &uInt8)))
		{
			return status;
		}
		if (0 == uInt8)
		{
			bHasBias = False;
		}
		else
		if (1 == uInt8)
		{
			bHasBias = True;
		}
		else
		{
			return Status(Status_InvalidArg, "Invalid bias {1} at {2}:{3}", uInt8, __FILE__, __LINE__);
		}
		if (!(status = Read(&is, &cPrevNeurons)))
		{
			return status;
		}
		if (cPrevNeurons != pSynapses->GetPrevNeuronsCount())
		{
			return Status(Status_InvalidArg, "Invalid prev neurons count {1} at {2}:{3}", cPrevNeurons, __FILE__, 
			              __LINE__);
		}
		if (!(status = Read(&is, &cNextNeurons)))
		{
			return status;
		}
		if (cNextNeurons != pSynapses->GetNextNeuronsCount())
		{
			return Status(Status_InvalidArg, "Invalid prev neurons count {1} at {2}:{3}", cNextNeurons, __FILE__, 
			              __LINE__);
		}
		if (!(status = Read(&is, pSynapses->GetWeights(), sizeof(Float) * pSynapses->GetWeightsCount())))
		{
			return status;
		}
		if (bHasBias)
		{
			if (!(status = Read(&is, &cNextNeurons)))
			{
				return status;
			}
			if (cNextNeurons != pSynapses->GetBiasesCount())
			{
				return Status(Status_InvalidArg, "Invalid prev neurons count {1} at {2}:{3}", cNextNeurons, __FILE__, 
			                 __LINE__);
			}
			if (!(status = Read(&is, pSynapses->GetBiases(), sizeof(Float) * pSynapses->GetBiasesCount())))
			{
				return status;
			}
		}
		if (NULL == pSynapses->GetNextNeurons())
		{
			break;
		}
		pSynapses = pSynapses->GetNextNeurons()->GetNextSynapses();
	}

	return Status();
}

Status BinaryFormat::SaveSynapses(const Network *pNetwork, const Char *szPath)
{
	if (!pNetwork->IsOK())
	{
		return Status(Status_InvalidArg, "Invalid arg at {1}:{2}", __FILE__, __LINE__);
	}

	IO::FileOutputStream   os(szPath);
	const Synapses         *pSynapses;
	Status                 status;

	if (!os.IsOK())
	{
		return Status(Status_CreateFailed, "Failed to create '{1}' at {2}:{3}", szPath, __FILE__, __LINE__);
	}
	if (!(status = Write(&os, SYNAPSES_MAGIC)))
	{
		return status;
	}
	if (!(status = Write(&os, SYNAPSES_VERSION)))
	{
		return status;
	}
	if (!(status = Write(&os, (UInt32)0)))
	{
		return status;
	}
	if (!(status = Write(&os, pNetwork->GetLayersCount())))
	{
		return status;
	}
	pSynapses = pNetwork->GetInputNeurons()->GetNextSynapses();
	while (NULL != pSynapses)
	{
		if (pSynapses->HasBias())
		{
			if (!(status = Write(&os, (UInt8)1)))
			{
				return status;
			}
		}
		else
		{
			if (!(status = Write(&os, (UInt8)0)))
			{
				return status;
			}
		}
		if (!(status = Write(&os, pSynapses->GetPrevNeuronsCount())))
		{
			return status;
		}
		if (!(status = Write(&os, pSynapses->GetNextNeuronsCount())))
		{
			return status;
		}
		if (!(status = Write(&os, pSynapses->GetWeights(), sizeof(Float) * pSynapses->GetWeightsCount())))
		{
			return status;
		}
		if (pSynapses->HasBias())
		{
			if (!(status = Write(&os, pSynapses->GetNextNeuronsCount())))
			{
				return status;
			}
			if (!(status = Write(&os, pSynapses->GetBiases(), sizeof(Float) * pSynapses->GetBiasesCount())))
			{
				return status;
			}
		}
		if (NULL == pSynapses->GetNextNeurons())
		{
			break;
		}
		pSynapses = pSynapses->GetNextNeurons()->GetNextSynapses();
	}

	return Status();
}

Status BinaryFormat::Write(IO::IOutputStream *pOutputStream, const void *pData, CX::Size cbSize)
{
	Size     cbAckSize;
	Status   status;

	if (!(status = pOutputStream->Write(pData, cbSize, &cbAckSize)))
	{
		return Status(Status_ReadFailed, "Failed to write file at {1}:{2}", __FILE__, __LINE__);
	}
	if (cbSize != cbAckSize)
	{
		return Status(Status_ReadFailed, "Failed to write file at {1}:{2}", __FILE__, __LINE__);
	}

	return Status();
}

Status BinaryFormat::Read(IO::IInputStream *pInputStream, void *pData, CX::Size cbSize)
{
	Size     cbAckSize;
	Status   status;

	if (!(status = pInputStream->Read(pData, cbSize, &cbAckSize)))
	{
		return Status(Status_ReadFailed, "Failed to read file at {1}:{2}", __FILE__, __LINE__);
	}
	if (cbSize != cbAckSize)
	{
		return Status(Status_ReadFailed, "Failed to read file at {1}:{2}", __FILE__, __LINE__);
	}

	return Status();
}

}//namespace NET

}//namespace N2
