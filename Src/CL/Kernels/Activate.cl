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


//remake this to match exactly the activations from keras

void kernel ActivateSigmoid(global float *neurons, unsigned int cNeuronsOffset) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	neurons[cNeuronsOffset + idx] = 1.0f / (1.0f + exp(-fValue));
}

void kernel ActivateBinaryStep(global float *neurons, unsigned int cNeuronsOffset) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	if (0.0f > fValue)
	{
		neurons[cNeuronsOffset + idx] = 0.0f;
	}
	else
	{
		neurons[cNeuronsOffset + idx] = 1.0f;
	}
}

void kernel ActivateTanH(global float *neurons, unsigned int cNeuronsOffset) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	neurons[cNeuronsOffset + idx] = tanh(fValue);
}

void kernel ActivateArcTan(global float *neurons, unsigned int cNeuronsOffset) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	neurons[cNeuronsOffset + idx] = atan(fValue);
}

void kernel ActivateSoftSign(global float *neurons, unsigned int cNeuronsOffset) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	if (0.0f > fValue)
	{
		neurons[cNeuronsOffset + idx] = fValue / (1.0f + -fValue);
	}
	else
	{
		neurons[cNeuronsOffset + idx] = fValue / (1.0f + fValue);
	}
}

void kernel ActivateISRU(global float *neurons, unsigned int cNeuronsOffset, float fAlpha) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	neurons[cNeuronsOffset + idx] = fValue / sqrt(1.0f + fAlpha * fValue * fValue);
}

void kernel ActivateRELU(global float *neurons, unsigned int cNeuronsOffset) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	if (0.0f > fValue)
	{
		neurons[cNeuronsOffset + idx] = 0.0f;
	}
	else
	{
		neurons[cNeuronsOffset + idx] = fValue;
	}
}

void kernel ActivateLeakyRELU(global float *neurons, unsigned int cNeuronsOffset) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	neurons[cNeuronsOffset + idx] = 0.01f * fValue;
}

void kernel ActivatePRELU(global float *neurons, unsigned int cNeuronsOffset, float fAlpha) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	if (0.0f > fValue)
	{
		neurons[cNeuronsOffset + idx] = fAlpha * fValue;
	}
	else
	{
		neurons[cNeuronsOffset + idx] = fValue;
	}
}

void kernel ActivateELU(global float *neurons, unsigned int cNeuronsOffset, float fAlpha) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	if (0.0f > fValue)
	{
		neurons[cNeuronsOffset + idx] = fAlpha * (exp(fValue) - 1.0f);
	}
	else
	{
		neurons[cNeuronsOffset + idx] = fValue;
	}
}

void kernel ActivateSELU(global float *neurons, unsigned int cNeuronsOffset, float fAlpha, float fLambda) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	if (0.0f > fValue)
	{
		neurons[cNeuronsOffset + idx] = fLambda * fAlpha * (exp(fValue) - 1.0f);
	}
	else
	{
		neurons[cNeuronsOffset + idx] = fLambda * fValue;
	}
}

void kernel ActivateSRELU(global float *neurons, unsigned int cNeuronsOffset, float fTl, float fAl, float fTr, 
                          float fAr) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	if (fValue <= fTl)
	{
		neurons[cNeuronsOffset + idx] = fTl + fAl * (fValue - fTl);
	}
	else
	if (fValue >= fTr)
	{
		neurons[cNeuronsOffset + idx] = fTr + fAr * (fValue - fTr);
	}
	else
	{
		neurons[cNeuronsOffset + idx] = fValue;
	}
}

void kernel ActivateISRLU(global float *neurons, unsigned int cNeuronsOffset, float fAlpha) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	if (0.0f > fValue)
	{
		neurons[cNeuronsOffset + idx] = fValue / sqrt(1.0f + fAlpha * fValue * fValue);
	}
	else
	{
		neurons[cNeuronsOffset + idx] = fValue;
	}
}

void kernel ActivateSoftPlus(global float *neurons, unsigned int cNeuronsOffset) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	neurons[cNeuronsOffset + idx] = log(1.0f + fValue);
}

void kernel ActivateBentIdentity(global float *neurons, unsigned int cNeuronsOffset) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	neurons[cNeuronsOffset + idx] = (sqrt(fValue * fValue + 1.0f) - 1.0f) / 2.0f + fValue;
}

void kernel ActivateSoftExponential(global float *neurons, unsigned int cNeuronsOffset, float fAlpha) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	if (0.0f > fAlpha)
	{
		neurons[cNeuronsOffset + idx] = -log(1.0f - fAlpha * (fValue + fAlpha)) / fAlpha;
	}
	else
	if (0.0f < fAlpha)
	{
		neurons[cNeuronsOffset + idx] = (exp(fAlpha * fValue) - 1.0f) / fAlpha + fAlpha;
	}
	else
	{
		neurons[cNeuronsOffset + idx] = fValue;
	}
}

void kernel ActivateSinusoid(global float *neurons, unsigned int cNeuronsOffset) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	neurons[cNeuronsOffset + idx] = sin(fValue);
}

void kernel ActivateSINC(global float *neurons, unsigned int cNeuronsOffset) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	if (0.0f == fValue)
	{
		neurons[cNeuronsOffset + idx] = 1.0f;
	}
	else
	{
		neurons[cNeuronsOffset + idx] = sin(fValue) / fValue;
	}
}

void kernel ActivateGaussian(global float *neurons, unsigned int cNeuronsOffset) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	neurons[cNeuronsOffset + idx] = exp(- fValue * fValue);
}

void kernel ActivateSoftMax(global float *neurons, unsigned int cNeuronsOffset, float fExpSum) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = neurons[cNeuronsOffset + idx];
	
	neurons[cNeuronsOffset + idx] = exp(fValue) / fExpSum;
}

//not very efficient - single threaded
void kernel ActivateSoftMaxExpSum(global float *neurons, unsigned int cNeuronsOffset, unsigned int cCount, 
                                  global float *expsum) 
{
	float   fExpSum = 0.0f;

	for (unsigned int i = 0; i < cCount; i++)
	{
		fExpSum += exp(neurons[cNeuronsOffset + i]);
	}
	expsum[0] = fExpSum;
}
