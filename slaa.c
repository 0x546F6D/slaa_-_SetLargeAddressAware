// # slaa - Pin To TaskBar

// Pin To TaskBar for command line:
//	- Set LLA flag
//	- UnSet LLA flag if already set

// Compiled with MSYS2/MinGW-w64:
//	gcc -o slaa slaa.c -s -O3 -Wl,--gc-sections -fno-asynchronous-unwind-tables -nostartfiles --entry=slaa

// Usage:
//	> slaa Path\to\32bit\executable

// #include <windows.h>
#include <Shldisp.h>
#include <stdio.h>

// ----------------------- Project Functions Prototype ------------------------ //
void CommandLineToArgvA(char* cmdLine_cp, char** args_cpa);						// Get arguments from command line.. just a personal preference for char* instead of the wchar_t* type provided by "CommandLineToArgvW()"
void WriteToConsoleA(char* cpMsg);												// "Write to Console A" function to save >20KB compared to printf and <stdio.h>
// void WriteIntToConsoleA(int iNum);											// "Write Integer as Hex to Console A" function to save >20KB compared to printf and <stdio.h>
// void WriteToConsoleW(wchar_t* cpMsg);										// "Write to Console W" function to save >20KB compared to printf and <stdio.h>

// --------------------------- Functions Prototype ---------------------------- //
int access(const char* path, int mode);											// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/access-waccess?view=msvc-160
// void* __stdcall GetStdHandle(int nStdHandle);								// https://docs.microsoft.com/en-us/windows/console/getstdhandle
// void* GetCommandLineA();														// https://docs.microsoft.com/en-us/windows/win32/api/processenv/nf-processenv-getcommandlinea
// int __stdcall WriteConsoleA(void* hConsoleOutput, const char* lpBuffer,int nNumberOfCharsToWrite, unsigned long* lpNumberOfCharsWritten,void* lpReserved);  // https://docs.microsoft.com/en-us/windows/console/writeconsole
// void* CreateFileA(const char* lpFileName, unsigned long dwDesiredAccess, unsigned long dwShareMode, void* lpSecurityAttributes, unsigned long dwCreationDisposition, unsigned long dwFlagsAndAttributes, void* hTemplateFile);  // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
// unsigned long SetFilePointer(void* hFile, long lDistanceToMove, long* lpDistanceToMoveHigh, unsigned long dwMoveMethod);  // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-setfilepointer
// int ReadFile(void* hFile, void* lpBuffer, unsigned long nNumberOfBytesToRead, unsigned long* lpNumberOfBytesRead, void* lpOverlapped);  // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile
// BOOL WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped); // https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile
// int CloseHandle(void* hObject);												// https://docs.microsoft.com/en-us/windows/win32/api/handleapi/nf-handleapi-closehandle
// void* malloc(size_t size);													// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/malloc?view=msvc-160
// void free(void *memblock);													// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/free?view=msvc-160
// void ExitProcess(unsigned int uExitCode);									// https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-exitprocess

// ----------------------------- Global Variables ----------------------------- //
void* __stdcall consOut_vp;

