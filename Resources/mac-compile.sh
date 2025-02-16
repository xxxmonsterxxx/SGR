#!/bin/bash

echo "-------------------------------------------"
echo "Welcome to shader auto script compilator! |"
echo "-------------------------------------------"

FOLDER_CREATED=false
rm -rf CompiledShaders

for entry in `find . -type f -name "*.frag" & find . -type f -name "*.vert"`; do
	SHADER_NAME=$(basename "$entry")
	echo "Found $SHADER_NAME"
	if [ $FOLDER_CREATED == false ]
	then
		mkdir CompiledShaders
		FOLDER_CREATED=true
	fi
	~/Libs/VulkanSDK/latest/macOS/bin/glslc $entry -o CompiledShaders/$SHADER_NAME.spv
	echo "$entry -> CompiledShaders/$entry.spv"
done

