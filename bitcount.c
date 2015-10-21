#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

static int BIT_COUNT[256];
static int      bitCountOfByte(int value) {
	return BIT_COUNT[value & 0xFF];
}

int bitcount_with_ands(int u)
{
	unsigned int    uCount;

	uCount = u
	- ((u >> 1) & 033333333333)
		- ((u >> 2) & 011111111111);
	return
		((uCount + (uCount >> 3))
		 & 030707070707) % 63;
}

int other_bitcount_with_ands(int i)
{
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return ((i + (i >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
}

int bitcount_by_lookup(int value) {
	return bitCountOfByte(value)
	+ bitCountOfByte(value >> 8)
	+ bitCountOfByte(value >> 16)
	+ bitCountOfByte(value >> 24);
}

int bitcount_kerningham(int n) {     
	int c; // c accumulates the total bits set in v
	for (c = 0; n; c++) 
		n &= n - 1; // clear the least significant bit set
	return c;
}

#define ITERATIONS 100000000

void benchmark_function(int (*method_under_test)(int value), int seed)
{
	clock_t start, stop;
	
	srand(seed);
	start = clock();
	long count = 0;
	
	for (int i = 0; i < ITERATIONS; i++) {
		count += method_under_test(rand());
	}
	
	stop = clock();
	
	printf("elapsed: %ld microseconds, count = %ld \n", stop - start, count);
}

void initialize_count() {	
	for (int i = 0; i < sizeof(BIT_COUNT)/sizeof(int); i++) {
		unsigned int v = i; // count the number of bits set in v
		unsigned int c; // c accumulates the total bits set in v
		
		for (c = 0; v; v >>= 1)
		{
			c += v & 1;
		}
		
		BIT_COUNT[i] = c;
	}
}

int main() {
	initialize_count();
	
	printf("lookup:  ");
	benchmark_function(bitcount_by_lookup, 7);
	printf("ands:  ");
	benchmark_function(bitcount_with_ands, 7);
	printf("more ands:  ");
	benchmark_function(other_bitcount_with_ands, 7);
	printf("kerningham:  ");
	benchmark_function(bitcount_kerningham, 7);
	// printf("__builtin_popcount:  ");
	// benchmark_function(__builtin_popcount, 7);

	return 0;
}
