
import sys
sys.path.append("..")

import n2
import numpy
import tensorflow
from tensorflow.keras import layers


# fix mem issue on windows
config = tensorflow.ConfigProto()
config.gpu_options.allow_growth = True
session = tensorflow.Session(config=config)
tensorflow.keras.backend.set_session(session)


best_loss = 1000000000

while True:
	inputs  = numpy.array([ [ 0.01, 0.01 ], [ 0.01, 0.99 ], [ 0.99, 0.01 ], [ 0.99, 0.99 ] ], "float32")

	outputs = numpy.array([ [ 0.01 ], [ 0.99 ] , [ 0.99 ], [ 0.01 ] ], "float32")

	model   = tensorflow.keras.Sequential()

	model.add(layers.Dense(4, activation='relu', use_bias=True, input_dim=2))
	model.add(layers.Dense(1, activation='sigmoid', use_bias=True))

	model.compile(loss='mean_squared_error', optimizer='adam')

	for i in range(25000):
		history = model.fit(inputs, outputs, epochs=1, verbose=0)

		last_loss = history.history['loss'][0]

		if best_loss > last_loss:
			print(best_loss)
			best_loss = last_loss
			file = open("stats.txt", "w")
			print("loss : ", best_loss, file=file);
			print("", file=file);
			print("0.01, 0.01 => ", model.predict(numpy.array([ [ 0.01, 0.01 ] ], "float32"))[0][0], " (expected 0.01)", file=file);
			print("0.01, 0.99 => ", model.predict(numpy.array([ [ 0.01, 0.99 ] ], "float32"))[0][0], " (expected 0.99)", file=file);
			print("0.99, 0.01 => ", model.predict(numpy.array([ [ 0.99, 0.01 ] ], "float32"))[0][0], " (expected 0.99)", file=file);
			print("0.99, 0.99 => ", model.predict(numpy.array([ [ 0.99, 0.99 ] ], "float32"))[0][0], " (expected 0.01)", file=file);
			file.close()
			n2.SaveBinaryNeurons(model, "neurons.n2n.bin")
			n2.SaveBinarySynapses(model, "synapses.n2s.bin")
			n2.SaveCodeNeurons(model, "neurons.n2n.txt")
			n2.SaveCodeSynapses(model, "synapses.n2s.txt")
