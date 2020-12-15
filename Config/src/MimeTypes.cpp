#include <Config/MimeTypes.hpp>
#include <Utils/String.hpp>
#include <Utils/Uri.hpp>
#include <algorithm>
#include <vector>

std::map<std::string, std::string> Mime::type_;

const std::string MIME_SETUP_PATH = "Config/include/Config/mime.types";

void Mime::initMap()
{
	char cwd[256];
	getcwd(cwd, 256);
	std::string line, ext, path;
	Uri filepath(cwd);
	filepath += MIME_SETUP_PATH;
	std::string::size_type first,
		last;
	std::ifstream file(filepath.str().c_str());
	if (!file.good())
		throw std::runtime_error("failed to access mime types file on path '" + filepath.str() + "'");
	while (getline(file, line))
	{
		if (line[0] == '#')
			continue;
		last = line.find_first_of("\t ");
		path = line.substr(0, last);
		while (last < line.size())
		{
			first = line.find_first_not_of("\t ", last);
			last = line.find_first_of("\t ", first);
			ext = line.substr(first, last - first);
			type_[ext] = path;
		}
	}
	file.close();
}

std::string Mime::type(std::string path)
{
	static bool initialized = false;
	if (!initialized)
	{
		Mime::initMap();
		initialized = true;
	}
	path = Utils::String::tolower(path);
	size_t last = path.find_last_of("\\/");
	if (last == std::string::npos && type_.count(path))
		return type_[path];
	std::string file = path.substr(last + 1);
	std::vector<std::string> tags;
	for (size_t dot = file.find_last_of(".");
		 dot != std::string::npos;
		 dot = file.find_last_of("."))
	{
		tags.push_back(file.substr(dot + 1));
		file.erase(dot);
	}
	for (std::vector<std::string>::iterator ext = tags.begin(); ext != tags.end(); ext++)
		if (type_.count(*ext))
			return type_[*ext];
	return "application/octet-stream";
}
