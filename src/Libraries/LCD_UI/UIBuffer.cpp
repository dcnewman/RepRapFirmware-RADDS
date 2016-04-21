#include "UIBuffer.h"

UIBuffer::UIBuffer() : gcodeReadIndex(0), gcodeWriteIndex(0)
{
	// Init();
}

void UIBuffer::Init()
{
	gcodeReadIndex = 0;
	gcodeWriteIndex = 0;
}

char UIBuffer::ReadGCode()
{
	char c;
	if (gcodeReadIndex == gcodeWriteIndex)
	{
		c = 0;
	}
	else
	{
		c = gcodeBuffer[gcodeReadIndex];
		gcodeReadIndex = (gcodeReadIndex + 1u) % uiGcodeBufferLength;
	}
	return c;
}

bool UIBuffer::GCodeAvailable()
{
	return gcodeReadIndex != gcodeWriteIndex;
}

void UIBuffer::Put(const char *data)
{
	if (!data) return;
	Put(data, strlen(data));
}

void UIBuffer::Put(const char *data, size_t len)
{
	if (!data || !len) return;

	size_t freeSpace = (gcodeReadIndex - gcodeWriteIndex - 1u) % uiGcodeBufferLength;
	if (len > freeSpace)
		return;

	size_t remaining = uiGcodeBufferLength - gcodeWriteIndex;
	if (len <= remaining)
	{
		memcpy(gcodeBuffer + gcodeWriteIndex, data, len);
	}
	else
	{
		memcpy(gcodeBuffer + gcodeWriteIndex, data, remaining);
		memcpy(gcodeBuffer, data + remaining, len - remaining);
	}
	gcodeWriteIndex = (gcodeWriteIndex + len) % uiGcodeBufferLength;
}
