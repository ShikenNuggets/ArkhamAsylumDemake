#include "Debug.hpp"

#include <cstdio>
#include <cstdlib>

#include "FileSystem.hpp"

void Debug::Log(const char* message, Debug::LogType type, const char* fileName, int lineNumber){
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

	std::printf("%s: %s (%s:%d)\n", typePrefix, message, FileSystem::GetFileNameFromPath(fileName).c_str(), lineNumber);

	if(type == Debug::FatalError){
		std::exit(-1);
	}
}