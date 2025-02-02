#include <string>

namespace FileSystem{
	static constexpr char PathSeparator = '/';

	static std::string GetFileNameFromPath(std::string_view path_){
		size_t i = path_.rfind(PathSeparator, path_.length());
		if(i != std::string::npos){
			return std::string(path_.substr(i + 1, path_.length() - i));
		}

		return std::string(path_);
	}
}