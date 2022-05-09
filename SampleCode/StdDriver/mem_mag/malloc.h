#ifndef __MALLOC_H
#define __MALLOC_H
#include "NuMicro.h"


#ifndef NULL
    #define NULL 0
#endif

//�w�q�T�Ӥ��s��
#define SRAMIN   0      //�������s��



#define SRAMBANK    1   //�w�q�����SRAM����.   


//mem1���s�ѼƳ]�w.mem1�����B�󤺳�SRAM�̭�.
#define MEM1_BLOCK_SIZE         32                              //���s���j�p��32�r�`
#define MEM1_MAX_SIZE           64*1024                         //�̤j�޲z���s 64K
#define MEM1_ALLOC_TABLE_SIZE   MEM1_MAX_SIZE/MEM1_BLOCK_SIZE   //���s��j�p

////mem2���s�ѼƳ]�w.mem2�����s���B��~��SRAM�̭�
//#define MEM2_BLOCK_SIZE           32                              //���s���j�p��32�r�`
//#define MEM2_MAX_SIZE         960 *1024                       //�̤j�޲z���s960K
//#define MEM2_ALLOC_TABLE_SIZE MEM2_MAX_SIZE/MEM2_BLOCK_SIZE   //���s��j�p
//
////mem3���s�ѼƳ]�w.mem3�B��CCM,�Ω�޲zCCM(�S�O�`�N,�o����SRAM,��CPU�i�H�X��!!)
//#define MEM3_BLOCK_SIZE           32                              //���s���j�p��32�r�`
//#define MEM3_MAX_SIZE         60 *1024                        //�̤j�޲z���s60K
//#define MEM3_ALLOC_TABLE_SIZE MEM3_MAX_SIZE/MEM3_BLOCK_SIZE   //���s��j�p
//


//���s�޲z���
struct _m_mallco_dev
{
    void (*init)(uint8_t);                  //��l��
    uint16_t (*perused)(uint8_t);                   //���s�ϥβv
    uint8_t     *membase[SRAMBANK];             //���s�� �޲zSRAMBANK�Ӱϰ쪺���s
    uint32_t *memmap[SRAMBANK];                 //���s�޲z���A��
    uint8_t  memrdy[SRAMBANK];              //���s�޲z�O�_�N��
};
extern struct _m_mallco_dev mallco_dev;  //�bmallco.c�̭��w�q

void mymemset(void *s, uint8_t c, uint32_t count);  //�]�m���s
void mymemcpy(void *des, void *src, uint32_t n); //�ƻs���s
void my_mem_init(uint8_t memx);             //���s�޲z��l�ƨ��(�~/�����ե�)
uint32_t my_mem_malloc(uint8_t memx, uint32_t size); //���s���t(�����ե�)
uint8_t my_mem_free(uint8_t memx, uint32_t offset);     //���s����(�����ե�)
uint16_t my_mem_perused(uint8_t memx);              //��o���s�ϥβv(�~/�����ե�)
////////////////////////////////////////////////////////////////////////////////
//�Τ�եΨ��
void myfree(uint8_t memx, void *ptr);           //���s����(�~���ե�)
void *mymalloc(uint8_t memx, uint32_t size);        //���s���t(�~���ե�)
void *myrealloc(uint8_t memx, void *ptr, uint32_t size); //���s���t���s(�~���ե�)
#endif













