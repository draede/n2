
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


inputs  = numpy.array([ [ 0.01, 0.01 ], [ 0.01, 0.99 ], [ 0.99, 0.01 ], [ 0.99, 0.99 ] ], "float32")

outputs = numpy.array([ [ 0.01 ], [ 0.99 ] , [ 0.99 ], [ 0.01 ] ], "float32")

model   = tensorflow.keras.Sequential()

model.add(layers.Dense(4, activation='relu', use_bias=True, input_dim=2))
model.add(layers.Dense(1, activation='sigmoid', use_bias=True))


print("=== Compiling ===============================================================================")

model.compile(loss='mean_squared_error', optimizer='adam')

print("=== Training ================================================================================")

model.fit(inputs, outputs, epochs=25000, verbose=2)

print("=== Results =================================================================================")

print(model.predict(numpy.array([ [ 0.01, 0.01 ] ], "float32"))[0][0])
print(model.predict(numpy.array([ [ 0.01, 0.99 ] ], "float32"))[0][0])
print(model.predict(numpy.array([ [ 0.99, 0.01 ] ], "float32"))[0][0])
print(model.predict(numpy.array([ [ 0.99, 0.99 ] ], "float32"))[0][0])

print("=== Saving ==================================================================================")

n2.SaveBinaryNeurons(model, "neurons.n2n.bin")
n2.SaveBinarySynapses(model, "synapses.n2s.bin")

n2.SaveCodeNeurons(model, "neurons.n2n.txt")
n2.SaveCodeSynapses(model, "synapses.n2s.txt")
