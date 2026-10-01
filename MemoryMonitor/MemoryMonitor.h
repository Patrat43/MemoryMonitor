#ifndef MEMORY_MONITOR_H
#define MEMORY_MONITOR_H

#include <iostream>
#include <string>
#include <memory>
#include <cstdlib>
#include <new>		// Required for std::bad_alloc
#include <cstddef>	// Required for std::max_align_t
#include <Windows.h>
#include <fstream>
#include <filesystem>

static std::size_t totalAllocatedBytes = 0;

inline std::ofstream& GetLogFile() {
	//Create the 'Logs' directory if it doesn't exist
	std::filesystem::create_directories("Logs");

	//Save the file inside the 'Logs' subfolder
	static std::ofstream logFile("Logs/memory_log.txt", std::ios::out | std::ios::trunc);
	return logFile;
}
inline std::ofstream& GetCsvLogFile() {
	std::filesystem::create_directories("Logs");
	static std::ofstream csvLog("Logs/memory_log.csv", std::ios::out | std::ios::trunc);
	return csvLog;
}

struct LogFileInitializer {
	LogFileInitializer() {
		GetLogFile(); // Forces creation & truncation as soon as the program launches
		GetCsvLogFile() << "Event Type,File Path,Line Number,Previous Memory (Bytes),Memory Changed (Bytes),Total Allocation (Bytes)" << std::endl;
	}
};

static LogFileInitializer g_logInit;
//Header for ALLOCs for delete to see what to ignore
struct alignas(std::max_align_t) Header {
	std::size_t numBytes;
	bool isTracked;
	const char* file;
	int line;
};

void PrintConsoleLine() {
	// Default fallback width if fetching fails
	int width = 80;

	// Safely get the live console window width
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
		// Calculate total column width
		width = csbi.srWindow.Right - csbi.srWindow.Left;
	}
	for (int i = 0; i <= width; i++) {
		std::cout << '-';
		GetLogFile() << '-';
	}
	std::cout << std::endl;
	GetLogFile() << std::endl;
}

void PrintAlloc(size_t numBytes, const char* file, int line, size_t previousAllocBytes) {
	const char* safeFile = file ? file : "Unknown";

	/// For the console
	std::cout << "[ALLOC] " << safeFile << ':' << line << "\n";
	std::cout << "Previous memory: " << previousAllocBytes << " bytes  |  ";
	std::cout << "Memory allocated: " << numBytes << " bytes  |  ";
	std::cout << "Total allocation: " << totalAllocatedBytes << " bytes\n";

	/// For txt log file
	GetLogFile() << "[ALLOC] " << safeFile << ':' << line << "\n";
	GetLogFile() << "Previous memory: " << previousAllocBytes << " bytes  |  ";
	GetLogFile() << "Memory allocated: " << numBytes << " bytes  |  ";
	GetLogFile() << "Total allocation: " << totalAllocatedBytes << " bytes\n";

	/// For csv log file
	GetCsvLogFile() << "ALLOC," << safeFile << ',' << line << ','
		<< previousAllocBytes << ',' << numBytes << ',' << totalAllocatedBytes << std::endl;

	PrintConsoleLine();
}

void PrintArrayAlloc(size_t numBytes, const char* file, int line, size_t previousAllocBytes) {
	const char* safeFile = file ? file : "Unknown";

	/// For the console
	std::cout << "[ALLOC ARRAY] " << safeFile << ':' << line << "\n";
	std::cout << "Previous memory: " << previousAllocBytes << " bytes  |  ";
	std::cout << "Memory allocated: " << numBytes << " bytes  |  ";
	std::cout << "Total allocation: " << totalAllocatedBytes << " bytes\n";

	/// For txt log file
	GetLogFile() << "[ALLOC ARRAY] " << safeFile << ':' << line << "\n";
	GetLogFile() << "Previous memory: " << previousAllocBytes << " bytes  |  ";
	GetLogFile() << "Memory allocated: " << numBytes << " bytes  |  ";
	GetLogFile() << "Total allocation: " << totalAllocatedBytes << " bytes\n";

	/// For csv log file
	GetCsvLogFile() << "ALLOC ARRAY," << safeFile << ',' << line << ','
		<< previousAllocBytes << ',' << numBytes << ',' << totalAllocatedBytes << std::endl;

	PrintConsoleLine();
}

