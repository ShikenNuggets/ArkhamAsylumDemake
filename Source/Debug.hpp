#ifndef PS2E_DEBUG_HPP
#define PS2E_DEBUG_HPP

#include <cstdint>

namespace Debug{
	enum LogType : uint8_t{
		Verbose 	= 0,
		Info		= 1,
		Warning		= 2,
		Error		= 3,
		FatalError	= 4
	};

	extern void Log(const char* message, LogType type, const char* fileName, int lineNumber);
};

#ifndef LOG_INFO
	#define LOG_INFO(message) Debug::Log(message, Debug::Info, __FILE__, __LINE__)
#endif //!LOG_INFO

#ifndef LOG_WARNING
	#define LOG_WARNING(message) Debug::Log(message, Debug::Warning, __FILE__, __LINE__)
#endif //!LOG_WARNING

#ifndef LOG_ERROR
	#define LOG_ERROR(message) Debug::Log(message, Debug::Error, __FILE__, __LINE__)
#endif //!LOG_ERROR

#ifndef FATAL_ERROR
	#define FATAL_ERROR(message) Debug::Log(message, Debug::FatalError, __FILE__, __LINE__)
#endif //!FATAL_ERROR

#endif //!PS2E_DEBUG_HPP