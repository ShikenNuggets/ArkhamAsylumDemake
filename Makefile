SRC_DIR = Source
BUILD_DIR = Build
OBJ_DIR = $(BUILD_DIR)/Intermediate

PS2SDK=/usr/local/ps2dev/ps2sdk

EE_BIN=$(BUILD_DIR)/test.elf
EE_OBJS=$(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.obj, $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/**/*.cpp))

EE_INCS += -I$(SRC_DIR) 
EE_LIBS += -lstdc++ -ldma -lgraph -ldraw -lkernel -ldebug -lpacket -lmath3d -lpad

EE_CXXFLAGS += -Wall --std=c++17
EE_LDFLAGS = -L$(PSDSDK)/ee/common/lib -L$(PS2SDK)/ee/lib

ISO_TGT=$(BUILD_DIR)/test.iso

include $(PS2SDK)/samples/Makefile.eeglobal
include $(PS2SDK)/samples/Makefile.pref

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/Main.obj: $(SRC_DIR)/Main.cpp $(SRC_DIR) | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(EE_CXX) $(EE_CXXFLAGS) $(EE_INCS) -c $< -o $@

$(OBJ_DIR)/%.obj: $(SRC_DIR)/%.cpp $(SRC_DIR)/%.hpp $(SRC_DIR) | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(EE_CXX) $(EE_CXXFLAGS) $(EE_INCS) -c $< -o $@

all: $(ISO_TGT)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: iso
iso:
	mkisofs -l -o $(ISO_TGT) $(EE_BIN) SYSTEM.CNF
	cp $(ISO_TGT) /mnt/e/Games/PS2/DVD/BatmanArkhamAsylum.iso