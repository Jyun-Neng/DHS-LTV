#include "ltv.h"


using namespace std;


int main(){

	int M 			= Modulus_M;			// Mth Cyclotomic Polynomial, n=euler_phi(M)
	ZZ p 			= to_ZZ("2");	// Message modulus
	int wordsize	= 16;			// radixsize. when computing relinearization we attach wordsize bits. larger makes it faster, smaller evaluaation keysize. however cause larger noise. usually 16 is limit, after that need to cut more bits.
	int numLevel	= Num_Primes-1;			// Number of levels
	int numPrimes	= numLevel+1;	// Num of Primes is equal to Number of levels + 1
	int logq		= Dif_Prime;			// Prime size which is also noise cutting size.
	int Maxlogq		= numPrimes*logq; //Maximum bitsize of the modulus q.
	int tableNum 	= 1;

	ltv n(M, numPrimes, Maxlogq, tableNum, p, wordsize);
	n.Func_SetModulus();
	n.Func_ModulusFindRing(numPrimes, Maxlogq, logq, *n.Pointer_PolyModulus());
	n.Func_ComputeKeysRingRelinFFT(numPrimes, tableNum);

	ZZX m, c;
	m = 1;

	c = n.Prim_Encrypt(m, 0);
	//Do 40 levels of multiplication using the same table without reducing the evaluation key to next level
	for(int i=0; i<numLevel; i++){

		c = c*c;
		c = n.Arith_PolyCoeffReduce(c,i);
		c = n.Prim_RelinRingFFT(c, i);
		c = n.Arith_PolyCoeffReduce(c,i);
		c = n.Prim_ModSwitch(c, i);
		c = n.Arith_PolyCoeffReduce(c, i+1);
		m = n.Prim_Decrypt(c, i+1);

		cout << "Level " << i << "\t" << m << endl;
	}


	c = n.Prim_Encrypt(m, 0);
	//Do 40 levels of multiplication using the same table by reducing the evaluation keys at each level.
	// Faster relinearization computation for each level but we spend time to reduce the evaluation keys to the next level.
	for(int i=0; i<numLevel; i++){

			c = c*c;
			c = n.Arith_PolyCoeffReduce(c,i);
			c = n.Prim_RelinRingFFT(c, i);
			c = n.Arith_PolyCoeffReduce(c,i);
			c = n.Prim_ModSwitch(c, i);
			///////////////////////////////////////
			n.Prim_ReduceKeysLevel(i+1);
			///////////////////////////////////////
			c = n.Arith_PolyCoeffReduce(c, i+1);
			m = n.Prim_Decrypt(c, i+1);

			cout << "Level " << i << "\t" << m << endl;
	}



	return 0;
}



