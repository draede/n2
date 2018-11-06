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

void kernel Compute(const global float *prevNeurons, unsigned int cPrevNeuronsOffset, unsigned int cPrevNeuronsCount, 
                    const global float *weights, 
                    global float *nextNeurons, unsigned int cNextNeuronsOffset, unsigned int cNextNeuronsCount) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = 0.0f;

	for (unsigned int k = 0; k < cPrevNeuronsCount; k++)
	{
		fValue += prevNeurons[cPrevNeuronsOffset + k] * weights[k * cNextNeuronsCount + idx];
	}
	nextNeurons[cNextNeuronsOffset + idx] = fValue;
}

void kernel ComputeWithBias(float fBias, 
                      const global float *prevNeurons, unsigned int cPrevNeuronsOffset, unsigned int cPrevNeuronsCount, 
                      const global float *weights, const global float *biases, 
                      global float *nextNeurons, unsigned int cNextNeuronsOffset, unsigned int cNextNeuronsCount) 
{
	unsigned int   idx    = get_global_id(0);
	float          fValue = 0.0f;

	for (unsigned int k = 0; k < cPrevNeuronsCount; k++)
	{
		fValue += prevNeurons[cPrevNeuronsOffset + k] * weights[k * cNextNeuronsCount + idx];
	}
	fValue += fBias * biases[idx];
	nextNeurons[cNextNeuronsOffset + idx] = fValue;
}
