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

function getUheprngState(random)
{
	var tmp, i;
	var states = Array(2*48+2);
	var carries = Array(2*48+2);
	var state = Array(48);
	var carry;
	var twoPow53 = Math.pow(2, 53);
	var twoPow21 = Math.pow(2, 21);
	var twoPow11 = Math.pow(2, 11);
	var twoPow32 = Math.pow(2, 32);

	// Get 49 random values
	// Find even states and partial odd states
	for (i = 0; i < 2*48+2; i += 2)
	{
		tmp = random(twoPow53);                      // 53 bits
		states[i] = Math.floor(tmp / twoPow21);      // 32 bits
		states[i + 1] = (tmp * twoPow11) % twoPow32; // 21 bits
	}

	// Find carries (47 thru 2*48-1)
	for (i = 48; i < 2*48; i += 2)
	{
		carries[i-1] = (states[i] - 1768863 * states[i - 48]) | 0;
		carries[i] = ((1768863 * states[i - 48] + carries[i-1]) / twoPow32) | 0;
	}
	carries[2*48-1] = (states[2*48] - 1768863 * states[2*48 - 48]) | 0;

	// Find full odd states (49 thru 2*48-1)
	for (i = 49; i < 2*48; i += 2)
	{
		states[i - 48] = Math.round((carries[i] * twoPow32 + states[i] - carries[i - 1]) / 1768863);
		states[i] = (1768863 * states[i - 48] + carries[i - 1]) % twoPow32;
	}

	// Set state to 2*48
	for (i = 0; i < 48; i++)
	{
		state[i] = states[48 + i] / twoPow32;
	}
	carry = carries[2*48 - 1];

	// Increment state to 2*48+2
	tmp = 1768863 * state[0] + carry / twoPow32;
	state[0] = tmp - (carry = tmp | 0);
	tmp = 1768863 * state[1] + carry / twoPow32;
	state[1] = tmp - (carry = tmp | 0);

	return {"state": state, "carry": carry, "pos": 1};
}

function rawprng(state)
{
	if (++state.pos >= 48)
	{
		state.pos = 0;
	}
	var tmp = 1768863 * state.state[state.pos] + state.carry * 2.3283064365386963e-10; // 2^-32
	return state.state[state.pos] = tmp - (state.carry = tmp | 0);
}

function uheprngRandom(range, state)
{
	return Math.floor(range * (rawprng(state) + (rawprng(state) * 0x200000 | 0) * 1.1102230246251565e-16)); // 2^-53
}

function ge(id)
{
	return document.getElementById(id);
}

function breakUheprng(count)
{
	var random, uheprngState, i, o, guess, real, text, correct = 0, wrong = 0;
	var twoPow53 = Math.pow(2, 53);

	random = uheprng();
	uheprngState = getUheprngState(random);

	o = ge("breakUheprng");
	o.innerHTML = "";
	count = count | 0;
	for (i = 0; i < count; i++)
	{
		guess = uheprngRandom(twoPow53, uheprngState);
		text = "guess: " + guess + "<br />\n";

		real = random(twoPow53);
		text = text + "real: " + real + "<br />\n";

		if (guess == real)
		{
			correct++;
			if (correct > 101)
			{
				text = "";
			}
			else if (correct == 101)
			{
				text = "...";
			}
			else
			{
				text = text + "#" + (i+1) + ' Guessed <span style="color:#0f0">correctly</span><br />\n<br />\n';
			}
		}
		else
		{
			wrong++;
			if (wrong > 101)
			{
				text = "";
			}
			else if (wrong == 101)
			{
				text = "...";
			}
			else
			{
				text = text + "#" + (i+1) + ' Guessed <span style="color:#f00">wrong</span><br />\n<br />\n';
			}
		}
		o.innerHTML = o.innerHTML + text;
	}
	o.innerHTML = "Guessed correctly " + correct + "/" + count + "<hr />\n" + o.innerHTML;
}
