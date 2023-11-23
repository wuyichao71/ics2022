AM_SRCS := platform/nemu/trm.c \
           platform/nemu/ioe/ioe.c \
           platform/nemu/ioe/timer.c \
           platform/nemu/ioe/input.c \
           platform/nemu/ioe/gpu.c \
           platform/nemu/ioe/audio.c \
           platform/nemu/ioe/disk.c \
           platform/nemu/mpe.c

CFLAGS    += -fdata-sections -ffunction-sections
LDFLAGS   += -T $(AM_HOME)/scripts/linker.ld \
             --defsym=_pmem_start=0x80000000 --defsym=_entry_offset=0x0
LDFLAGS   += --gc-sections -e _start
# NEMUFLAGS += -l $(shell dirname $(IMAGE).elf)/nemu-log.txt
# wuyc
NEMUFLAGS += -l $(shell dirname $(IMAGE).elf)/nemu-log.txt -e $(IMAGE).elf
RAMDISK = $(wildcard $(shell dirname $(IMAGE).elf)/ramdisk.img)
# ifeq ($(RAMDISK),)
#   $(error $(RAMDISK))
#   NEMUFLAGS += -e $(RAMDISK)
# endif
$(info $(shell dirname $(IMAGE).elf))
$(info $(RAMDISK))
NEMU_BATCH_FLAGS += $(NEMUFLAGS) --batch
# wuyc

CFLAGS += -DMAINARGS=\"$(mainargs)\"
# $(info mainargs --> $(mainargs))
CFLAGS += -I$(AM_HOME)/am/src/platform/nemu/include
.PHONY: $(AM_HOME)/am/src/platform/nemu/trm.c

image: $(IMAGE).elf
	@$(OBJDUMP) -d $(IMAGE).elf > $(IMAGE).txt
	@echo + OBJCOPY "->" $(IMAGE_REL).bin
	@$(OBJCOPY) -S --set-section-flags .bss=alloc,contents -O binary $(IMAGE).elf $(IMAGE).bin

run: image
	$(MAKE) -C $(NEMU_HOME) ISA=$(ISA) run ARGS="$(NEMUFLAGS)" IMG=$(IMAGE).bin

# wuyc
batch: image
	$(MAKE) -C $(NEMU_HOME) ISA=$(ISA) run ARGS="$(NEMU_BATCH_FLAGS)" IMG=$(IMAGE).bin
# wuyc

gdb: image
	$(MAKE) -C $(NEMU_HOME) ISA=$(ISA) gdb ARGS="$(NEMUFLAGS)" IMG=$(IMAGE).bin