void PrintDealloc(Header* header, size_t previousAllocBytes, size_t numBytes) {
	const char* safeFile = header->file ? header->file : "Unknown";

	/// For console
	std::cout << "[DEALLOC] " << safeFile << ':' << header->line << "\n";
	std::cout << "Previous memory: " << previousAllocBytes << " bytes  |  ";
	std::cout << "Memory deallocated: " << numBytes << " bytes  |  ";
	std::cout << "Total allocation: " << totalAllocatedBytes << " bytes\n";

	/// For txt log file
	GetLogFile() << "[DEALLOC] " << safeFile << ':' << header->line << "\n";
	GetLogFile() << "Previous memory: " << previousAllocBytes << " bytes  |  ";
	GetLogFile() << "Memory deallocated: " << numBytes << " bytes  |  ";
	GetLogFile() << "Total allocation: " << totalAllocatedBytes << " bytes\n";

	/// For csv log file
	GetCsvLogFile() << "DEALLOC," << safeFile << ',' << header->line << ','
		<< previousAllocBytes << ',' << numBytes << ',' << totalAllocatedBytes << std::endl;

	PrintConsoleLine();
}

void PrintArrayDealloc(Header* header, size_t previousAllocBytes) {
	const char* safeFile = header->file ? header->file : "Unknown";

	/// For console
	std::cout << "[DEALLOC ARRAY] " << safeFile << ':' << header->line << "\n";
	std::cout << "Previous memory: " << previousAllocBytes << " bytes  |  ";
	std::cout << "Memory deallocated: " << header->numBytes << " bytes  |  ";
	std::cout << "Total allocation: " << totalAllocatedBytes << " bytes\n";

	/// For txt log file
	GetLogFile() << "[DEALLOC ARRAY] " << safeFile << ':' << header->line << "\n";
	GetLogFile() << "Previous memory: " << previousAllocBytes << " bytes  |  ";
	GetLogFile() << "Memory deallocated: " << header->numBytes << " bytes  |  ";
	GetLogFile() << "Total allocation: " << totalAllocatedBytes << " bytes\n";

	/// For csv log file
	GetCsvLogFile() << "DEALLOC ARRAY," << safeFile << ',' << header->line << ','
		<< previousAllocBytes << ',' << header->numBytes << ',' << totalAllocatedBytes << std::endl;

	PrintConsoleLine();
}


struct MemoryLeakChecker {
	~MemoryLeakChecker() {
		PrintConsoleLine();
		if (totalAllocatedBytes > 0) {
			std::cout << "[MEMORY LEAK DETECTED] " << totalAllocatedBytes << " bytes left unfreed!\n";
			GetLogFile() << "[MEMORY LEAK DETECTED] " << totalAllocatedBytes << " bytes left unfreed!\n";
		}
		else {
			std::cout << "[NO LEAKS DETECTED] All memory clean.\n";
			GetLogFile() << "[NO LEAKS DETECTED] All memory clean.\n";
		}
		PrintConsoleLine();
	}
};

static MemoryLeakChecker g_leakChecker;


//Allocs untracked
void* operator new(std::size_t numBytes) {
	Header* header = static_cast<Header*>(std::malloc(numBytes + sizeof(Header)));
	if (!header) throw std::bad_alloc();
	header->numBytes = numBytes;
	header->isTracked = false; // Silently allocate
	header->file = nullptr;
	header->line = 0;
	return header + 1;
}

void* operator new[](std::size_t numBytes) {
	Header* header = static_cast<Header*>(std::malloc(numBytes + sizeof(Header)));
	if (!header) throw std::bad_alloc();
	header->numBytes = numBytes;
	header->isTracked = false; // Silently allocate
	header->file = nullptr;
	header->line = 0;
	return header + 1;
}

