TARGET = littleVGL
TOP = $(shell chdir)
BUILD = $(TOP)\build
OBJS_DIR = $(BUILD)\Objects
LISTINGS = $(BUILD)\Listings
C_SRC += $(shell where /R .\startup *.c)  
C_SRC += $(shell where /R .\std_periph_driver *.c)  
C_SRC += $(shell where /R .\user *.c)  
C_SRC += $(shell where /R .\gui\lvgl\src *.c)  
C_SRC += $(shell where /R .\gui\lvgl_driver *.c)
C_SRC += $(shell where /R .\gui\lv_examples\lv_tests\lv_test_theme *.c)   
C_SRC += $(shell where /R .\gui\lv_examples\lv_apps\demo *.c) 
C_SRC += $(shell where /R .\gui_app *.c)  
S_SRC = $(shell where /R .\startup *.s)
C_OBJ=$(C_SRC:%.c=%.o)
S_OBJ=$(S_SRC:%.s=%.o)
ARMCC = armcc
ARMASM = armasm
ARMAR = armar 
ARMLINK = armlink 
FROMELF = fromelf 
CPU = --cpu=Cortex-M4.fp.sp
CFLAGS = --c99 --gnu -c $(CPU) -D__MICROLIB -g -O0 --apcs=interwork --split_sections
ASMFLAGS = $(CPU) -g --apcs=interwork --pd "__MICROLIB SETA 1" --pd "__UVISION_VERSION SETA 529" --pd "STM32F407xx SETA 1"
LINKFLAGS = --library_type=microlib --strict --scatter=$(OBJS_DIR)\$(TARGET).sct
INFO = --info sizes --info totals --info unused --info veneers
MAP = --summary_stderr --info summarysizes --map --load_addr_map_info --xref --callgraph --symbols --list $(LISTINGS)\$(TARGET).map
INC_FLAGS = -I $(TOP)\user\inc \
			-I $(TOP)\std_periph_driver\inc \
			-I $(TOP)\startup \
			-I $(TOP)\gui_app \
			-I $(TOP)\gui \
			-I $(TOP)\gui\lv_examples \
			-I $(TOP)\gui\lvgl \
			-I $(TOP)\gui\lvgl_driver	\
			-I.\RTE\_littleVGL	\
			-I"D:\Keil MDK\Arm packs\Keil\STM32F4xx_DFP\2.11.0\Drivers\CMSIS\Device\ST\STM32F4xx\Include"	\
			-I"D:\Keil MDK\ARM\CMSIS\Include"

IGNORE = --diag_suppress=68  --diag_suppress=111 --diag_suppress=550
DEFINES = -D__UVISION_VERSION="529"	\
		  -DSTM32F407xx \
		  -DSTM32F40_41xxx \
		  -DUSE_STDPERIPH_DRIVER

.PHONY: all clean

all: $(BUILD) $(OBJS_DIR) $(LISTINGS) $(S_OBJ) $(C_OBJ)
	@echo Linking and creating $(TARGET).axf file......
	@$(ARMLINK) $(CPU) $(S_OBJ) $(C_OBJ) $(LINKFLAGS) $(INFO) $(MAP) -o $(BUILD)\$(TARGET).axf
	@echo Creating $(TARGET).hex file...
	@$(FROMELF) $(BUILD)\$(TARGET).axf --i32combined --output $(BUILD)\$(TARGET).hex
	@echo Creating $(TARGET).bin file......
	@$(FROMELF) --bin -o $(BUILD)\$(TARGET).bin $(BUILD)\$(TARGET).axf
	@echo Done.
	
$(BUILD):
	@mkdir $@
$(OBJS_DIR):
	@mkdir $@
$(LISTINGS):
	@mkdir $@
$(S_OBJ):%.o:%.s
	@echo Compiling $^...
	@$(ARMASM) $(ASMFLAGS) $(INC_FLAGS) --list $(LISTINGS)\$(basename $(notdir $@)).lst --xref -o $@ --depend=$(OBJS_DIR)\$(basename $(notdir $@)).d $^ 
$(C_OBJ):%.o:%.c
	@echo Compiling $^...
	@$(ARMCC) $(CFLAGS) $(INC_FLAGS) $(IGNORE) $(DEFINES) -o $@ --omf_browse=$(OBJS_DIR)\$(basename $(notdir $@)).crf --depend="$(OBJS_DIR)\$(basename $(notdir $@)).d" $^
	



clean:
	@echo Deleting *.o files...
	@del $(shell where /R .\ *.o)
	@echo Deleting *.d files...
	@del $(shell where /R .\ *.d)
	@echo Deleting *.crf files...
	@del $(shell where /R .\ *.crf)
	@echo Deleting *.lst files...
	@del $(shell where /R .\ *.lst)
	@echo Deleting *.map files...
	@del $(shell where /R .\ *.map)
	@echo Deleting *.axf files...
	@del $(shell where /R .\ *.axf)
	@echo Deleting *.hex files...
	@del $(shell where /R .\ *.hex)
	@echo Deleting *.bin files...
	@del $(shell where /R .\ *.bin)
	@echo Deleting *.htm files...
	@del $(shell where /R .\ *.htm)
	@echo Done.