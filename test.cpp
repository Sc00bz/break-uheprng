/*
	Copyright (c) 2018 Steve "Sc00bz" Thomas (steve at tobtu dot com)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <algorithm>

// fails
void uniformEvery24thByteTest(uint32_t previousByte, uint32_t outputType)
{
	uint32_t state;
	uint64_t freq[256] = {0};

	// previousState = previousByte << 24;
	// state = 1768863 * previousState;
	state = 1768863 * (previousByte << 24);

	for (uint32_t i = 0; i < 1 << 24; i++)
	{
		uint32_t nextByte = state >> 24;
		// Min carry to change byte
		uint32_t carry = ((nextByte + 1) << 24) - state;

		if (carry > 1768863)
		{
			carry = 1768863;
		}
		freq[ nextByte            ] += carry;
		freq[(nextByte + 1) & 0xff] += 1768863 - carry;

		// previousState++;
		// state = 1768863 * previousState;
		state += 1768863;
	}

	// I first wrote outputType == 0 and was like oh just 2, I wonder what it looks like
	// Then wrote outputType == 1 and was like oh it's grouped, I wonder what the offsets are
	// Then wrote outputType == 2 and was like oh offsets are (159*byteValue)%256
	if (outputType == 0)
	{
		std::sort(freq, freq + 256);
		printf("lo=%" PRIu64 " hi=%" PRIu64 " count(frequency):", freq[0], freq[255]);
		uint32_t count = 0;
		uint64_t last = freq[0];
		for (uint32_t i = 0; i < 256; i++)
		{
			if (last != freq[i])
			{
				printf(" %u(%" PRIu64 ")", count, last);
				count = 1;
				last = freq[i];
			}
			else
			{
				count++;
			}
		}
		printf(" %u(%" PRIu64 ")", count, last);
		printf("\n");
	}
	else if (outputType == 1)
	{
		printf("\n");
		uint32_t first = 0;
		uint64_t firstFreq = freq[0];
		for (uint32_t i = 1; i < 256; i++)
		{
			if (firstFreq != freq[i])
			{
				printf("% 3u-% 3u: %" PRIu64 "\n", first, i - 1, firstFreq);
				first = i;
				firstFreq = freq[i];
			}
		}
		printf("% 3u-% 3u: %" PRIu64 "\n\n", first, 255, firstFreq);
	}
	else
	{
		uint32_t offset = 0;
		uint64_t firstFreq = freq[0];
		for (uint32_t i = 1; i < 256; i++)
		{
			if (firstFreq != freq[i])
			{
				firstFreq = freq[i];
				if (firstFreq == 115930562560)
				{
					offset = i;
				}
			}
		}
		printf("offset=%u\n", offset);
	}
}

// passes
void uniformCarryTest()
{
	// state with carry
	uint64_t state;
	uint64_t *freq = new uint64_t[1768863];
	
	// realState = 0;
	// state = 1768863 * realState;
	state = 0;

	// Zero freq[]
	for (uint32_t i = 0; i < 1768863; i++)
	{
		freq[i] = 0;
	}

	for (uint64_t i = 0; i < UINT64_C(1) << 32; i++)
	{
		uint32_t carry = state >> 32;
		// Min carry to change next carry
		uint64_t prevCarry = ((state + (UINT64_C(1) << 32)) & UINT64_C(0xffffffff00000000)) - state;

		if (prevCarry > 1768863)
		{
			prevCarry = 1768863;
		}
		freq[carry    ] += prevCarry;
		freq[carry + 1] += 1768863 - prevCarry;

		// realState++;
		// state = 1768863 * realState;
		state += 1768863;
	}
	std::sort(freq, freq + 1768863);
	printf("lo=%" PRIu64 " hi=%" PRIu64 " count(frequency):", freq[0], freq[1768862]);
	uint32_t count = 0;
	uint64_t last = freq[0];
	for (uint32_t i = 0; i < 1768863; i++)
	{
		if (last != freq[i])
		{
			printf(" %u(%" PRIu64 ")", count, last);
			count = 1;
			last = freq[i];
		}
		else
		{
			count++;
		}
	}
	printf(" %u(%" PRIu64 ")", count, last);
	printf("\n");
}

int main()
{
	printf("Uniform Every 24th Byte Test:\n");
	for (uint32_t i = 0; i < 256; i++)
	{
		printf("Byte 0x%02x: ", i);
		uniformEvery24thByteTest(i, 1);
	}

	printf("\nUniform Carry Test:\n");
	uniformCarryTest();

	return 0;
}