void* operator new(std::size_t numBytes, std::align_val_t al) {
	Header* header = static_cast<Header*>(_aligned_malloc(numBytes + sizeof(Header), static_cast<size_t>(al)));
	if (!header) throw std::bad_alloc();
	header->numBytes = numBytes;
	header->isTracked = false;
	header->file = nullptr;
	header->line = 0;

	return header + 1;
}
void* operator new[](std::size_t numBytes, std::align_val_t al) {
	Header* header = static_cast<Header*>(_aligned_malloc(numBytes + sizeof(Header), static_cast<size_t>(al)));
	if (!header) throw std::bad_alloc();
	header->numBytes = numBytes;
	header->isTracked = false; // Silently allocate
	header->file = nullptr;
	header->line = 0;
	return header + 1;
}

//Allocs tracked
void* operator new(std::size_t numBytes, const char* file, int line) {
	size_t previousAllocBytes = totalAllocatedBytes;
	totalAllocatedBytes += numBytes;
	PrintAlloc(numBytes, file, line, previousAllocBytes);

	Header* header = static_cast<Header*>(std::malloc(numBytes + sizeof(Header)));
	if (!header) throw std::bad_alloc();
	header->numBytes = numBytes;
	header->isTracked = true;
	header->file = file;
	header->line = line;
	return header + 1;
}

void* operator new[](std::size_t numBytes, const char* file, int line) {
	size_t previousAllocBytes = totalAllocatedBytes;
	totalAllocatedBytes += numBytes;
	PrintArrayAlloc(numBytes, file, line, previousAllocBytes);

	Header* header = static_cast<Header*>(std::malloc(numBytes + sizeof(Header)));
	if (!header) throw std::bad_alloc();
	header->numBytes = numBytes;
	header->isTracked = true;
	header->file = file;
	header->line = line;
	return header + 1;
}

void* operator new(std::size_t numBytes, std::align_val_t al, const char* file, int line) {
	size_t previousAllocBytes = totalAllocatedBytes;
	totalAllocatedBytes += numBytes;
	PrintAlloc(numBytes, file, line, previousAllocBytes);
	// Round size up or align manually if strict alignment requires extra space
	Header* header = static_cast<Header*>(_aligned_malloc(numBytes + sizeof(Header), static_cast<size_t>(al)));
	if (!header) throw std::bad_alloc();

	header->numBytes = numBytes;
	header->isTracked = true;
	header->file = file;
	header->line = line;
	return header + 1;
}

void* operator new[](std::size_t numBytes, std::align_val_t al, const char* file, int line) {
	size_t previousAllocBytes = totalAllocatedBytes;
	totalAllocatedBytes += numBytes;
	PrintArrayAlloc(numBytes, file, line, previousAllocBytes);

	Header* header = static_cast<Header*>(_aligned_malloc(numBytes + sizeof(Header), static_cast<size_t>(al)));
	if (!header) throw std::bad_alloc();
	header->numBytes = numBytes;
	header->isTracked = true;
	header->file = file;
	header->line = line;
	return header + 1;
}

// --- CRT DEBUG ALLOCATORS (_NORMAL_BLOCK Overloads) ---
// Forwards CRT debug allocation calls directly to your tracked allocator.
void* operator new(std::size_t numBytes, int blockType, const char* file, int line) {
	return operator new(numBytes, file, line);
}

void* operator new[](std::size_t numBytes, int blockType, const char* file, int line) {
	return operator new[](numBytes, file, line);
}

//Deallocs
void operator delete(void* memLocation, std::size_t numBytes) {
	if (!memLocation) return;

	//Step back to the header
	Header* header = static_cast<Header*>(memLocation) - 1;

	if (header->isTracked) {
		size_t previousAllocBytes = totalAllocatedBytes;
		totalAllocatedBytes -= numBytes;
		PrintDealloc(header, previousAllocBytes, numBytes);
	}

	free(header);
}

void operator delete[](void* memLocation) {
	if (!memLocation) return;

	//Step back to header
	Header* header = static_cast<Header*>(memLocation) - 1;

	if (header->isTracked) {
		size_t previousAllocBytes = totalAllocatedBytes;
		totalAllocatedBytes -= header->numBytes;
		PrintArrayDealloc(header, previousAllocBytes);
	}

	std::free(header);
}

#define _new new(_NORMAL_BLOCK, __FILE__, __LINE__)

#endif// MEMORY_MONITOR_H