################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-1822914657: ../i2ctmp.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/sysconfig_1_8_2/sysconfig_cli.bat" -s "C:/ti/simplelink_cc13x2_26x2_sdk_5_20_00_52/.metadata/product.json" --script "C:/Users/alberto/workspace_v11/MPU-6050_LP_CC2652RB_tirtos_ccs/i2ctmp.syscfg" -o "syscfg" --compiler ccs
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_devices_config.c: build-1822914657 ../i2ctmp.syscfg
syscfg/ti_drivers_config.c: build-1822914657
syscfg/ti_drivers_config.h: build-1822914657
syscfg/ti_utils_build_linker.cmd.genlibs: build-1822914657
syscfg/syscfg_c.rov.xs: build-1822914657
syscfg/ti_utils_runtime_model.gv: build-1822914657
syscfg/ti_utils_runtime_Makefile: build-1822914657
syscfg/: build-1822914657

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.6.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/alberto/workspace_v11/MPU-6050_LP_CC2652RB_tirtos_ccs" --include_path="C:/Users/alberto/workspace_v11/MPU-6050_LP_CC2652RB_tirtos_ccs/Debug" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_5_20_00_52/source" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_5_20_00_52/source/ti/posix/ccs" --include_path="C:/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.6.LTS/include" --define=DeviceFamily_CC26X2 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="C:/Users/alberto/workspace_v11/MPU-6050_LP_CC2652RB_tirtos_ccs/Debug/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.6.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/alberto/workspace_v11/MPU-6050_LP_CC2652RB_tirtos_ccs" --include_path="C:/Users/alberto/workspace_v11/MPU-6050_LP_CC2652RB_tirtos_ccs/Debug" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_5_20_00_52/source" --include_path="C:/ti/simplelink_cc13x2_26x2_sdk_5_20_00_52/source/ti/posix/ccs" --include_path="C:/ti/ccs1120/ccs/tools/compiler/ti-cgt-arm_20.2.6.LTS/include" --define=DeviceFamily_CC26X2 -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/alberto/workspace_v11/MPU-6050_LP_CC2652RB_tirtos_ccs/Debug/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


