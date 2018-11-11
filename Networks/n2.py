
import struct
import numpy


def SaveBinaryNeurons(model, filename):
	file = open(filename, "wb")

	file.write((0x444E324E).to_bytes(4, 'little')) # magic
	
	file.write((1).to_bytes(4, 'little')) # version
	
	file.write((0).to_bytes(4, 'little')) # checksum
	
	inpcfg = model.get_layer(index=0).get_config()
	file.write((inpcfg['batch_input_shape'][1]).to_bytes(4, 'little')) # number of inputs
	
	file.write((len(model.layers)).to_bytes(4, 'little')) # number of layers
	
	for layer in model.layers:
		cfg = layer.get_config()
		if 'float32' != cfg['dtype']:
			return False

		file.write((cfg['units']).to_bytes(4, 'little')) # neurons

		act = 0
		if 'softmax' == cfg['activation']:
			act = 1
		elif 'elu' == cfg['activation']:
			act = 2
		elif 'selu' == cfg['activation']:
			act = 3
		elif 'softplus' == cfg['activation']:
			act = 4
		elif 'softsign' == cfg['activation']:
			act = 5
		elif 'relu' == cfg['activation']:
			act = 6
		elif 'tanh' == cfg['activation']:
			act = 7
		elif 'sigmoid' == cfg['activation']:
			act = 8
		elif 'hard_sigmoid' == cfg['activation']:
			act = 9
		elif 'exponential' == cfg['activation']:
			act = 10
		elif 'linear' == cfg['activation']:
			act = 11
		elif 'LeakyReLU' == cfg['activation']:
			act = 12
		elif 'PReLU' == cfg['activation']:
			act = 13
		elif 'ELU' == cfg['activation']:
			act = 14
		elif 'ThresholdedReLU' == cfg['activation']:
			act = 15
		elif 'Softmax' == cfg['activation']:
			act = 16
		elif 'ReLU' == cfg['activation']:
			act = 17
		else:
			return False

		file.write((act).to_bytes(1, 'little')) # activation

		if cfg['use_bias']:
			file.write((1).to_bytes(1, 'little')) # has bias
		else:
			file.write((0).to_bytes(1, 'little')) # no bias

	file.close()

	return True


def SaveBinarySynapses(model, filename):
	file = open(filename, "wb")

	file.write((0x4453324E).to_bytes(4, 'little')) # magic
	
	file.write((1).to_bytes(4, 'little')) # version
	
	file.write((0).to_bytes(4, 'little')) # checksum
	
	file.write((len(model.layers)).to_bytes(4, 'little')) # number of layers
	
	for layer in model.layers:
		cfg = layer.get_config()
		if 'float32' != cfg['dtype']:
			return False

		weights = layer.get_weights()

		if cfg['use_bias'] and 2 != len(weights):
			return False
		if not cfg['use_bias'] and 1 != len(weights):
			return False

		if cfg['use_bias']:
			file.write((1).to_bytes(1, 'little')) # has bias
		else:
			file.write((0).to_bytes(1, 'little')) # no bias

		rows = len(weights[0])
		cols = len(weights[0][0])
		file.write((rows).to_bytes(4, 'little'))
		file.write((cols).to_bytes(4, 'little'))
		for i in range(rows):
			for j in range(cols):
				file.write(bytearray(struct.pack("f", weights[0][i][j]))) # weight <i, j>

		if cfg['use_bias']:
			cols = len(weights[1])
			file.write((cols).to_bytes(4, 'little'))
			for j in range(cols):
				file.write(bytearray(struct.pack("f", float(weights[1][j])))) # bias <j>

	file.close()

	return True


