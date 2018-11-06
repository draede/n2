import numpy as np
import tensorflow as tf
from tensorflow.keras import layers


inputs  = np.array([ [ 0.01, 0.01 ], [ 0.01, 0.99 ], [ 0.99, 0.01 ], [ 0.99, 0.99 ] ], "float32")

outputs = np.array([ [ 0.01 ], [ 0.99 ] , [ 0.99 ], [ 0.01 ] ], "float32")

model   = tf.keras.Sequential()

model.add(layers.Dense(8, activation='relu', use_bias=True, bias_initializer=tf.keras.initializers.Constant(0.1), input_dim=2))
model.add(layers.Dense(1, activation='sigmoid', use_bias=True, bias_initializer=tf.keras.initializers.Constant(0.1)))

print("Compiling...")

model.compile(loss='mean_squared_error', optimizer='adam')

print("Training...")

model.fit(inputs, outputs, epochs=20000, verbose=2)

print(model.predict(inputs).round())

for layer in model.layers:
	print("=== config ==================")
	print(layer.get_config())
	print("=== weights =================")
	print(layer.get_weights())

