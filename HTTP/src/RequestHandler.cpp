#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

#ifdef BONUS
// #include <HTTP/LanguageEmbed.hpp>
#include <Utils/Mutex.hpp>
#endif
#include <HTTP/RequestHandler.hpp>
#include <Utils/Uri.hpp>

namespace Http
{
#ifdef BONUS
Mutex RequestHandler::MUTEX("/sem__req");
#endif
RequestHandler::RequestHandler(Config::ServerConfig const &config,
							   const Request &request,
							   Response &response) : config_(config),
													 request_(request),
													 response_(response),
													 target_("." + request_.path)
{
	Uri finalPath(config.root());
#ifndef BONUS
	if (request_.path.length() > config.uri().length())
		finalPath += request_.path.substr(config.uri().length());
#else
	finalPath += request_.path;
#endif
	target_ = Utils::File(finalPath.str());
}
RequestHandler::~RequestHandler() {}

void RequestHandler::build()
{
	std::vector<std::string> matches;
	if (!method_allowed())
	{
		response_.code = 405;
		response_.headers.addHeader("Allow", config_.allowedMethods());
	}
	else if (!authorized())
	{
		response_.code = 401;
		response_.headers.addHeader("WWW-Authenticate", "Basic realm=\"Login required\"");
	}
	else if (!body_size_allowed())
	{
		response_.code = 413;
	}
	else if (target_.IsDirectory() && !indexRedirection() &&
			 (request_.method == "GET" || request_.method == "HEAD") &&
			 config_.autoindex())
	{
		response_.listDirectory(target_.GetPath(), request_.path);
	}
	else if (!target_.IsDirectory() && !target_.Exists() &&
			 (!find_matches(matches) || !content_negotiation(matches)) &&
			 request_.method != "PUT" && request_.method != "POST")
	{
		response_.code = 404;
	}
	else
		handler();
	if (response_.code >= 400 && request_.method != "HEAD")
		set_error_body();
}

void RequestHandler::handler()
{
	if (isCGI())
		handleCGI();
	else if (request_.method == "PUT")
		handlePut();
	else if (request_.method == "POST")
		handlePost();
	else if (request_.method == "DELETE")
		handleDelete();
	else if (request_.method == "GET" || request_.method == "HEAD")
		handleGet();
}

void RequestHandler::handleCGI()
{
#ifdef BONUS
#ifdef BONUS
	MUTEX.lock();
#endif
#endif
	CGI cgi(config_, request_, ext_);
#ifdef BONUS
	MUTEX.unlock();
#endif
	if (cgi.executeCGI())
	{
		size_t endOfHeaders = response_.headers.parseRawMessage(cgi.getBody());
		if (response_.headers.hasHeader("Status"))
		{
			try
			{
				response_.code = Utils::String::to_int(response_.headers.value("Status"));
				if (StatusCodes::createString(response_.code) == "")
					response_.code = 400;
			}
			catch (std::exception &e)
			{
				response_.code = 500;
			}
			response_.headers.removeHeader("Status");
		}
		response_.body = cgi.getBody().substr(endOfHeaders);
	}
	else
	{
		std::cerr << "Error: failed to execute cgi\n";
		response_.code = 500;
	}
}
void RequestHandler::handlePost()
{
#ifdef BONUS
	MUTEX.lock();
#endif
	if (!target_.Exists())
	{
		std::string fileName = target_.GetPath().substr(config_.root().size());
		Uri path = Uri(config_.root()) +
				   Uri(config_.upload()) +
				   Uri(fileName);
		target_ = Utils::File(path.str());
		Uri location(config_.uri());
		location += Uri(config_.upload()) + Uri(request_.path.substr(config_.uri().size()));
		response_.headers.setHeader("Location", location.str());
		// set target path appropriataly to Content-Type and Content-Language
		// + /upload folder
		response_.code = (!target_.Exists()) ? 201 : 200;
	}
	std::cerr << "Post of size " << request_.body.size() << '\n';
	if (target_.OpenReadWrite(O_APPEND))
	{  // creates if it doesn't exist
		if (request_.body.length())
		{
			if (!target_.Write((void *)(request_.body.c_str()), request_.body.length()))
			{
				std::cout << "Error: write failed\n";
				response_.code = 500;
			}
		}
	}
	else
	{
		std::cout << "Failed to open " << target_.GetPath() << '\n';
		response_.code = 500;
	}
#ifdef BONUS
	MUTEX.unlock();
#endif
	std::cout << response_.code << '\n';
}
void RequestHandler::handlePut()
{
	response_.code = 204;
#ifdef BONUS
	MUTEX.lock();
#endif
	if (!target_.Exists())
	{
		std::string fileName = target_.GetPath().substr(config_.root().size());
		Uri path = Uri(config_.root()) +
				   Uri(config_.upload()) +
				   Uri(fileName);
		target_ = Utils::File(path.str());
		Uri location(config_.uri());
		location += Uri(config_.upload()) + Uri(request_.path.substr(config_.uri().size()));
		response_.headers.setHeader("Location", location.str());
		if (!target_.Exists())
			response_.code = 201;
	}
	std::cerr << "Put of size " << request_.body.size() << '\n';
	if (target_.OpenReadWrite(O_TRUNC))
	{
		if (request_.body.length())
		{
			if (!target_.Write((void *)(request_.body.c_str()), request_.body.length()))
			{
				std::cout << "Error: write failed\n";
				response_.code = 500;
			}
		}
	}
	else
	{
		std::cout << "Failed to open " << target_.GetPath() << '\n';
		response_.code = 500;
	}
#ifdef BONUS
	MUTEX.unlock();
#endif
}
void RequestHandler::handleDelete()
{
#ifdef BONUS
	MUTEX.lock();
#endif
	if (target_.Destroy())
	{
		response_.body += "File deleted\n";
		response_.code = 200;
	}
	else
	{
		response_.code = 500;
	}
#ifdef BONUS
	MUTEX.unlock();
#endif
}
void RequestHandler::handleGet()
{
	if (!target_.IsDirectory() && target_.Exists())
	{
		size_t fileSize = target_.getSize();
		if (request_.method == "GET")
		{
			target_.OpenReadOnly();
			target_.ReadAll(response_.body);
		}
		if (!response_.headers.hasHeader("Content-Type"))
		{
			response_.headers.setHeader("Content-Type", Mime::type(target_.GetPath()));
		}
		response_.headers.setHeader("Content-Length", Utils::String::to_string(fileSize));
		response_.headers.addHeader("Last-Modified", target_.GetLastModifiedTime());
	}
	else
		response_.code = 404;
}

bool RequestHandler::indexRedirection()
{
	std::vector<std::string> files;
	Utils::File::ListDirectory(target_.GetPath(), files);

	for (std::vector<std::string>::iterator file = files.begin();
		 file != files.end(); ++file)
	{
		for (std::vector<std::string>::const_iterator index = config_.index().begin();
			 index != config_.index().end(); ++index)
		{
			if (!file->compare(0, index->size(), *index))
			{
				std::string toAdd;
				if (request_.path[request_.path.size() - 1] != '/')
					toAdd = "/";
				toAdd += *index;
				const_cast<Request &>(request_).path += toAdd;
				std::string newPath = target_.GetPath();
				if (newPath[newPath.size() - 1] != '/')
					newPath += "/";
				target_ = Utils::File(newPath + *index);
				return true;
			}
		}
	}
	return false;
}

bool RequestHandler::method_allowed()
{
	return config_.allow(request_.method);
}

bool RequestHandler::body_size_allowed()
{
	int maxSize = config_.maxBodySize();
	return maxSize < 0 || request_.body.size() <= size_t(maxSize);
}

bool RequestHandler::isCGI()
{
	if (!config_.directive("cgi-bin").empty())
	{
		std::vector<std::string> supportedExt = config_.getCGI();
		Uri uri(target_.GetPath());
		std::pair<int, std::string> match = uri.findExt(supportedExt);
		if (match.first != -1)
		{
			ext_ = match.second;
			return true;
		}
	}
	// return request_.path.find(config_.directive("cgi-bin"), 0) != std::string::npos;
	return false;
}

bool RequestHandler::authorized()
{
	return config_.auth_basic().empty() ||
		   (request_.headers.hasHeader("Authorization") &&
			config_.authenticate(request_.headers.value("Authorization")));
}

bool RequestHandler::find_matches(std::vector<std::string> &matches)
{
	size_t dirEnd = target_.GetPath().find_last_of("/\\");
	std::string directory = target_.GetPath().substr(0, dirEnd),
				targetFileName = target_.GetPath().substr(dirEnd + 1);
	if (targetFileName[targetFileName.size() - 1] != '.')
		targetFileName += ".";
	std::vector<std::string> files;
	Utils::File::ListDirectory(directory, files);
	for (size_t i = 0; i < files.size(); ++i)
	{
		if (!files[i].compare(0, targetFileName.size(), targetFileName) &&
			targetFileName.size() < files[i].size())
			matches.push_back(files[i].substr(targetFileName.size() - 1));
	}
	if (matches.size() == 1)
		target_ = target_.GetPath() + matches[0];
	return matches.size() >= 1;
}

int getQuality(std::string const &token)
{
	size_t commentStart = token.find(";"),
		   qualityStart = token.find("q=", commentStart);
	if (commentStart == std::string::npos ||
		qualityStart == std::string::npos)
		return 1000;
	return strtod(token.substr(qualityStart + 2).c_str(), NULL) * 1000;
}

std::string matchByLanguage(std::vector<std::string> Tokens,
							std::map<std::vector<std::string>, std::string> &matches)
{
	std::map<int, std::string, std::greater<int> > LanguageTokens;

	for (std::vector<std::string>::iterator it = Tokens.begin(); it != Tokens.end(); ++it)
		LanguageTokens[getQuality(*it)] = *it;
	for (std::map<int, std::string, std::greater<int> >::iterator Token = LanguageTokens.begin();
		 Token != LanguageTokens.end(); Token++)
	{
		std::map<std::vector<std::string>, std::string> LanguageMatches;
		size_t div = Token->second.find_first_of(";");
		std::string tag = Token->second.substr(0, div);
		if (tag == "*")
			return tag;
		for (std::map<std::vector<std::string>, std::string>::iterator it = matches.begin();
			 it != matches.end(); ++it)
			if (std::count(it->first.begin(), it->first.end(), tag))
				LanguageMatches.insert(*it);
		if (!LanguageMatches.empty())
		{
			matches = LanguageMatches;
			return tag;
		}
	}
	return "";
}

std::string matchByCharset(std::vector<std::string> Tokens,
						   std::map<std::vector<std::string>, std::string> &matches)
{
	std::map<int, std::string, std::greater<int> > CharsetTokens;

	for (std::vector<std::string>::iterator it = Tokens.begin(); it != Tokens.end(); ++it)
		CharsetTokens.insert(std::make_pair(getQuality(*it), *it));
	for (std::map<int, std::string, std::greater<int> >::iterator Token = CharsetTokens.begin();
		 Token != CharsetTokens.end(); Token++)
	{
		std::map<std::vector<std::string>, std::string> CharsetMatches;
		size_t div = Token->second.find_first_of(";");
		std::string tag = Token->second.substr(0, div);
		if (tag == "*")
			return tag;
		for (std::map<std::vector<std::string>, std::string>::iterator it = matches.begin();
			 it != matches.end(); ++it)
			if (std::count(it->first.begin(), it->first.end(), tag))
				CharsetMatches.insert(*it);
		if (!CharsetMatches.empty())
		{
			matches = CharsetMatches;
			return tag;
		}
	}
	return "";
}
bool RequestHandler::content_negotiation(std::vector<std::string> &matches)
{
	std::string matchedCharset, matchedLanguage;
	if (matches.size() == 1)
		return true;

	std::map<std::vector<std::string>, std::string> splitMatches;
	for (size_t i = 0; i < matches.size(); ++i)
		splitMatches.insert(make_pair(Uri::getExtensions(matches[i]), matches[i]));
	if (request_.headers.hasHeader("Accept-Language"))
	{
		matchedLanguage = matchByLanguage(request_.headers.tokens("Accept-Language"), splitMatches);
		if (matchedLanguage != "*" && !matchedLanguage.empty())
			response_.headers.setHeader("Content-Language", matchedLanguage);
		// if (matchedLanguage.empty())
		// 	return false;
	}
	if (request_.headers.hasHeader("Accept-Charset"))
		matchedCharset = matchByCharset(request_.headers.tokens("Accept-Charset"), splitMatches);
	target_ = target_.GetPath() + splitMatches.begin()->second;	 // ! Never clears?
	std::string contentType = Mime::type(target_.GetPath());
	if (!matchedCharset.empty())
		contentType += "; charset=" + matchedCharset;
	response_.headers.setHeader("Content-Type", contentType);
	return true;
}

void RequestHandler::set_error_body()
{
	response_.setBody();
	if (config_.errorPageExists(response_.code))
	{
		Utils::File target(config_.errorPage(response_.code));
		if (target.Exists())
		{
			target.OpenReadOnly();
			target.ReadAll(response_.body);
		}
	}
}

}  // namespace Http
