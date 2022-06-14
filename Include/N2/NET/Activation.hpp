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

#pragma once


#include "CX/Types.hpp"
#include "CX/Status.hpp"


namespace N2
{

namespace NET
{

typedef CX::UInt16               ActivationType;

struct Activation
{
	static const ActivationType   MIN_VALUE       = 1;

	static const ActivationType   Identity        = 1;
	static const ActivationType   Sigmoid         = 2;
	static const ActivationType   BinaryStep      = 3;
	static const ActivationType   TanH            = 4;
	static const ActivationType   ArcTan          = 5;
	static const ActivationType   SoftSign        = 6;
	static const ActivationType   RELU            = 7;
	static const ActivationType   LeakyRELU       = 8;
	static const ActivationType   SoftPlus        = 9;
	static const ActivationType   BentIdentity    = 10;
	static const ActivationType   Sinusoid        = 11;
	static const ActivationType   SINC            = 12;
	static const ActivationType   Gaussian        = 13;
	static const ActivationType   ISRU            = 14;
	static const ActivationType   PRELU           = 15;
	static const ActivationType   ELU             = 16;
	static const ActivationType   SELU            = 17;
	static const ActivationType   SRELU           = 18;
	static const ActivationType   ISRLU           = 19;
	static const ActivationType   SoftExponential = 20;
	static const ActivationType   SoftMax         = 21;

	static const ActivationType   MAX_VALUE       = 13;

	//ISRLU
	//SELU
	//PRELU
	//ISRU
	//ELU
	//SRELU
	//SoftExponential
	//SoftMax;
};

}//namespace NET

}//namespace N2
