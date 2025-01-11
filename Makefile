
EE_BIN=Build/test.elf
EE_OBJS=Build/Intermediate/main.o

EE_LIBS= -ldma -lgraph -ldraw -lkernel -ldebug

EE_CFLAGS += -Wall --std=c99
EE_LDFLAGS = -L$(PSDSDK)/ee/common/lib -L$(PS2SDK)/ee/lib


PS2SDK=/usr/local/ps2dev/ps2sdk

ISO_TGT=Build/test.iso

include $(PS2SDK)/samples/Makefile.eeglobal
include $(PS2SDK)/samples/Makefile.pref

all: $(ISO_TGT)

$(ISO_TGT): $(EE_BIN)
	mkisofs -l -o $(ISO_TGT) $(EE_BIN) SYSTEM.CNF

.PHONY: docker-build
docker-build:
	docker run -v $(shell pwd):/src ps2build make

.PHONY: clean
clean:
	rm -rf $(ISO_TGT) $(EE_BIN) $(EE_OBJS)