
import numpy as np
import tensorflow as tf
from tensorflow.keras import layers


inputs  = np.array([ [ 0.01, 0.01 ], [ 0.01, 0.99 ], [ 0.99, 0.01 ], [ 0.99, 0.99 ] ], "float32")

outputs = np.array([ [ 0.01 ], [ 0.99 ] , [ 0.99 ], [ 0.01 ] ], "float32")

model   = tf.keras.Sequential()

model.add(layers.Dense(4, activation='relu', use_bias=True, input_dim=2))
model.add(layers.Dense(1, activation='sigmoid', use_bias=True))

print("Compiling...")

model.compile(loss='mean_squared_error', optimizer='adam')

print("Training...")

model.fit(inputs, outputs, epochs=100000, verbose=2)

print("=======================================================================================")

print(model.predict(np.array([ [ 0.01, 0.01 ] ], "float32")))
print(model.predict(np.array([ [ 0.01, 0.99 ] ], "float32")))
print(model.predict(np.array([ [ 0.99, 0.01 ] ], "float32")))
print(model.predict(np.array([ [ 0.99, 0.99 ] ], "float32")))

print("=======================================================================================")

print("      static const N2::NET::Network::SynapsesDataVector   synapses =")
print("      {")
for layer in model.layers:
	print("         {")
	entries = 0
	for weight in layer.get_weights():
		print("            {")
		count = 0
		line  = ""
		for w in np.nditer(weight):
			for x in np.nditer(w):
				line += str(x)
				line += "f, "
				count += 1
				if 5 == count:
					print("               ", line)
					count = 0
					line  = ""
		if 0 < count:
			print("               ", line)
		print("            }, ")
		entries += 1
	if 2 != entries:
		print("            {")
		print("            },")
	print("         }, ")
print("      };")
