#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/log.h"
#include "hw/sysbus.h"
#include "chardev/char.h"
#include "hw/hw.h"
#include "hw/irq.h"

#define TYPE_TEST_COUNTER "test-counter"

typedef struct TestCounterState TestCounterState;
DECLARE_INSTANCE_CHECKER(TestCounterState, TEST_COUNTER, TYPE_TEST_COUNTER)

/* Register map */
#define REG_ID      0x00    // R: ID/Version
#define REG_RST     0x04    // W: Reset counter
#define REG_INCR    0x08    // W: Increment counter
#define REG_SAMPLE  0x0C    // W: Sample counter
#define REG_DATA    0x10    // R: Read sampled data
#define REG_SET     0x14    // W: Set counter to value

#define CHIP_ID 0x1234

struct TestCounterState
{
    SysBusDevice parent_obj;
    MemoryRegion mmio;
    qemu_irq irq; // needed for SysBusDevice initialization, unused for now
    uint32_t id;
    uint32_t counter;
    uint32_t data;
};

static uint64_t test_counter_read(void *opaque, hwaddr addr, unsigned int size)
{
    TestCounterState *s = opaque;
    qemu_log_mask(LOG_GUEST_ERROR, "%s: read: addr=0x%x size=%d\n",
                  __func__, (int)addr,size);
    switch(addr){
        case REG_ID:
            return s->id;
        case REG_DATA:
            return s->data;
        default:
            break;
    }
    return 0;
}

static void test_counter_write(void *opaque, hwaddr addr, uint64_t val64, unsigned int size)
{
    /*dummy code for future development*/
    TestCounterState *s = (TestCounterState *) opaque;

    switch(addr){
        case REG_RST:
            s->counter = 0;
            s->data = 0;
            break;
        case REG_INCR:
            s->counter++;
            break;
        case REG_SAMPLE:
            s->data = s->counter;
            break;
        case REG_SET:
            s->counter = (uint32_t) val64;
            break;
        default:
            break;
    }
    qemu_log_mask(LOG_GUEST_ERROR, "%s: write: addr=0x%x v=0x%x\n",
                  __func__, (int)addr, (int)val64);
}

static const MemoryRegionOps test_counter_ops = {
    .read = test_counter_read,
    .write = test_counter_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 1,
        .max_access_size = 6}
};

static void test_counter_realize(DeviceState *d, Error **errp)
{

    TestCounterState *s = TEST_COUNTER(d);
    SysBusDevice *sbd = SYS_BUS_DEVICE(d);

    memory_region_init_io(&s->mmio, OBJECT(s), &test_counter_ops, s,
                          TYPE_TEST_COUNTER, 0x100);
    sysbus_init_mmio(sbd, &s->mmio);
    sysbus_init_irq(sbd, &s->irq);

    s->id = CHIP_ID;
    s->counter = 0;
    s->data = 0;
}

static void test_counter_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = test_counter_realize;
}

static const TypeInfo test_counter_info = {
    .name          = TYPE_TEST_COUNTER,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(TestCounterState),
    .class_init    = test_counter_class_init,
};

static void test_counter_register_types(void)
{
    type_register_static(&test_counter_info);
}

type_init(test_counter_register_types)
