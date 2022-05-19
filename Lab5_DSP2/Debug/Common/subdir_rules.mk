################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Common/%.obj: ../Common/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"/home/sahil/ti/ccs1020/ccs/tools/compiler/ti-cgt-c2000_20.2.4.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="/home/sahil/Documents/workspace/Lab5_DSP2" --include_path="/home/sahil/ti/c2000/C2000Ware_3_04_00_00/device_support/f2837xd/headers/include" --include_path="/home/sahil/ti/c2000/C2000Ware_3_04_00_00/device_support/f2837xd/common/include" --include_path="/home/sahil/ti/c2000/C2000Ware_3_04_00_00/driverlib/f2837xd/driverlib" --include_path="/home/sahil/ti/ccs1020/ccs/tools/compiler/ti-cgt-c2000_20.2.4.LTS/include" --advice:performance=all --define=CPU1 --define=_LAUNCHXL_F28379D -g --c11 --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="Common/$(basename $(<F)).d_raw" --obj_directory="Common" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

Common/%.obj: ../Common/%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"/home/sahil/ti/ccs1020/ccs/tools/compiler/ti-cgt-c2000_20.2.4.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 --include_path="/home/sahil/Documents/workspace/Lab5_DSP2" --include_path="/home/sahil/ti/c2000/C2000Ware_3_04_00_00/device_support/f2837xd/headers/include" --include_path="/home/sahil/ti/c2000/C2000Ware_3_04_00_00/device_support/f2837xd/common/include" --include_path="/home/sahil/ti/c2000/C2000Ware_3_04_00_00/driverlib/f2837xd/driverlib" --include_path="/home/sahil/ti/ccs1020/ccs/tools/compiler/ti-cgt-c2000_20.2.4.LTS/include" --advice:performance=all --define=CPU1 --define=_LAUNCHXL_F28379D -g --c11 --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="Common/$(basename $(<F)).d_raw" --obj_directory="Common" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


