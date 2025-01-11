EE_BIN=Build/test.elf
EE_OBJS=Build/Intermediate/main.o

EE_LIBS= -ldma -lgraph -ldraw -lkernel -ldebug

EE_CFLAGS += -Wall --std=c99
EE_LDFLAGS = -L$(PSDSDK)/ee/common/lib -L$(PS2SDK)/ee/lib


PS2SDK=/usr/local/ps2dev/ps2sdk

ISO_TGT=Build/test.iso

include $(PS2SDK)/samples/Makefile.eeglobal
include $(PS2SDK)/samples/Makefile.pref

$(EE_OBJS): main.c
	$(EE_CC) $(EE_CFLAGS) $(EE_INCS) -c $< -o $@

all: $(ISO_TGT)

.PHONY: clean
clean:
	rm -rf $(ISO_TGT) $(EE_BIN) $(EE_OBJS)

.PHONY: iso
iso:
	mkisofs -l -o $(ISO_TGT) $(EE_BIN) SYSTEM.CNF