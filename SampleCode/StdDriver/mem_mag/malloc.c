#include "malloc.h"
#include "stdio.h"


//���s��(32�r�`���)
__align(32) uint8_t mem1base[MEM1_MAX_SIZE];                                                    //����SRAM���s��
//__align(32) uint8_t mem2base[MEM2_MAX_SIZE] __attribute__((at(0X68000000)));                  //�~��SRAM���s��
//__align(32) uint8_t mem3base[MEM3_MAX_SIZE] __attribute__((at(0X10000000)));                  //����CCM���s��
//���s�޲z��
uint32_t mem1mapbase[MEM1_ALLOC_TABLE_SIZE];                                                    //����SRAM���s��MAP
//u16 mem2mapbase[MEM2_ALLOC_TABLE_SIZE] __attribute__((at(0X68000000+MEM2_MAX_SIZE))); //�~��SRAM���s��MAP
//u16 mem3mapbase[MEM3_ALLOC_TABLE_SIZE] __attribute__((at(0X10000000+MEM3_MAX_SIZE))); //����CCM���s��MAP
//���s�޲z�Ѽ�
const uint32_t memtblsize[SRAMBANK] = {MEM1_ALLOC_TABLE_SIZE};  //���s��j�p
const uint32_t memblksize[SRAMBANK] = {MEM1_BLOCK_SIZE};                //���s�����j�p
const uint32_t memsize[SRAMBANK] = {MEM1_MAX_SIZE};                         //���s�`�j�p


//���s�޲z���
struct _m_mallco_dev mallco_dev =
{
    my_mem_init,                        //���s��l��
    my_mem_perused,                     //���s�ϥβv
    mem1base,           //���s��
    mem1mapbase,//���s�޲z���A��
    0,                  //���s�޲z���N��
};

//�ƻs���s
//*des:�ت��a�}
//*src:���a�}
//n:�ݭn�ƻs�����s����(�r�`�����)
void mymemcpy(void *des, void *src, uint32_t n)
{
    uint8_t *xdes = des;
    uint8_t *xsrc = src;

    while (n--)*xdes++ = *xsrc++;
}
//�]�m���s
//*s:���s���a�}
//c :�n�]�m����
//count:�ݭn�]�m�����s�j�p(�r�`�����)
void mymemset(void *s, uint8_t c, uint32_t count)
{
    uint8_t *xs = s;

    while (count--)*xs++ = c;
}
//���s�޲z��l��
//memx:���ݤ��s��
void my_mem_init(uint8_t memx)
{
    mymemset(mallco_dev.memmap[memx], 0, memtblsize[memx] * 2); //���s���A��ƾڲM�s
    mymemset(mallco_dev.membase[memx], 0, memsize[memx]);   //���s���Ҧ��ƾڲM�s
    mallco_dev.memrdy[memx] = 1;                            //���s�޲z��l��OK
}
//������s�ϥβv
//memx:���ݤ��s��
//��^��:�ϥβv(0~100)
uint16_t my_mem_perused(uint8_t memx)
{
    uint32_t used = 0;
    uint32_t i;

    for (i = 0; i < memtblsize[memx]; i++)
    {
        if (mallco_dev.memmap[memx][i])used++;
    }

    return (used * 100) / (memtblsize[memx]);
}
//���s���t(�����ե�)
//memx:���ݤ��s��
//size:�n���t�����s�j�p(�r�`)
//��^��:0XFFFFFFFF,�N����~;��L,���s�����a�}
uint32_t my_mem_malloc(uint8_t memx, uint32_t size)
{
    signed long offset = 0;
    uint32_t nmemb; //�ݭn�����s����
    uint32_t cmemb = 0; //�s��Ť��s����
    uint32_t i;

    if (!mallco_dev.memrdy[memx])mallco_dev.init(memx); //����l��,�������l��

    if (size == 0)return 0XFFFFFFFF; //���ݭn���t

    nmemb = size / memblksize[memx]; //����ݭn���t���s�򤺦s����

    if (size % memblksize[memx])nmemb++;

    for (offset = memtblsize[memx] - 1; offset >= 0; offset--) //�j����Ӥ��s�����
    {
        if (!mallco_dev.memmap[memx][offset])cmemb++; //�s��Ť��s���ƼW�[
        else cmemb = 0;                             //�s�򤺦s���M�s

        if (cmemb == nmemb)                         //���F�s��nmemb�ӪŤ��s��
        {
            for (i = 0; i < nmemb; i++)             //�Ъ`���s���D��
            {
                mallco_dev.memmap[memx][offset + i] = nmemb;
            }

            return (offset * memblksize[memx]); //��^�����a�}
        }
    }

    return 0XFFFFFFFF;//�����ŦX���t���󪺤��s��
}
//���񤺦s(�����ե�)
//memx:���ݤ��s��
//offset:���s�a�}����
//��^��:0,���񦨥\;1,���񥢱�;
uint8_t my_mem_free(uint8_t memx, uint32_t offset)
{
    int i;

    if (!mallco_dev.memrdy[memx]) //����l��,�������l��
    {
        mallco_dev.init(memx);
        return 1;//����l��
    }

    if (offset < memsize[memx]) //�����b���s����.
    {
        int index = offset / memblksize[memx];      //�����Ҧb���s�����X
        int nmemb = mallco_dev.memmap[memx][index]; //���s���ƶq

        for (i = 0; i < nmemb; i++)                 //���s���M�s
        {
            mallco_dev.memmap[memx][index + i] = 0;
        }

        return 0;
    }
    else return 2; //�����W�ϤF.
}
//���񤺦s(�~���ե�)
//memx:���ݤ��s��
//ptr:���s���a�}
void myfree(uint8_t memx, void *ptr)
{
    uint32_t offset;

    if (ptr == NULL)return; //�a�}��0.

    offset = (uint32_t)ptr - (uint32_t)mallco_dev.membase[memx];
    my_mem_free(memx, offset);  //���񤺦s
	  ptr=(void *)0;
}
//���t���s(�~���ե�)
//memx:���ݤ��s��
//size:���s�j�p(�r�`)
//��^��:���t�쪺���s���a�}.
void *mymalloc(uint8_t memx, uint32_t size)
{
    uint32_t offset;
    offset = my_mem_malloc(memx, size);

    if (offset == 0XFFFFFFFF)
        return NULL;
    else return (void *)((uint32_t)mallco_dev.membase[memx] + offset);
}
//���s���t���s(�~���ե�)
//memx:���ݤ��s��
//*ptr:�¤��s���a�}
//size:�n���t�����s�j�p(�r�`)
//��^��:�s���t�쪺���s���a�}.
void *myrealloc(uint8_t memx, void *ptr, uint32_t size)
{
    uint32_t offset;
    offset = my_mem_malloc(memx, size);

    if (offset == 0XFFFFFFFF)
    {
        printf("\r\nrealloc_Err");
        return NULL;
    }

    else
    {
        mymemcpy((void *)((uint32_t)mallco_dev.membase[memx] + offset), ptr, size); //�����¤��s���e��s���s
        myfree(memx, ptr);                                                  //�����¤��s
        return (void *)((uint32_t)mallco_dev.membase[memx] + offset);               //��^�s���s���a�}
    }
}












