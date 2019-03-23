#include <stdio.h>
#include <stdint.h>
#include "dragon.h"

typedef uint64_t u64;
typedef unsigned int u_int;

typedef struct {
	u64 lb;
	u64 rb;
} element;

static u64 M[2] = {0x00004472, 0x61676F6E};

int F_func(u_int *);
u_int G_box(u_int, int);
u_int H_box(u_int, int);

int init_key(u64 *, u64 *, element *, int);
int gen_key(u_int *, u64 *);

u_int G_box(u_int x, int n) {
	
	u_int x_0, x_1, x_2, x_3;
	x_0 = x>>24;
	x_1 = (x<<8)>>24;
	x_2 = (x<<16)>>24;
	x_3 = (x<<24)>>24;
	switch (n) {
		case 1:
			return sbox1[x_0] ^ sbox1[x_1] ^ sbox1[x_2] ^ sbox2[x_3];
		case 2:
			return sbox1[x_0] ^ sbox1[x_1] ^ sbox2[x_2] ^ sbox1[x_3];
		case 3:
			return sbox1[x_0] ^ sbox2[x_1] ^ sbox1[x_2] ^ sbox1[x_3];
		default:
			fprintf(stderr, "Error in G_box: incorrect value n");
			break;
	}
	return 0;
}	
	
u_int H_box(u_int x, int n) {
	
	u_int x_0, x_1, x_2, x_3;
	x_0 = x>>24;
	x_1 = (x<<8)>>24;
	x_2 = (x<<16)>>24;
	x_3 = (x<<24)>>24;
	switch (n) {
		case 1:
			return sbox2[x_0] ^ sbox2[x_1] ^ sbox2[x_2] ^ sbox1[x_3];
		case 2:
			return sbox2[x_0] ^ sbox2[x_1] ^ sbox1[x_2] ^ sbox2[x_3];
		case 3:
			return sbox2[x_0] ^ sbox1[x_1] ^ sbox2[x_2] ^ sbox2[x_3];
		default:
			fprintf(stderr, "Error in H_box: incorrect value n");
			break;
	}
	return 0;
}

int F_func(u_int *A) {

	int i, a;
	for (i = 1; i < 6; i+=2) 
		A[i] ^= A[i-1];	
	
	for (i = 2; i < 7; i+=2) 
		A[i%6] += A[i-1];	
		
	for (i = 3, a = 1; i < 8; i+=2, a++)
		A[i%6] ^= G_box(A[(i-3)%6], a);
	
	for (i = 0, a = 1; i < 5; i+=2, a++)
		A[i] ^= H_box(A[i+1], a); 
	
	for (i = 3; i < 8; i+=2)
		A[i%6] += A[(i-3)%6];

	for (i = 2; i < 7; i+=2) 
		A[i%6] ^= A[i-1];
	
	return 0;	
}

int init_key(u64 *key, u64 *iv, element *W,  int n) {
	
	u64 s_k0 = ((key[0]<<32)^(key[0]>>32));
	u64 s_k1 = ((key[1]<<32)^(key[1]>>32));
	u64 s_iv0 = ((iv[0]<<32)^(iv[0]>>32));
	u64 s_iv1 = ((iv[1]<<32)^(iv[1]>>32));
	
	if (n == 1) {
		W[0].lb = key[0];
	      	W[0].rb = key[1];
		W[1].lb = s_k0 ^ s_iv0;
		W[1].rb = s_k1 ^ s_iv1;
		W[2].lb = iv[0];
		W[2].rb = iv[1];
		W[3].lb = key[0] ^ s_iv0;
		W[3].rb = key[1] ^ s_iv1;
		W[4].lb = s_k0;
		W[4].rb = s_k1;
		W[5].lb = key[0] ^ iv[0];
		W[5].rb = key[1] ^ iv[1];
		W[6].lb = s_iv0;
		W[6].rb = s_iv1;
		W[7].lb = s_k0 ^ iv[0];
		W[7].rb	= s_k1 ^ iv[1];
	}
	else {
		W[0].lb = key[0];
	      	W[0].rb = key[1];
		W[1].lb = key[2];
		W[1].rb = key[3];
		W[2].lb = key[0] ^ iv[0];
		W[2].rb = key[1] ^ iv[1];
		W[3].lb = key[2] ^ iv[2];
		W[3].rb = key[3] ^ iv[3];
		W[4].lb = ~(key[0] ^ iv[0]);
		W[4].rb = ~(key[1] ^ iv[1]);
		W[5].lb = ~(key[2] ^ iv[2]);
		W[5].rb = ~(key[3] ^ iv[3]);	
		W[6].lb = iv[0];
		W[6].rb = iv[1];
		W[7].lb = iv[2];
		W[7].rb	= iv[3];
	}
	
	u_int A[6];
	element t = {0, 0};

	for (int i = 0; i < 16; i++) {
		A[0] = ((W[0].lb ^ W[6].lb ^ W[7].lb)>>32);
		A[1] = (W[0].lb ^ W[6].lb ^ W[7].lb);
		A[2] = ((W[0].rb ^ W[6].rb ^ W[7].rb)>>32);
		A[3] = (W[0].rb ^ W[6].rb ^ W[7].rb);
		A[4] = M[0];
		A[5] = M[1];
		
		F_func(A);
		
		t.lb = (((u64)A[0])<<32) ^ ((u64)A[1]) ^ W[4].lb;
		t.rb = (((u64)A[2])<<32) ^ ((u64)A[3]) ^ W[4].rb;

		for (int j = 7; j > 0; j--)
			W[j] = W[j-1];
		
		W[0] = t;
	
		M[0] = A[4];
		M[1] = A[5];
	}
	return 0;
}

int gen_key(u_int *B, u64 *z) {
	u_int A[6];
	A[0] = B[0];
	A[1] = B[9];
	A[2] = B[16];
	A[3] = B[19];
	A[4] = B[30] ^ M[0];
	A[5] = B[31] ^ M[1];

	F_func(A);

	for (int i = 32; i > 1; i--)
	       B[i] = B[i-2];
	
	B[0] = A[1];
	B[1] = A[2];
	
	//u64 new_M = (((u64)M[0])<<32) ^ ((u64)(M[1]));
	//new_M += 1;
	//M[0] = new_M >> 32;
	//M[1] = (new_M << 32) >> 32;

	M[1] += 1;
	*z = (((u64)(A[0]))<<32) ^ ((u64)(A[4]));
}

		
	
int main(int argc, char **argv) {

	u64 key[2] = {0x0000111122223333, 0x4444555566667777};
	u64 iv[2] = {0x0000111122223333, 0x4444555566667777};

	element W[8];
	init_key(key, iv, W, 1);
	u_int B[32];
	
	int j,k;
	u64 z;
	for (j=0, k=0; k < 8; k++, j+=4) {
		B[j] = W[k].lb >> 32;
		B[j+1] = W[k].lb;
		B[j+2] = W[k].rb >> 32;
		B[j+3] = W[k].rb;
	}

	for(int x = 0; x < 16; x++) {
		gen_key(B, &z); 
		printf("%lx\n", z);
	}

	return 0;
}

