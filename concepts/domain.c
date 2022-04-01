/* Program to illustrate the functionaility of the domainSize calculation
 * If we have N elements we want to split into M groups, what value K
 * of K do we need such that K is the smallest multiple of M greater than N
 * 
 * If N % M = 0 then we require N = K
 */

#include <stdio.h>

int domain(int size, int chunkSize) {
	return ((size - 1) / chunkSize + 1) * chunkSize;
}

int main(int argc, char* argv[]) {
	printf("domain( 98,20) = %5d\n", domain(98, 20));
	printf("domain( 99,20) = %5d\n", domain(99, 20));
	printf("domain(100,20) = %5d\n", domain(100, 20));
	printf("domain(101,20) = %5d\n", domain(101, 20));
	printf("domain(102,20) = %5d\n", domain(102, 20));
	return 0;
}


