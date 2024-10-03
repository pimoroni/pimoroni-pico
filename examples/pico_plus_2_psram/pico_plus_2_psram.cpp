#include "stdio.h"
#include <cstdlib>
#include "pico/stdlib.h"
#include "boards/pimoroni_pico_plus2_rp2350.h"

#include "hardware/structs/ioqspi.h"
#include "hardware/structs/qmi.h"
#include "hardware/structs/xip_ctrl.h"
#include "hardware/clocks.h"
#include "hardware/sync.h"
#include "rp2_psram.h"
#include "TestClass.h"
#include "lwmem/lwmem.hpp"

#include <vector>
#include "lwmemAllocator.h"

// 128 kb

// #define TEST_SPEED_SIZE         (16384)
// #define TEST_SPEED_ITTERATIONS  128
// #define TEST_SPEED_SIZE         (16384/8)
// #define TEST_SPEED_ITTERATIONS  (128*8)

#define TEST_MEM_ITTERATIONS    1

#pragma GCC push_options
#pragma GCC optimize ("-O2")


class ElapsedUs
{
public:
	ElapsedUs()
	{
		last_time = time_us_64();
	}

	float elapsed(void)
	{
		uint64_t time_now = time_us_64();
		uint64_t elapsed = time_now - last_time;
		last_time = time_now;
		return (float)elapsed/1000.0f;
	}

private:
	uint64_t last_time;
};

float CalcMbPerSec(uint32_t uSize, float fMsecs)
{
    float fBytesMS = (float)uSize/fMsecs;
    float fBytesSec = fBytesMS * 1000;
    float fSpeedMBS = fBytesSec/1024/1024;

    return fSpeedMBS;
}


absolute_time_t abs_start_time;
uint64_t microsecs;
double secs;
float fMySecs;
int    *xi;

void start_time()
{
    abs_start_time = get_absolute_time();
}

void end_time()
{
    microsecs = absolute_time_diff_us(abs_start_time, get_absolute_time());
    secs = microsecs * 1e-6;
}  

int testI(int jmax, int arraymax)    
{
    int i, j;
    int toti;
    volatile int* xvi = xi;
    
    ElapsedUs timer;
    start_time();
    
    for (j=0; j<jmax; j++)
    {
        toti = 0;
        for (i=0; i<arraymax; i=i+16)
        {
            (void)xvi[i+0];
            (void)xvi[i+1];
            (void)xvi[i+2];
            (void)xvi[i+3];
            (void)xvi[i+4];
            (void)xvi[i+5];
            (void)xvi[i+6];
            (void)xvi[i+7];
            (void)xvi[i+8];
            (void)xvi[i+9];
            (void)xvi[i+10];
            (void)xvi[i+11];
            (void)xvi[i+12];
            (void)xvi[i+13];
            (void)xvi[i+14];
            (void)xvi[i+15];            
        }
    }
    end_time();
    fMySecs = timer.elapsed()/1000.0f;
        
    return toti;
}

void TestMem(size_t uMemSize)
{
    uint32_t uTotalSize = uMemSize * TEST_MEM_ITTERATIONS;
    printf("Total size for test = %lu bytes\n", uTotalSize);

    printf("  Writing..\n");
    ElapsedUs timer;
    for(uint32_t i = 0; i < TEST_MEM_ITTERATIONS; i++)
    {
        uint32_t *p = (uint32_t *)PSRAM_LOCATION;
        for(uint32_t u = 0; u < uMemSize/sizeof(uint32_t); u++)
            *p++ = u;
    }
    float fWrite= timer.elapsed();
    printf("    Took %fms @%fMB/Sec\n", fWrite, CalcMbPerSec(uTotalSize, fWrite));

    printf("  Reading..\n");
    bool bReadOk = true;

    for(uint32_t i = 0; i < TEST_MEM_ITTERATIONS; i++)
    {
        uint32_t *p = (uint32_t *)PSRAM_LOCATION;
        for(uint32_t u = 0; bReadOk && (u < uMemSize/sizeof(uint32_t)); u++)
            bReadOk = *p++ == u;
    }

    float fRead = timer.elapsed();
    printf("    Took %fms @%fMB/Sec\n", fRead, CalcMbPerSec(uTotalSize, fRead));

    if(bReadOk)
        printf("memory test passed\n");
    else
        printf("memory test failed\n");

}

void InitHeap(size_t uHeapSize)
{
    static lwmem_region_t regions[] = 
    {
        {(void *)PSRAM_LOCATION, uHeapSize}, 
        {nullptr, 0}
    };

    lwmem_assignmem(regions);
}

