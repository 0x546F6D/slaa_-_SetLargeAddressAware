# slaa - Set Large Address Aware


Set Large Address Aware flag of 32bit executables from command line:

  - Set LLA flag
  - UnSet LLA flag if already set

Compiled with MSYS2/MinGW-w64:

	$ gcc -o slaa slaa.c -s -O3 -Wl,--gc-sections -fno-asynchronous-unwind-tables -nostartfiles --entry=slaa


Usage:

	> slaa Path\to\32bit\executable


Notes:

  - ..

To view the source code in your browser with original tabbing (4 white-spaces instead of browsers default 8), add '/?ts=4' to url.  
Works on Firefox/Chrome based browser, except for edge..  
https://github.com/0x546F6D/slaa_-_Set_Large_Address_Aware/blob/main/slaa.c/?ts=4
