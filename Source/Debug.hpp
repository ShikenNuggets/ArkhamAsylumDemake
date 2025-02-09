#ifndef PS2E_DEBUG_HPP
#define PS2E_DEBUG_HPP

#include <cstdint>

#include "FileSystem.hpp"

namespace Debug{
	enum LogType : uint8_t{
		Verbose 	= 0,
		Info		= 1,
		Warning		= 2,
		Error		= 3,
		FatalError	= 4
	};

	template <typename... Args>
	inline void Log(const char* message, LogType type, const char* fileName, int lineNumber, Args... args){
		const char* typePrefix = "";
		switch(type){
			case Debug::Warning:
				typePrefix = "WARNING";
				break;
			case Debug::Error:
				typePrefix = "ERROR";
				break;
			case Debug::FatalError:
				typePrefix = "FATAL ERROR";
				break;
			default:
				typePrefix = "LOG";
				break;
		}

		std::printf("[PS2E] %s: ", typePrefix);

		if constexpr(sizeof...(args) == 0){
			std::printf("%s", message);
		}else{
			std::printf(message, args...);
		}
		
		std::printf("   (%s:%d)\n", FileSystem::GetFileNameFromPath(fileName).c_str(), lineNumber);
	
		if(type == Debug::FatalError){
			std::exit(-1);
		}
	}
};

#ifndef LOG_VERBOSE
	#define LOG_VERBOSE(message, ...) Debug::Log(message, Debug::Verbose, __FILE__, __LINE__, ##__VA_ARGS__)
#endif //!LOG_VERBOSE

#ifndef LOG_INFO
	#define LOG_INFO(message, ...) Debug::Log(message, Debug::Info, __FILE__, __LINE__, ##__VA_ARGS__)
#endif //!LOG_INFO

#ifndef LOG_WARNING
	#define LOG_WARNING(message, ...) Debug::Log(message, Debug::Warning, __FILE__, __LINE__, ##__VA_ARGS__)
#endif //!LOG_WARNING

#ifndef LOG_ERROR
	#define LOG_ERROR(message, ...) Debug::Log(message, Debug::Error, __FILE__, __LINE__, ##__VA_ARGS__)
#endif //!LOG_ERROR

#ifndef FATAL_ERROR
	#define FATAL_ERROR(message, ...) Debug::Log(message, Debug::FatalError, __FILE__, __LINE__, ##__VA_ARGS__)
#endif //!FATAL_ERROR

#endif //!PS2E_DEBUG_HPP