#include "hash.h"
#include "vmem.h"

/* list of primes can be found here
   https://primes.utm.edu/lists/small/10000.txt
*/
static Word primes[] = {
  5009,
  5099,
  5189,
  5281,
  5393,
  5449,
  5527,
  5641,
  5701,
  5801,
  5861,
  5953,
  6067,
  6143,
  6229,
  6311,
  6373,
  6481,
  6577,
  6679,
  6763,
  6841,
  6947,
  7001,
  7109,
  7211
};

Word hashPrimes(Word index) {
  return primes[index];
}

static Word highBit = ((Word)-1) / 2 + 1;

Word hashRotateLeft(Word w) {
  bool g = w >= highBit;
  return (w << 1) + g;
}
