//
// Utility to check for rdrand support on Intel CPUs.
//   to compile: gcc -o rdrand rdrand.c
//

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct {
	unsigned int EAX;
	unsigned int EBX;
	unsigned int ECX;
	unsigned int EDX;
} CPUIDinfo;

void CPUID( CPUIDinfo *info, const unsigned int func, const unsigned int subfunc ) {
	asm(".intel_syntax noprefix\n");
	asm("mov r8, rdi\n");
	asm("mov r9, rsi\n");
	asm("mov r10, rdx\n");
	asm("push rax\n");
	asm("push rbx\n");
	asm("push rcx\n");
	asm("push rdx\n");
	asm("mov eax, r9d\n");
	asm("mov ecx, r10d\n");
	asm("cpuid;\n");
	asm("mov DWORD PTR [r8], eax\n");
	asm("mov DWORD PTR [r8+4], ebx\n");
	asm("mov DWORD PTR [r8+8], ecx\n");
	asm("mov DWORD PTR [r8+12], edx\n");
	asm("pop rdx\n");
	asm("pop rcx\n");
	asm("pop rbx\n");
	asm("pop rax\n");
	asm(".att_syntax prefix\n");
}

typedef unsigned int DWORD;

int rdrand_check_support() {
	unsigned int j;
	DWORD maxCPUID;
	CPUIDinfo info;
	int got_intel_cpu=0;
	CPUID( &info,0,0 );

	if(memcmp( ( char * )( &info.EBX ), "Genu", 4 ) == 0 &&
	   memcmp( ( char * )( &info.EDX ), "ineI", 4 ) == 0 &&
	   memcmp( ( char * )( &info.ECX), "ntel", 4 ) == 0 ) {
		got_intel_cpu = 1;
	}

	if ( got_intel_cpu ) {
		CPUID(&info,1,0);
		if ( ( info.ECX & 0x40000000 ) == 0x40000000 ) return 1;
	}
	return 0;
}

int main() {
	if ( rdrand_check_support() == 1 ) {
		printf( "RDRAND SUPPORTED\n" );
		return( 0 );
	} else {
		printf( "RDRAND NOT SUPPORTED\n" );
		return( -1 );
	}
}
