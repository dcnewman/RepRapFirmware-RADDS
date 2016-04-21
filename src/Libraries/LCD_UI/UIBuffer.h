#ifndef UIBUFFER_H
#define UIBUFFER_H

#include <cstddef> // for size_t
#include <string.h> // for memcpy()

const size_t uiGcodeBufferLength = 48;

class UIBuffer {

public:
	UIBuffer();
	void Init();
	bool GCodeAvailable();
	void Put(const char *str);
	void Put(const char *str, size_t len);
	char ReadGCode();

private:
	char gcodeBuffer[uiGcodeBufferLength];
	size_t gcodeReadIndex;
	size_t gcodeWriteIndex;
};

#endif
