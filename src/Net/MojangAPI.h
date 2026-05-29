#pragma once
#include <SFML/Network/Http.hpp>
#include <SFML/Graphics/Image.hpp>
#include <string>

class MojangAPI
{
public:
	static bool fetchPlayerHead(const std::string &playerName, const std::string &outputPath);

private:
	static std::string httpGet(const std::string &host, const std::string &path);
	static std::string formatUUID(const std::string &uuid);
	static std::string base64Decode(const std::string &input);
};
