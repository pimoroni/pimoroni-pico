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

#define TEST_SPEED_SIZE         32768
#define TEST_BYTE_SIZE          (TEST_SPEED_SIZE*sizeof(uint32_t))
#define TEST_SPEED_ITTERATIONS  64
#define TEST_MEM_ITTERATIONS    1

#pragma GCC push_options
#pragma GCC optimize ("-O3")

uint32_t randValues[1024];

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

void TestHeap(size_t uHeapSize)
{
    lwmem_region_t regions[] = 
    {
        {(void *)PSRAM_LOCATION, uHeapSize}, 
        {nullptr, 0}
    };

    lwmem_assignmem(regions);

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

void TestMem(volatile uint32_t *pMem, uint32_t uSize)
{
    uint32_t uTotalSize = uSize * TEST_SPEED_ITTERATIONS;
    printf("Total size for test = %lu bytes using addr %p\n", uTotalSize, pMem);
    ElapsedUs timer;
    for(uint32_t i = 0; i < TEST_SPEED_ITTERATIONS; i++)
    {
        for(uint32_t u = 0; u < uSize; u++)
            pMem[i] = u;
    }
    float fWrite = timer.elapsed();
    printf("Write took %fms %fMB/Sec\n", fWrite, CalcMbPerSec(uTotalSize, fWrite));
    timer.elapsed();
    bool bReadOk = true;
    for(uint32_t i = 0; i < TEST_SPEED_ITTERATIONS; i++)
    {
        for(uint32_t u = 0; u < uSize; u++)
            bReadOk = pMem[i] == u;
    }
    float fRead = timer.elapsed();
    printf("Read (%u) took %fms %fMB/Sec\n", bReadOk, fRead, CalcMbPerSec(uTotalSize, fRead));
}

void TestSpeed(void)
{
    printf("Testing normal ram\n");
    volatile uint32_t *pNormalRam = (uint32_t *)malloc(TEST_BYTE_SIZE);
    if(pNormalRam)
    {
        TestMem(pNormalRam, TEST_BYTE_SIZE);
        free((uint32_t *)pNormalRam);
    }

    printf("Testing psram\n");
    volatile uint32_t *pPsram = (uint32_t *)lwmem_malloc(TEST_BYTE_SIZE);
    if(pPsram)
    {
        TestMem(pPsram, TEST_BYTE_SIZE);
        lwmem_free((uint32_t *)pPsram);
    }
}

int main() {
	stdio_init_all();

    for(uint32_t u =0; u < 1024; u++)
        u = rand();
        
	printf("Pico Plus 2 PSRAM tests\n\n");

	size_t uRamSize = psram_init(PIMORONI_PICO_PLUS2_PSRAM_CS_PIN);

	printf("Psram installed = %u bytes, %u KB, %u MB\n", uRamSize, uRamSize/1024, uRamSize/1024/1024);

	if(uRamSize)
	{
        TestMem(uRamSize);

        TestHeap(uRamSize);

        TestCpp();

        TestSpeed();
	}
    else
        printf("No ram found, tests not run\n");

  return 0;
}

#pragma GCC pop_options