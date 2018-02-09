# UHEPRNG is Broken
UHEPRNG (Ultra High Entropy Pseudo-Random Number Generator) is not a CSPRNG. A property of a CSPRNG is that you can't guess a future bit with better accuracy than 50%. This code predicts future values. It exploits the fact that `rawprng()` returns the internal state and `random()` cycles through the internal state every 24 calls.

## Bad Seed
It is seeded by `Math.random()`. Which is a poor source of random.

## Unevenly Distribution
UHEPRNG's main function `random()` is not evenly distributed. To generate a random number in a range that is not a nice binary number (2\*\*n) you should do something like https://github.com/Sc00bz/ModRandom/blob/master/random.js.

## Slight Bias Between Every 24th Byte
When getting a byte at a time there's a slight bias between every 24th byte of output. I did a frequency analysis of what could come after a 0 byte, and byte values 0 to 158 are more likely 6910/1768863 vs 6909/1768863 for the others. There is this same bias after any byte value it's just toward different byte values. In general for byte value `x` the start of the 159 bytes that are more likely start at byte value `(159*x) mod 256` and wrap around and all others are less likely. For byte value 1 start is 159. So byte values 159 through 255 and 0 through 61 have a probability of 6910/1768863 and byte values 62 through 158 have a probability of 6909/1768863. I used [test.cpp](test.cpp) to find these probabilities.

## Can Return NaN
`rawprng()` can return NaN. If during seeding `Math.random()` returns 0, then `mash()` doesn't return a value and `s[i]` is set to undefined. When `rawprng()` is called and `s[p]` is undefined, `s[p]` is set to NaN and NaN is returned. When calling UHEPRNG's main function `random()` and the first `rawprng()` returns NaN then `random()` will return NaN. The probability that `random()` is `1-(1-1/2**53)**24` or 1 in about 2\*\*48.4150 (about 375.3 trillion). So if everyone in the world initialized UHEPRNG 50,000 times then it's likely someone was returned NaN when calling `random()`.