// --------------------------- entry point function --------------------------- //
void slaa() {
	consOut_vp = GetStdHandle(-11);
	unsigned int exitCode_ui = 0;
// Get arguments from command line
	const int nbArgs_i = 1;														// number of expected arguments
	char*	args_cpa[nbArgs_i+1];												// +1: 1st "argument" is this program path
	char*	cmdLine_cp = GetCommandLineA();
	CommandLineToArgvA(cmdLine_cp, args_cpa);									// Get arguments from command line
// Check if an argument was passed
	if(!args_cpa[1]) {
		WriteToConsoleA("\nERROR_BAD_ARGUMENTS: Arguments missing\n");
		WriteToConsoleA("Usage: > slaa Path\\to\\32bit\\executable\n");
		ExitProcess(0xA0); }													// 0xA0: ERROR_BAD_ARGUMENTS	
// Check if 1st argument is a path to an existing file
	if(access(args_cpa[1], 0) < 0 ) {
		WriteToConsoleA("\nERROR_FILE_NOT_FOUND: \""); WriteToConsoleA(args_cpa[1]); WriteToConsoleA("\"\n");
		ExitProcess(0x2); }														// 0x2: ERROR_FILE_NOT_FOUND
// Process file
	void* exeFile_vp = CreateFileA(args_cpa[1], 0xC0000000, 0, NULL, 3, 0, NULL); // 0xC0000000: GENERIC_READ|GENERIC_WRITE; 3: OPEN_EXISTING
	void* exeReadBuf_vp = malloc(4);
	unsigned long* procByte_ulp = 0;
// Get IMAGE_FILE_HEADERS->Characteristics
	SetFilePointer(exeFile_vp, 0x3C, NULL, 0);									// 0x3C: IMAGE_DOS_HEADER->e_lfanew (offset to IMAGE_NT_HEADERS)
	ReadFile(exeFile_vp, exeReadBuf_vp, 4, procByte_ulp, NULL);
	int fileHeadChar_i = *(int*)exeReadBuf_vp + 22;								// 22: IMAGE_NT_HEADERS->IMAGE_FILE_HEADERS->Characteristics
	SetFilePointer(exeFile_vp, fileHeadChar_i, NULL, 0);
	ReadFile(exeFile_vp, exeReadBuf_vp, 2, procByte_ulp, NULL);
// Check if 32bit exe - 0x0100: IMAGE_FILE_32BIT_MACHINE; 0x0002: IMAGE_FILE_EXECUTABLE_IMAGE
	if ((*(short*)exeReadBuf_vp & 0x0102) != 0x0102) {
		WriteToConsoleA("\nERROR_INVALID_DATA: \""); WriteToConsoleA(args_cpa[1]); WriteToConsoleA("\" is not a 32bit executable\n");
		exitCode_ui = 0xD; }
	else {
// Check if LLA flag is already set - 0x0020: IMAGE_FILE_LARGE_ADDRESS_AWARE;
		if ((*(short*)exeReadBuf_vp & 0x0020) == 0x0020 ) {
			WriteToConsoleA("\nUnsetting LLA flag..");
			*(short*)exeReadBuf_vp = *(short*)exeReadBuf_vp & 0xFFDF; }
		else {
			WriteToConsoleA("\nSetting LLA flag..");
			*(short*)exeReadBuf_vp = *(short*)exeReadBuf_vp | 0x0020; }
// Set/Unset LLA flag
		SetFilePointer(exeFile_vp, fileHeadChar_i, NULL, 0);
		WriteFile(exeFile_vp, (short*)exeReadBuf_vp, 2, procByte_ulp, NULL);
		WriteToConsoleA("\n..Done\n"); }
// Clean Up
	free(exeReadBuf_vp);
	CloseHandle(exeFile_vp);
	ExitProcess(exitCode_ui);
}

// -------------------- Get arguments from command line A --------------------- //
// Notes:
//	- Personal preference for char* instead of the wchar_t* provided by "CommandLineToArgvW()"
//	- Probably works with double quoted arguments containing escaped quotes.. in most cases:
//		- "Such as this \"Double Quoted\" Argument with \"Escaped Quotes\" and \\\"Escaped BackSlash\"\\"
void CommandLineToArgvA(char* cmdLine_cp, char** args_cpa) {
	char endChar_c;
	while (*cmdLine_cp) {
		while (*cmdLine_cp && *cmdLine_cp == ' ') cmdLine_cp++;					// Trim white-spaces before the argument
		endChar_c = ' ';														// end of argument is defined as white-space..
		if (*cmdLine_cp == '\"') { endChar_c = '\"'; cmdLine_cp++; }			// ..or as a double quote if argument is between double quotes
		*args_cpa = cmdLine_cp;													// Save argument pointer
// Find end of argument ' ' or '\"', while skipping '\\\"' if endChar_c = '\"'
		int prevBackSlash_b = 0;
		while (*cmdLine_cp && (*cmdLine_cp != endChar_c || (endChar_c == '\"' && prevBackSlash_b))) {
			prevBackSlash_b = 0;
			int checkBackSlash_i = 0;
			while(*(cmdLine_cp-checkBackSlash_i) == '\\') {
				checkBackSlash_i++;
				prevBackSlash_b = !prevBackSlash_b; }
			cmdLine_cp++; }
		if(*cmdLine_cp) {
			*cmdLine_cp = 0; cmdLine_cp++; }
		args_cpa++; }
}

// --------------------------- "Write to Console A" --------------------------- //
// Note: Saves >20KB compared to printf and <stdio.h>
void WriteToConsoleA(char* cpMsg) {
	WriteConsoleA(consOut_vp, cpMsg, strlen(cpMsg), NULL, NULL);
}

// ------------------- "Write Integer as Hex to Console A" -------------------- //
// Note: Saves >20KB compared to printf and <stdio.h>
// void WriteIntToConsoleA(int iNum) {
	// char caHex[19] = {'\0'};
	// char* cpHex = &caHex[17];
	// while(iNum != 0) {
		// int iTmp = iNum % 16;
		// if( iTmp < 10 ) *cpHex = iTmp + 48;
		// else *cpHex = iTmp + 55;
		// iNum = iNum / 16;
		// cpHex--;}
	// *cpHex = 'x'; cpHex--;
	// *cpHex = '0';
	// WriteConsoleA(consOut_vp, cpHex, strlen(cpHex), NULL, NULL);
// }

// --------------------------- "Write to Console W" --------------------------- //
// Note: Saves >20KB compared to printf and <stdio.h>
// void WriteToConsoleW(wchar_t* cpMsg) {
	// WriteConsoleW(consOut_vp, cpMsg, wcslen(cpMsg), NULL, NULL);
// }
