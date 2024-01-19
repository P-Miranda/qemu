#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/log.h"
#include "hw/sysbus.h"
#include "chardev/char.h"
#include "hw/hw.h"
#include "hw/irq.h"

#define TYPE_VIRT_FOO "virt-foo"

typedef struct virt_fooState virt_fooState;
DECLARE_INSTANCE_CHECKER(virt_fooState, VIRT_FOO, TYPE_VIRT_FOO)

struct virt_fooState
{
    SysBusDevice parent_obj;
    MemoryRegion mmio;
    unsigned char virt_fooReg[6]; //{'B','U','T','T','E','R'};
    qemu_irq irq; // needed for SysBusDevice initialization, unused for now
};

static uint64_t virt_foo_read(void *opaque, hwaddr addr, unsigned int size)
{
    virt_fooState *s = opaque;
    qemu_log_mask(LOG_GUEST_ERROR, "%s: read: addr=0x%x size=%d\n",
                  __func__, (int)addr,size);
    return s->virt_fooReg[addr];
}

static void virt_foo_write(void *opaque, hwaddr addr, uint64_t val64, unsigned int size)
{
    /*dummy code for future development*/
    virt_fooState *s = opaque;
    uint32_t value = val64;
    unsigned char ch = value;
    (void)s;
    (void)ch;
    qemu_log_mask(LOG_GUEST_ERROR, "%s: write: addr=0x%x v=0x%x\n",
                  __func__, (int)addr, (int)value);
}

static const MemoryRegionOps virt_foo_ops = {
    .read = virt_foo_read,
    .write = virt_foo_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
        .min_access_size = 1,
        .max_access_size = 6}
};

static void virt_foo_realize(DeviceState *d, Error **errp)
{

    virt_fooState *s = VIRT_FOO(d);
    SysBusDevice *sbd = SYS_BUS_DEVICE(d);

    memory_region_init_io(&s->mmio, OBJECT(s), &virt_foo_ops, s,
                          TYPE_VIRT_FOO, 0x100);
    sysbus_init_mmio(sbd, &s->mmio);
    sysbus_init_irq(sbd, &s->irq);

    s->virt_fooReg[0]='B';
    s->virt_fooReg[1]='U';
    s->virt_fooReg[2]='T';
    s->virt_fooReg[3]='T';
    s->virt_fooReg[4]='E';
    s->virt_fooReg[5]='R';
}

static void virt_foo_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = virt_foo_realize;
}

static const TypeInfo virt_foo_info = {
    .name          = TYPE_VIRT_FOO,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(virt_fooState),
    .class_init    = virt_foo_class_init,
};

static void virt_foo_register_types(void)
{
    type_register_static(&virt_foo_info);
}

type_init(virt_foo_register_types)
