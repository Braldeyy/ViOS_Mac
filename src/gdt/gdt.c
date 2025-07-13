#include "gdt.h"
#include "panic/panic.h"

void encodeGdtEntry(uint8_t *target, struct gdt_structured source)
{
    // Check if this is a TSS descriptor (type 0x89 = available 32-bit TSS)
    if (source.type == 0x89) {
        // TSS descriptors must use byte granularity, not page granularity
        target[6] = 0x40; // 32-bit segment, byte granularity
        
        // Encodes the limit (byte granularity for TSS)
        target[0] = source.limit & 0xFF;
        target[1] = (source.limit >> 8) & 0xFF;
        target[6] |= (source.limit >> 16) & 0x0F;
    } else {
        // Standard segment descriptor handling
        if ((source.limit > 65536) && ((source.limit & 0xFFF) != 0xFFF))
        {
            panic("encodeGdtEntry: Invalid argument\n");
        }

        target[6] = 0x40;
        if (source.limit > 65536)
        {
            source.limit = source.limit >> 12;
            target[6] = 0xC0;
        }

        // Encodes the limit
        target[0] = source.limit & 0xFF;
        target[1] = (source.limit >> 8) & 0xFF;
        target[6] |= (source.limit >> 16) & 0x0F;
    }

    // Encode the base (same for both TSS and regular segments)
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;

    // Set the type
    target[5] = source.type;
}

void gdt_structured_to_gdt(struct gdt *gdt, struct gdt_structured *structured_gdt, int total_entires)
{
    for (int i = 0; i < total_entires; i++)
    {
        encodeGdtEntry((uint8_t *)&gdt[i], structured_gdt[i]);
    }
}
