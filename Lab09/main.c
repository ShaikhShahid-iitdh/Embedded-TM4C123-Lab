#include <stdint.h>
uint32_t fadd(uint32_t a, uint32_t b);
uint32_t fadd(uint32_t a, uint32_t b)
{
    unsigned int e1, f1, m1;
    unsigned int e2, f2, m2;
    unsigned int e, f, m;
    uint32_t c;

    e1 = (a >> 23) & 0xFF;
    e2 = (b >> 23) & 0xFF;

    f1 = a & 0x7FFFFF;
    f2 = b & 0x7FFFFF;

    m1 = f1 | 0x800000;
    m2 = f2 | 0x800000;

    if (e1 > e2)
    {
        m2 >>= (e1 - e2);
        e = e1;
    }
    else if (e2 > e1)
    {
        m1 >>= (e2 - e1);
        e = e2;
    }
    else
    {
        e = e1;
    }

    m = m1 + m2;

    if (m & 0x1000000)
    {
        m >>= 1;
        e++;
    }

    f = m & 0x7FFFFF;
    c = (e << 23) | f;

    return c;
}

struct
{
    uint32_t u;     // 32-bit IEEE format computed by fadd
    float f;        // reference result computed by C addition
} buf[0x100];
int index = 0;

void fadd_test(float a, float b)
{
    union
    {
        uint32_t u; // access as 32-bit raw bits
        float f;    // access as float
    } A, B, C;

    float c;

    A.f = a;
    B.f = b;

    C.u = fadd(A.u, B.u); // result from fadd()
    c = a + b;            // reference result from C

    if (index < 0x100)
    {
        buf[index].u = C.u;
        buf[index].f = c;
        index++;
    }

    return;
}
int main(void)
{
    fadd_test(100.0f, 0.25f);       // Expect 100.25f  (0x42C88000)
    fadd_test(1.5f, 1.5f);          // Expect 3.0f     (0x40400000)
    fadd_test(2.75f, 1.5f);         // Expect 4.25f    (0x40880000)
    fadd_test(1024.0f, 1.0f);       // Expect 1025.0f  (0x44802000)
    fadd_test(1.0f, 0.125f);        // Expect 1.125f   (0x3F900000)
    fadd_test(65536.0f, 256.0f);    // Expect 65792.0f (0x47808000)

    while (1);
    return 0;
}
