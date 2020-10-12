#include <iostream>

void mmx() 
{
	_int8 A[8] = { 7,6,5,4, 3,2, 1,0 };
	_int8 B[8] = { 1,1,1,0, 1,0, 1,1 };
	_int8 C[8] = { 3,4,5,10,7,10,9,10 };
	_int16 D[8] = { 10,10,10,10,10,10,10,10 };

	_int16 F[8] = {};

	_asm 
	{
		movq mm0, qword ptr[A]
		movq mm1, qword ptr[B]
		movq mm2, qword ptr[C]
		movq mm3, qword ptr[D]

		punpcklbw mm0, mm0 //int8 -> int16
		punpcklbw mm1, mm1
		punpcklbw mm2, mm2

		psraw mm0, 8
		psraw mm1, 8
		psraw mm2, 8

		pmullw mm0, mm1 //A*B (first half)
		paddsw mm0, mm2 //A*B+C (first half)
		psubsw mm0, mm3 //A*B+C-D (first half)

		movq  qword ptr[F], mm0

		movq mm0, qword ptr[A + 4]
		movq mm1, qword ptr[B + 4]
		movq mm2, qword ptr[C + 4]
		movq mm3, [D + 8]

		punpcklbw mm0, mm0
		punpcklbw mm1, mm1
		punpcklbw mm2, mm2

		psraw     mm0, 8
		psraw     mm1, 8
		psraw     mm2, 8

		pmullw mm0, mm1 //A*B (sec half)
		paddsw mm0, mm2 //A*B+C (sec half)
		psubsw mm0, mm3 //A*B+C-D (sec half)

		movq  qword ptr[F + 8], mm0
		
		emms
	}

	for (int i = 0; i < 8; i++)
	{
		std::cout << F[i]<<" ";
	}
}

int main()
{
	mmx();

	system("pause");
	return 0;
}