def SaveCodeNeurons(model, filename):
	file = open(filename, "w")

	inpcfg = model.get_layer(index=0).get_config()
	
	print("      static const CX::UInt32       INPUTS_COUNT = ", inpcfg['batch_input_shape'][1], ";", file=file)
	print("      static const N2::NET::Layer   LAYERS[]     = ", file=file)
	print("      {", file=file)
	for layer in model.layers:
		cfg = layer.get_config()
		if 'float32' != cfg['dtype']:
			return False

		act = ""
		if 'softmax' == cfg['activation']:
			act = "N2::NET::Activation::XXX_softmax, 0, { 0.0f }"
		elif 'elu' == cfg['activation']:
			act = "N2::NET::Activation::XXX_elu, 0, { 0.0f }"
		elif 'selu' == cfg['activation']:
			act = "N2::NET::Activation::XXX_selu, 0, { 0.0f }"
		elif 'softplus' == cfg['activation']:
			act = "N2::NET::Activation::XXX_softplus, 0, { 0.0f }"
		elif 'softsign' == cfg['activation']:
			act = "N2::NET::Activation::XXX_softsign, 0, { 0.0f }"
		elif 'relu' == cfg['activation']:
			act = "N2::NET::Activation::RELU, 0, { 0.0f }"
		elif 'tanh' == cfg['activation']:
			act = "N2::NET::Activation::XXX_tanh, 0, { 0.0f }"
		elif 'sigmoid' == cfg['activation']:
			act = "N2::NET::Activation::Sigmoid, 0, { 0.0f }"
		elif 'hard_sigmoid' == cfg['activation']:
			act = "N2::NET::Activation::XXX_hard_sigmoid, 0, { 0.0f }"
		elif 'exponential' == cfg['activation']:
			act = "N2::NET::Activation::XXX_exponential, 0, { 0.0f }"
		elif 'linear' == cfg['activation']:
			act = "N2::NET::Activation::XXX_linear, 0, { 0.0f }"
		elif 'LeakyReLU' == cfg['activation']:
			act = "N2::NET::Activation::XXX_LeakyReLU, 0, { 0.0f }"
		elif 'PReLU' == cfg['activation']:
			act = "N2::NET::Activation::XXX_PReLU, 0, { 0.0f }"
		elif 'ELU' == cfg['activation']:
			act = "N2::NET::Activation::XXX_ELU, 0, { 0.0f }"
		elif 'ThresholdedReLU' == cfg['activation']:
			act = "N2::NET::Activation::XXX_ThresholdedReLU, 0, { 0.0f }"
		elif 'Softmax' == cfg['activation']:
			act = "N2::NET::Activation::XXX_Softmax, 0, { 0.0f }"
		elif 'ReLU' == cfg['activation']:
			act = "N2::NET::Activation::XXX_ReLU, 0, { 0.0f }"
		else:
			return False
			
		bias = "CX::False"
		if cfg['use_bias']:
			bias = "CX::True"
		else:
			bias = "CX::False"

		print("      { ", cfg['units'], ", ", act, ", ", bias, ", 1.0f }, ", file=file)

	print("      };", file=file)
	print("      static const CX::Size         LAYERS_COUNT = sizeof(LAYERS) / sizeof(LAYERS[0]);", file=file)

	file.close()

	return True


def SaveCodeSynapses(model, filename):
	file = open(filename, "w")

	print("      static const N2::NET::Network::SynapsesDataVector   synapses =", file=file)
	print("      {", file=file)
	for layer in model.layers:
		print("         {", file=file)
		entries = 0
		for weight in layer.get_weights():
			print("            {", file=file)
			count = 0
			line  = ""
			for w in numpy.nditer(weight):
				for x in numpy.nditer(w):
					line += str(x)
					line += "f, "
					count += 1
					if 5 == count:
						print("               ", line, file=file)
						count = 0
						line  = ""
			if 0 < count:
				print("               ", line, file=file)
			print("            }, ", file=file)
			entries += 1
		if 2 != entries:
			print("            {", file=file)
			print("            },", file=file)
		print("         }, ", file=file)
	print("      };", file=file)

	file.close()

	return True