void TestHeap(void)
{
    constexpr size_t allocSize = 1024;
    uint32_t *p = (uint32_t *)lwmem_malloc(allocSize * sizeof(uint32_t));
    if(p)
    {
        for(size_t u = 0; u < allocSize; u++)
            p[u] = u;

        bool bReadOk = true;
        for(size_t u = 0; bReadOk && (u < allocSize); u++)
            bReadOk = p[u] == u;

        if(bReadOk)
            printf("lwmem_malloc test passed\n");
        else
            printf("lwmem_malloc test failed\n");

        lwmem_free(p);
    }
    else
        printf("Error: lwmem_malloc failed\n");

}

void TestCpp(void)
{
    TestClass *tc = new TestClass(1);

    std::vector<int, LwmemAllocator<int>> v;
    v.push_back(1);
    v.push_back(2);
    

    if(tc && v[0] == 1 && v[1] == 2)
    {
        printf("C++ test passed\n");
        delete tc;
    }
    else
        printf("C++ test failed\n");

}

void TestMem(volatile uint32_t *pMem, uint32_t uSize, uint32_t uIterations, const char *str)
{
    uint32_t uTotalSize = uSize * uIterations;
    printf("%s Block Size = %lu bytes, Iterations = %lu,  Total size for = %lu bytes, using addr %p\n", str, uSize*sizeof(uint32_t), uIterations, uTotalSize, pMem);
    ElapsedUs timer;

    float fWriteTime = 0.0f;
    float fReadTime = 0.0f;

    for(uint32_t i = 0; i < uIterations; i++)
    {
        volatile uint32_t *p = pMem;

        for(uint32_t u = 0; u < uSize/32; u++)
        {
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
            *p++ = u;
        }

        fWriteTime += timer.elapsed();

        for(uint32_t u = 0; u < uSize/32; u++)
        {
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
            (void)*p++;
        }
        fReadTime += timer.elapsed();
    }
    printf(" Read: %fMB/Sec, Write %fMB/Sec\n\n", CalcMbPerSec(uTotalSize*4, fReadTime), CalcMbPerSec(uTotalSize*4, fWriteTime));
    // printf("Write took %fms %fMB/Sec\n", fWriteTime, CalcMbPerSec(uTotalSize, fWriteTime));
    // printf("Read  took %fms %fMB/Sec (%u)\n", fReadTime, CalcMbPerSec(uTotalSize, fReadTime), bReadOk);
}

void TestSpeed(uint32_t uSize, uint32_t uIterations)
{
    volatile uint32_t *pNormalRam = (uint32_t *)malloc(uSize * sizeof(uint32_t));
    if(pNormalRam) 
    {
        TestMem(pNormalRam, uSize, uIterations, "RAM:");
        free((uint32_t *)pNormalRam);
    }

    volatile uint32_t *pPsram = (uint32_t *)lwmem_malloc(uSize * sizeof(uint32_t));
    if(pPsram)
    {
        TestMem(pPsram, uSize, uIterations, "PSRAM:");
        lwmem_free((uint32_t *)pPsram);
    }
}

void TestXI(void)
{
    uint32_t uXiTestSize = (1536);
    xi = (int *)lwmem_malloc(uXiTestSize * sizeof(int));
    testI(1000, uXiTestSize);
    uint32_t uTotalMem = uXiTestSize * 4 * 1000;
    float fBytesSecs = (float)uTotalMem/secs;
    float fSpeedMBS = fBytesSecs/1024/1024;
    printf("XI Test 6k = %f, %f, %f\n", secs, fMySecs, fSpeedMBS);
}

int main() {
	size_t uRamSize = psram_init(PIMORONI_PICO_PLUS2_PSRAM_CS_PIN);

	stdio_init_all();

        
	printf("Pico Plus 2 PSRAM tests\n\n");


	printf("Psram installed = %u bytes, %u KB, %u MB\n", uRamSize, uRamSize/1024, uRamSize/1024/1024);

	if(uRamSize)
	{
        TestMem(uRamSize);

        InitHeap(uRamSize);

        TestCpp();

        TestXI();

        TestSpeed(1024,1024);
        TestSpeed(1024*2,1024/2);
        TestSpeed(1024*4,1024/4);
        TestSpeed(1024*8,1024/8);
        TestSpeed(1024*16,1024/16);
        TestSpeed(1024*32,1024/32);
	}
    else
        printf("No ram found, tests not run\n");

  return 0;
}

#pragma GCC pop_options