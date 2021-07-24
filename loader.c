#include <string.h>
#include <stdio.h>
#include <windows.h>

#define ADDR 0x8000000000ll
#define ALEN 0x5000002000ll
#define FNAME "Songs of the Eons.exe"

static char buf[512] = FNAME;

int main(int argc, char ** argv)
{
	strcpy(buf + strlen(buf) + 1, "-sote")[-1] = ' ';
	for (int i=1; i<argc; ++i)
		strcpy(buf + strlen(buf) + 1, argv[i])[-1] = ' ';

	int tries = 0;
	FILE * f;
	if (! (f = fopen("loader.log", "wa")))
		return -2;

	PROCESS_INFORMATION pi;
	STARTUPINFO si;

	retry:

	si.cb = sizeof(si);
	si.lpReserved = 0;
	si.lpDesktop = 0;
	si.lpTitle = 0;
	si.dwX = 0;
	si.dwY = 0;
	si.dwXSize = 0;
	si.dwYSize = 0;
	si.dwXCountChars = 0;
	si.dwYCountChars = 0;
	si.dwFillAttribute = 0;
	si.dwFlags = 0;
	si.wShowWindow = 0;
	si.cbReserved2 = 0;
	si.lpReserved2 = 0;
	si.hStdInput = 0;
	si.hStdOutput = 0;
	si.hStdError = 0;

	if (! CreateProcess(FNAME, buf, 0, 0, 0, 4, 0, 0, &si, &pi))
	{
		int err = GetLastError();
		fprintf(f, "P %d\n", err);
		return 1;
	}

	if (VirtualAllocEx(pi.hProcess, (void *) ADDR, ALEN, 0x2000, 4) != (void *) ADDR)
	{
		int err = GetLastError();
		fprintf(f, "A %d\n", err);

		if (! TerminateProcess(pi.hProcess, -1))
		{
			int err = GetLastError();
			fprintf(f, "T %d\n", err);
			return 1;
		}
		if (! CloseHandle(pi.hThread))
		{
			int err = GetLastError();
			fprintf(f, "C %d\n", err);
			return 1;
		}
		if (! CloseHandle(pi.hProcess))
		{
			int err = GetLastError();
			fprintf(f, "C %d\n", err);
			return 1;
		}
		if (++tries >= 7)
			return 2;
		goto retry;
	}

	if (ResumeThread(pi.hThread) == (unsigned)-1)
	{
		int err = GetLastError();
		fprintf(f, "R %d\n", err);
		return 1;
	}

	if (WaitForSingleObject(pi.hProcess, -1) == (unsigned)-1)
	{
		int err = GetLastError();
		fprintf(f, "W %d\n", err);
		return 1;
	}

	if (! CloseHandle(pi.hThread))
	{
		int err = GetLastError();
		fprintf(f, "C %d\n", err);
		return 1;
	}

	if (! CloseHandle(pi.hProcess))
	{
		int err = GetLastError();
		fprintf(f, "C %d\n", err);
		return 1;
	}

	fprintf(f, "END\n");
	fclose(f);
	return 0;
}

