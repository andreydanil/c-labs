/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))


#define MINBLOCKSIZE ((int)(TAGSIZE+ALIGNMENT))
#define BIGBLOCKSIZE (4096)
#define NUM8BINS (BIGBLOCKSIZE/ALIGNMENT)
#define NUMBIGBINS 24
#define NUMBINS (NUM8BINS+NUMBIGBINS)//0-128 for payload<1024, 25for < 40,
/* rounds up to the nearest multiple of ALIGNMENT, or multiple
 * of BIGPAYLOADSIZE bytes for large payloads in accordance with bin structure */
#define ALIGN(size) ((size) > BIGBLOCKSIZE ? ((size) > MINBLOCKSIZE*NUMBIGBINS ? ((((size+MINBLOCKSIZE)|(BIGBLOCKSIZE-1))+1)&~0x7) : (((size+MINBLOCKSIZE)|(BIGBLOCKSIZE-1))+1) & ~0x7 ): (((size) + (MINBLOCKSIZE-1)) & ~0x7))

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    int newsize = ALIGN(size + SIZE_T_SIZE);
    void *p = mem_sbrk(newsize);
    if ((long)p == -1)
	return NULL;
    else {
        *(size_t *)p = size;
        return (void *)((char *)p + SIZE_T_SIZE);
    }
}

/*
 * mm_free - Freeing a block adds header tags to blocks
 */
void mm_free(void *ptr)
{
    ptr=((char*)ptr-ALIGNMENT);
    ((aheader_t*)ptr)->size--;//assign even values for free blocks
    if((void*)BINSTART[BIN(((aheader_t*)ptr)->size)]<MINMEMLOC){//Check if Bin is empty
        ((fheader_t*)(ptr))->next=NULL;
    }
    else{
        ((fheader_t*)(ptr))->next=BINSTART[BIN(((aheader_t*)ptr)->size)];//assign old next in bin, regardless of null
    }
    BINSTART[BIN(((aheader_t*)ptr)->size)]=(fheader_t*)ptr;//assign the freed blocks bin ptr to point to it
    if(REALLOCING){
    coalesce(ptr);
    }
    return;
}

void coalesce(void *ptr)
{

    void *original=ptr;
    ptr=((char*)ptr+(((fheader_t*)(ptr))->size-1));
    if(ptr<HEAPEND&&!(((fheader_t*)(ptr))->size)%2){
        join(original,ptr);
    }
    return;
}
void join(void*original,void*next){
    if((!(((fheader_t*)(original))->size)%2)&&(!(((fheader_t*)(next))->size)%2)){
        printf("Joining");
        (((fheader_t*)(original))->size)=((((fheader_t*)(original))->size)+((fheader_t*)(next))->size);

    }
    return;
}

void activecollect(int bigblock){
    int i;
    for(i=(NUM8BINS-1);i<BIN(bigblock);i++){
        fheader_t *cursor=BINSTART[i];
        while((void*)cursor>MINMEMLOC){
            coalesce(cursor);
            cursor=cursor->next;
        }
    }
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}

void *findfit(size){
    int i;
    for(i=(BIN(size));i<NUMBINS-2;i++){
        fheader_t *fit=BINSTART[i];
        if((long)fit>(long)MINMEMLOC){
            if((void*)BINSTART[i]>MINMEMLOC){
                BINSTART[i]=BINSTART[i]->next;//is next block available to be added
            }
            else{
                BINSTART[i]=(fheader_t*)NULL;
            }
            split(fit,size);
            return (void*)fit;
        }
    }
    return NULL;
}
void split(void *p,int payload){
    int blocksize=((fheader_t*)p)->size;
    if((blocksize-payload)>MINBLOCKSIZE){
        ((fheader_t*)p)->size=(ALIGN(payload+TAGSIZE));
        p=((char*)p+(ALIGN(payload+TAGSIZE)));
        ((fheader_t*)p)->size=(ALIGN((blocksize-payload)+TAGSIZE));
        p=((char*)p-(ALIGN(payload+TAGSIZE)));
        return;
    }
    else{
        return;
    }
}
void *breakupfit(int blocksize){
    int i,j;
    fheader_t *bigblock;
    for(i=BIN(blocksize)+1;i<NUMBINS-1;i++){
        if((long)BINSTART[i]>(long)MINMEMLOC){
            bigblock=BINSTART[i];
            BINSTART[i]=bigblock->next;
            int bigblockstartsize=bigblock->size;
            for(j=0;j<((bigblockstartsize)-blocksize);j+=blocksize){
                printf("break %d of %d \n",bigblockstartsize, blocksize );
                bigblock->size=blocksize;
                bigblock->next=BINSTART[BIN(blocksize)];
                BINSTART[BIN(blocksize)]=bigblock;
            }

        return (void*)bigblock;
        }
    }

    return NULL;
}
void *bigfit(size_t size){
    fheader_t *prev=BINSTART[BIN((int)size)];
    if((void*)prev>MINMEMLOC&&prev->size>size){
        BINSTART[BIN(size)]=BINSTART[BIN(size)]->next;
        return prev;
    }
    fheader_t *binp=prev;
    printf("Big ol' bin:%d/%d+%d heapsize:%li\n", BIN((int)size),NUMBINS,(int)size, HEAPSIZE);
    while((void*)binp>MINMEMLOC){
        if(binp->size>size){
            return prev;
        }
        else{
            prev=binp;
            binp=binp->next;
        }
    }
    return (void*)-1;
}

