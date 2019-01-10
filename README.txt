1.
Install msys like discribed in docs/msys setup.rtf

2.
Copy content of folder "tools/components_libs" into the components folder in your IDF_PATH
 
3.
In Eclipse:
	Click File -> Import
		under C/C++ choose "Existing Code as Makefile Project" and click next
	Browse to Folder Location
	Uncheck the "Show only available toolchains that support this platform" and choose "Cygwin GCC"
		Click Finish
	
Project Settings:
	Eclipse -> Project Properties -> C/C++ Build:
		Behavios: 		Enable parallel build
		Builder Settings:	python ${IDF_PATH}/tools/windows/eclipse_make.py -j8

	Eclipse -> Project Properties -> C/C++ Build -> Environment:

		Add:
			Variable: BATCH_BUILD	Value: 1
		Edit:
			Variable: PATH		Value: C:\msys32\usr\bin;C:\msys32\mingw32\bin;C:\msys32\opt\xtensa-esp32-elf\bin (Delete existing Path)

Eclipse -> Project Properties -> C/C++ General -> Preprocessor Include Paths, Macros etc.:

Check:
	CDT GCC Built-in Compiler Settings Cygwin
		Comand to get compiler specs:
			xtensa-esp32-elf-gcc ${FLAGS} -E -P -v -dD "${INPUTS}"
	CDT GCC Built-in Output Parser
		Compiler command pattern:
			xtensa-esp32-elf-(g?cc)|([gc]\+\+)|(clang)

4. 
Now you can Build the Project
	Project -> Build Project

5.
To Flash ESP32:
	Plugin the ESP32 and check the COM-Port in the Device Manager of Windows
	If there is no COM-Port:
		Install ESP32 Driver (located in tools folder)
	Check COM-Port and change it to COM15 or eddit the sdkconfig file in this Project and enter your COM-Port (line 41)

	Now in Eclipse click:
		Project -> Build Targets -> Create...
	Enter Target name "flash"
	
		Project -> Build Targets -> build...
	Select "flash" and click Build


DONE!