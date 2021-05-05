#include <libepc.h>

void llmultiply(unsigned long long int l1,
                unsigned long long int l2,
                unsigned char *result);

struct test_case {
  unsigned long long int a;
  unsigned long long int b;
  unsigned long long int rh;
  unsigned long long int rl;
};

struct test_case cases[6] = {

  { 0x0000111122223333ULL, 0x0000555566667777ULL,
    0x0000000005B061D9ULL, 0x58BF0ECA7C0481B5ULL },

  { 0x3456FEDCAAAA1000ULL, 0xEDBA00112233FF01ULL,
    0x309A912AF7188C57ULL, 0xE62072DD409A1000ULL },

  { 0xFFFFEEEEDDDDCCCCULL, 0xBBBBAAAA99998888ULL,
    0xBBBB9E2692C5DDDCULL, 0xC28F7531048D2C60ULL },

  { 0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL,
    0xFFFFFFFFFFFFFFFEULL, 0x0000000000000001ULL },

  { 0x00000001FFFFFFFFULL, 0x00000001FFFFFFFFULL,
    0x0000000000000003ULL, 0xFFFFFFFC00000001ULL },

  { 0xFFFEFFFFFFFFFFFFULL, 0xFFFF0001FFFFFFFFULL,
    0xFFFE0002FFFDFFFEULL, 0x0001FFFE00000001ULL }
};

// long = 32 bits
// long long = 64 bits

void cmultiply(unsigned long long int l1,
               unsigned long long int l2,
               unsigned char *result)
{
  unsigned long int AL = l1; //Tar den låga delen
  unsigned long int AH = l1 >> 32;
  unsigned long int BL = l2; //Tar den låga delen
  unsigned long int BH = l2 >> 32;

  // Kolla så att det går att optimera asm och c för sig / Henkert

  unsigned long int RLL = 0;
  unsigned long int RLH = 0;
  unsigned long int RHL = 0;
  unsigned long int RHH = 0;
  unsigned long long int RTMP = 0;
  unsigned long int CC1 = 0;
  unsigned long int CC2 = 0;
  unsigned int carry = 0;

  unsigned long int* Result_ptr = ((unsigned long int*) result);
  Result_ptr[0] = 0;
  Result_ptr[1] = 0;
  Result_ptr[2] = 0;
  Result_ptr[3] = 0;

  //AL * BL
  RTMP = (unsigned long long int)AL*(unsigned long long int)BL;
  RLL = (unsigned long int) RTMP ; // RLL DONE
  RLH = (unsigned long int) (RTMP >> 32);

  //AL * BH
  RTMP = (unsigned long long int)AL*(unsigned long long int)BH;
  CC1 = RLH;
  CC2 = RHL;
  RLH = RLH + (unsigned long int) RTMP;
  carry = RLH < CC1;
  RHL = carry + (unsigned long int) (RTMP >> 32);
  carry = RHL < CC2;
  RHH = carry + RHH;

  //AH * BL
  RTMP = (unsigned long long int)AH*(unsigned long long int)BL;
  CC1 = RLH;
  CC2 = RHL;
  RLH = RLH + (unsigned long int)RTMP; // RLH DONE
  carry = RLH < CC1;
  RHL = carry + RHL + (unsigned long int)(RTMP >> 32);
  carry = RHL < CC2;
  RHH = carry + RHH;

  //AH * BH
  RTMP = (unsigned long long int)AH*(unsigned long long int)BH;
  CC1 = RHL;
  CC2 = RHH;
  RHL = RHL + (unsigned long int)RTMP; // RHL DONE
  carry = RHL < CC1;
  RHH = carry + RHH + (unsigned long int)(RTMP >> 32);

  Result_ptr[0] = RLL;
  Result_ptr[1] = RLH;
  Result_ptr[2] = RHL;
  Result_ptr[3] = RHH;
}

void PutUnsignedLongLong(unsigned long long int* ulli)
{
  unsigned long int* uli = (unsigned long int*)ulli;
  PutUnsigned(uli[1], 16, 8);
  PutUnsigned(uli[0], 16, 8);
}

int main(int argc, char *argv[])
{
  unsigned char result[16]; // char = byte
  unsigned char result1[16];
  int i;

  ClearScreen(0x07);
  SetCursorPosition(0, 0);

  QWORD64 CL_start = 0;
  QWORD64 CL_end = 0;


///////////////// C
  CL_start = CPU_Clock_Cycles();
  for (i = 0; i < 100000; ++i)
  {
    cmultiply(cases[i%6].a, cases[i%6].b, result);
  }
  CL_end = (CPU_Clock_Cycles() - CL_start)/100000;
  PutString("  C clock cycles: ");
  PutUnsigned(CL_end, 10, 16);
  PutString("\r\n");


//////////////// ASM
  CL_start = CPU_Clock_Cycles();
  for (i = 0; i < 100000; ++i)
  {
    llmultiply(cases[i%6].a, cases[i%6].b, result);
  }
  CL_end = (CPU_Clock_Cycles() - CL_start)/100000;
  PutString("ASM clock cycles: ");
  PutUnsigned(CL_end, 10, 16);
  /*
*/
/*
  for (i = 0; i < 6; ++i)
  {
    PutString("Test : ");
    PutUnsignedLongLong(&cases[i].a);
    PutString(" * ");
    PutUnsignedLongLong(&cases[i].b);
    PutString("\r\n");

    PutString("    == ");
    PutUnsignedLongLong(&cases[i].rh);
    PutUnsignedLongLong(&cases[i].rl);
    PutString("\r\n");

    llmultiply(cases[i].a, cases[i].b, result);

    PutString("Result ");
    PutUnsignedLongLong(&result[8]);
    PutUnsignedLongLong(&result[0]);
    PutString("\r\n");

    cmultiply(cases[i].a, cases[i].b, result1);

    PutString("Result ");
    PutUnsignedLongLong(&result[8]);
    PutUnsignedLongLong(&result[0]);
    PutString("\r\n");

  }
*/

  return 0;
}
//whippa nae nae på din grav grav