void macroTest(){
    printf("ALIGNMENT:%d bin:%d,align:%d\n",ALIGNMENT,BIN(ALIGN(ALIGNMENT)),ALIGN(ALIGNMENT));
    printf("2*ALIGNMENT:%d bin:%d,align:%d\n",2*ALIGNMENT,BIN(ALIGN(2*ALIGNMENT)),ALIGN(2*ALIGNMENT));
    printf("2*ALIGNMENT-1:%d bin%d,align:%d\n",2*ALIGNMENT-1,BIN(ALIGN(2*ALIGNMENT-1)),ALIGN(2*ALIGNMENT-1));
    printf("BIGBLOCKSIZE-2*ALIGNMENT:%d bin%d,align%d\n",BIGBLOCKSIZE-2*ALIGNMENT,BIN(ALIGN(BIGBLOCKSIZE-2*ALIGNMENT)),ALIGN(BIGBLOCKSIZE-2*ALIGNMENT));
    printf("BIGBLOCKSIZE-ALIGNMENT:%d bin%d,align%d\n",BIGBLOCKSIZE-ALIGNMENT,BIN(ALIGN(BIGBLOCKSIZE-ALIGNMENT)),ALIGN(BIGBLOCKSIZE-ALIGNMENT));
    printf("BIGBLOCKSIZE-(ALIGNMENT-1):%d bin%d,align%d\n",BIGBLOCKSIZE-(ALIGNMENT-1),BIN(ALIGN(BIGBLOCKSIZE-(ALIGNMENT-1))),ALIGN(BIGBLOCKSIZE-(ALIGNMENT-1)));
    printf("BIGBLOCKSIZE:%d bin%d,align%d\n",BIGBLOCKSIZE,BIN(ALIGN(BIGBLOCKSIZE)),ALIGN(BIGBLOCKSIZE));
    printf("BIGBLOCKSIZE+ALIGNMENT:%d bin%d,align%d\n",BIGBLOCKSIZE+ALIGNMENT,BIN(ALIGN(BIGBLOCKSIZE+ALIGNMENT)),ALIGN(BIGBLOCKSIZE+ALIGNMENT));
    printf("2*BIGBLOCKSIZE-ALIGNMENT:%d bin%d,align%d\n",2*BIGBLOCKSIZE-ALIGNMENT,BIN(ALIGN(2*(BIGBLOCKSIZE-ALIGNMENT))),ALIGN(2*(BIGBLOCKSIZE-ALIGNMENT)));
    printf("10*BIGBLOCKSIZE:%d bin%d,align%d\n",10*BIGBLOCKSIZE,BIN(ALIGN(10*BIGBLOCKSIZE)),ALIGN(10*BIGBLOCKSIZE));
    printf("100*BIGBLOCKSIZE+1:%d bin%d,align%d\n",100*BIGBLOCKSIZE+1,BIN(ALIGN(100*BIGBLOCKSIZE+1)),ALIGN(100*BIGBLOCKSIZE+1));
    printf("(NUMBIGBINS-3)*BIGBLOCKSIZE:%d bin%d,align%d\n",(NUMBIGBINS-3)*BIGBLOCKSIZE,BIN(ALIGN((NUMBIGBINS-3)*BIGBLOCKSIZE)),ALIGN((NUMBIGBINS-3)*BIGBLOCKSIZE));
    printf("(NUMBIGBINS-2)*BIGBLOCKSIZE:%d bin%d,align%d\n",(NUMBIGBINS-2)*BIGBLOCKSIZE,BIN(ALIGN((NUMBIGBINS-2)*BIGBLOCKSIZE)),ALIGN((NUMBIGBINS-2)*BIGBLOCKSIZE));
    printf("(NUMBIGBINS-1)*BIGBLOCKSIZE:%d bin%d,align%d\n",(NUMBIGBINS-1)*BIGBLOCKSIZE,BIN(ALIGN((NUMBIGBINS-1)*BIGBLOCKSIZE)),ALIGN((NUMBIGBINS-1)*BIGBLOCKSIZE));
    printf("NUMBIGBINS*BIGBLOCKSIZE:%d bin%d,align%d\n",NUMBIGBINS*BIGBLOCKSIZE,BIN(ALIGN(NUMBIGBINS*BIGBLOCKSIZE)),ALIGN(128*BIGBLOCKSIZE+1));
    printf("BIGBLOCKSIZE*BIGBLOCKSIZE+1:%d bin%d,align%d\n",BIGBLOCKSIZE*BIGBLOCKSIZE+1,BIN(ALIGN(BIGBLOCKSIZE*BIGBLOCKSIZE+1)),ALIGN(BIGBLOCKSIZE*BIGBLOCKSIZE+1));

}
