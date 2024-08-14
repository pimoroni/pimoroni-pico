#include "stdio.h"
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

void TestMem(size_t uMemSize)
{
    uint32_t *p = (uint32_t *)PSRAM_LOCATION;

    printf("  Writing..\n");
    for(uint32_t u = 0; u < uMemSize/sizeof(uint32_t); u++)
        *p++ = u;

    printf("  Reading..\n");
    bool bReadOk = true;
    p = (uint32_t *)PSRAM_LOCATION;
    for(uint32_t u = 0; bReadOk && (u < uMemSize/sizeof(uint32_t)); u++)
        bReadOk = *p++ == u;


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

    if(tc)
    {
        printf("C++ test passed");
        delete tc;
    }
}

int main() {
	stdio_init_all();

    
	printf("Pico Plus 2 PSRAM tests\n\n");

	size_t uRamSize = psram_init(PIMORONI_PICO_PLUS2_PSRAM_CS_PIN);

	printf("Psram installed = %u bytes, %u KB, %u MB\n", uRamSize, uRamSize/1024, uRamSize/1024/1024);

	if(uRamSize)
	{
        TestMem(uRamSize);

        TestHeap(uRamSize);

        TestCpp();
	}
    else
        printf("No ram found, tests not run\n");

  return 0;
}
