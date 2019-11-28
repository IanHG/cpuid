#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdint.h> /* for uint32_t */
#include <bitset>

#define cpu_true  1
#define cpu_false 0

/**
 * cpuid regestry struct. Hold input/output of registers
 *
 *    eax, ebx, ecx, edx
 *
 **/
struct cpuid_reg
{
   uint32_t eax;
   uint32_t ebx;
   uint32_t ecx;
   uint32_t edx;
};

/**
 * Call 'cpuid' 
 *
 * @return  Returns status '0' if ok.
 **/
int cpuid
   (  uint32_t* eax 
   ,  uint32_t* ebx 
   ,  uint32_t* ecx 
   ,  uint32_t* edx 
   )
{
   asm volatile ("cpuid" :
         "=a" (*eax),
         "=b" (*ebx),
         "=c" (*ecx),
         "=d" (*edx)
         : "a" (*eax), "c" (*ecx)
         //: "0" (*eax), "2" (*ecx)
      );
   
   //if(*eax == 0)
   //{
   //   return 0; 
   //}
   return 0; // exit ok
}

/**
 *
 **/
int cpuid
   (  cpuid_reg* reg
   )
{
   return cpuid(&(reg->eax), &(reg->ebx), &(reg->ecx), &(reg->edx));
}

/**
 * Get CPU vendor ID.
 **/
int cpu_vendor_id
   (  char* c
   ,  int   size
   )
{
   if(size < 12)
   {
      return -1;
   }

   cpuid_reg reg = { 0x0 };

   int status = cpuid(&reg);

   if(status == 0)
   {
      memcpy(c    , &(reg.ebx), 4);
      memcpy(c + 4, &(reg.edx), 4);
      memcpy(c + 8, &(reg.ecx), 4);
   }

   return status;
}

/**
 *
 **/
int cpu_model_name
   (  char* c
   ,  int   size
   )
{
   if(size < 48)
   {
      return -1;
   }
   
   cpuid_reg reg[3] = 
      {  {  0x80000002}
      ,  {  0x80000003}
      ,  {  0x80000004}
      };
   
   int status1 = cpuid(&(reg[0].eax), &(reg[0].ebx), &(reg[0].ecx), &(reg[0].edx));
   int status2 = cpuid(&(reg[1].eax), &(reg[1].ebx), &(reg[1].ecx), &(reg[1].edx));
   int status3 = cpuid(&(reg[2].eax), &(reg[2].ebx), &(reg[2].ecx), &(reg[2].edx));

   memcpy(c, &reg, sizeof(reg));
   
   if(status1 == 0 && status2 == 0 && status3 == 0)
   {
      return 0;
   }
   else
   {
      return -1;
   }
}

/**
 *
 **/
int cpu_has_avx2()
{
   cpuid_reg reg = { 0x7, 0x0, 0x0 };

   if(cpuid(&reg) == 0)
   {
      if(reg.ebx & (1<<(5)))
      {
         return cpu_true;
      }
   }

   return cpu_false;
}

/**
 *
 **/
int cpu_has_avx512()
{
   cpuid_reg reg = { 0x7, 0x0, 0x0 };

   if(cpuid(&reg) == 0)
   {
      if(reg.ebx & (1<<(15)))
      {
         return cpu_true;
      }
   }

   return cpu_false;
}

/**
 * Main function
 **/
int main()
{
   char c[13];
   cpu_vendor_id(c, sizeof(c));
   c[12] = '\0';
   
   std::cout << cpu_has_avx2() << std::endl;
   std::cout << cpu_has_avx512() << std::endl;
   
   char model_name[48];
   cpu_model_name(model_name, sizeof(model_name));

   printf("Model name: %s\n", model_name);

   return 0;
}
