#include "MojangAPI.h"
#include "json.hpp"
#include <iostream>
#include <cstdint>

#ifdef _WIN32
#include <windows.h>
#include <wininet.h>
#endif

using json = nlohmann::json;

static const char BASE64_TABLE[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string MojangAPI::httpGet(const std::string &host, const std::string &path)
{
#ifdef _WIN32
	HINTERNET hSession = InternetOpenA("SFMLGame/1.0", INTERNET_OPEN_TYPE_PRECONFIG,
		nullptr, nullptr, 0);
	if (!hSession)
	{
		std::cerr << "[MojangAPI] InternetOpen failed for " << host << path << std::endl;
		return "";
	}

	HINTERNET hConnect = InternetConnectA(hSession, host.c_str(),
		INTERNET_DEFAULT_HTTPS_PORT, nullptr, nullptr,
		INTERNET_SERVICE_HTTP, 0, 0);
	if (!hConnect)
	{
		std::cerr << "[MojangAPI] InternetConnect failed for " << host << path << std::endl;
		InternetCloseHandle(hSession);
		return "";
	}

	const char *acceptTypes[] = {"*/*", nullptr};
	HINTERNET hRequest = HttpOpenRequestA(hConnect, "GET", path.c_str(),
		nullptr, nullptr, acceptTypes,
		INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD, 0);
	if (!hRequest)
	{
		std::cerr << "[MojangAPI] HttpOpenRequest failed for " << host << path << std::endl;
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hSession);
		return "";
	}

	DWORD timeout = 10000;
	InternetSetOption(hRequest, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
	InternetSetOption(hRequest, INTERNET_OPTION_SEND_TIMEOUT, &timeout, sizeof(timeout));

	if (!HttpSendRequestA(hRequest, nullptr, 0, nullptr, 0))
	{
		std::cerr << "[MojangAPI] HTTP send failed for " << host << path << std::endl;
		InternetCloseHandle(hRequest);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hSession);
		return "";
	}

	std::string result;
	char buffer[4096];
	DWORD bytesRead = 0;
	while (InternetReadFile(hRequest, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0)
	{
		buffer[bytesRead] = '\0';
		result.append(buffer, bytesRead);
	}

	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hSession);
	return result;
#else
	// Fallback for non-Windows platforms (sf::Http without HTTPS)
	std::string fullHost = "http://" + host;
	sf::Http http(fullHost);
	sf::Http::Request req(path, sf::Http::Request::Method::Get);
	req.setField("User-Agent", "SFMLGame/1.0");

	sf::Http::Response resp = http.sendRequest(req, sf::seconds(10));
	auto status = resp.getStatus();

	if (status != sf::Http::Response::Status::Ok)
	{
		std::cerr << "[MojangAPI] HTTP " << static_cast<int>(status)
				  << " for " << host << path << std::endl;
		return "";
	}
	return resp.getBody();
#endif
}

std::string MojangAPI::formatUUID(const std::string &uuid)
{
	if (uuid.size() != 32)
		return uuid;
	return uuid.substr(0, 8) + "-" +
		   uuid.substr(8, 4) + "-" +
		   uuid.substr(12, 4) + "-" +
		   uuid.substr(16, 4) + "-" +
		   uuid.substr(20, 12);
}

std::string MojangAPI::base64Decode(const std::string &input)
{
	std::string result;
	result.reserve(input.size() * 3 / 4);

	int val = 0, valb = -8;
	for (unsigned char c : input)
	{
		if (c == '=')
			break;
		const char *p = nullptr;
		for (size_t i = 0; i < 64; ++i)
		{
			if (BASE64_TABLE[i] == c)
			{
				p = BASE64_TABLE + i;
				break;
			}
		}
		if (!p)
			continue;
		val = (val << 6) | static_cast<int>(p - BASE64_TABLE);
		valb += 6;
		if (valb >= 0)
		{
			result.push_back(static_cast<char>((val >> valb) & 0xFF));
			valb -= 8;
		}
	}
	return result;
}

bool MojangAPI::fetchPlayerHead(const std::string &playerName, const std::string &outputPath)
{
	std::cout << "[MojangAPI] Fetching head for: " << playerName << std::endl;

	try
	{
	// Step 1: Get player UUID
	std::string profile = httpGet("api.mojang.com", "/users/profiles/minecraft/" + playerName);
	if (profile.empty())
	{
		std::cerr << "[MojangAPI] Failed to get profile" << std::endl;
		return false;
	}
	json profileJson = json::parse(profile);
	std::string uuid = profileJson.value("id", "");
	if (uuid.empty())
	{
		std::cerr << "[MojangAPI] Failed to extract UUID" << std::endl;
		return false;
	}
	std::cout << "[MojangAPI] UUID: " << uuid << std::endl;

	// Step 2: Format UUID
	std::string formattedUUID = formatUUID(uuid);

	// Step 3: Get session profile
	std::string session = httpGet("sessionserver.mojang.com",
		"/session/minecraft/profile/" + formattedUUID + "?unsigned=false");
	if (session.empty())
	{
		std::cerr << "[MojangAPI] Failed to get session profile" << std::endl;
		return false;
	}
	json sessionJson = json::parse(session);
	if (!sessionJson.contains("properties") || !sessionJson["properties"].is_array() ||
		sessionJson["properties"].empty())
	{
		std::cerr << "[MojangAPI] No properties in session profile" << std::endl;
		return false;
	}
	std::string propsVal = sessionJson["properties"][0].value("value", "");
	if (propsVal.empty())
	{
		std::cerr << "[MojangAPI] Failed to extract properties value" << std::endl;
		return false;
	}

	// Step 4: Base64 decode
	std::string decoded = base64Decode(propsVal);
	if (decoded.empty())
	{
		std::cerr << "[MojangAPI] Base64 decode failed" << std::endl;
		return false;
	}

	// Step 5: Extract textures.SKIN.url
	json texturesJson = json::parse(decoded);
	if (!texturesJson.contains("textures") ||
		!texturesJson["textures"].contains("SKIN") ||
		!texturesJson["textures"]["SKIN"].contains("url"))
	{
		std::cerr << "[MojangAPI] Failed to extract skin URL" << std::endl;
		return false;
	}
	std::string skinUrl = texturesJson["textures"]["SKIN"]["url"].get<std::string>();
	if (skinUrl.empty())
	{
		std::cerr << "[MojangAPI] Failed to extract skin URL" << std::endl;
		return false;
	}
	std::cout << "[MojangAPI] Skin URL: " << skinUrl << std::endl;

	// Step 6: Download skin
	std::string skinHost, skinPath;
	std::string skinRemainder = skinUrl;
	if (skinRemainder.compare(0, 7, "http://") == 0)
		skinRemainder = skinRemainder.substr(7);
	else if (skinRemainder.compare(0, 8, "https://") == 0)
		skinRemainder = skinRemainder.substr(8);

	size_t slashPos = skinRemainder.find('/');
	if (slashPos != std::string::npos)
	{
		skinHost = skinRemainder.substr(0, slashPos);
		skinPath = skinRemainder.substr(slashPos);
	}
	else
	{
		skinHost = skinRemainder;
		skinPath = "/";
	}

	std::string skinData = httpGet(skinHost, skinPath);
	if (skinData.empty())
	{
		std::cerr << "[MojangAPI] Failed to download skin" << std::endl;
		return false;
	}

	// Step 7: Load skin image (64x64)
	sf::Image skin;
	if (!skin.loadFromMemory(skinData.data(), skinData.size()))
	{
		std::cerr << "[MojangAPI] Failed to decode skin image" << std::endl;
		return false;
	}

	// Build 8x8 head from skin face region (Minecraft 64x64 format)
	// Face: (8,8)-(15,15), Hat overlay: (40,8)-(47,15)
	std::uint8_t headPixels[8 * 8 * 4] = {};
	for (unsigned int y = 0; y < 8; ++y)
	{
		for (unsigned int x = 0; x < 8; ++x)
		{
			sf::Color c = skin.getPixel({8u + x, 8u + y});
			std::size_t idx = (y * 8 + x) * 4;
			headPixels[idx + 0] = c.r;
			headPixels[idx + 1] = c.g;
			headPixels[idx + 2] = c.b;
			headPixels[idx + 3] = c.a;
		}
	}
	for (unsigned int y = 0; y < 8; ++y)
	{
		for (unsigned int x = 0; x < 8; ++x)
		{
			sf::Color hat = skin.getPixel({40u + x, 8u + y});
			if (hat.a > 0)
			{
				std::size_t idx = (y * 8 + x) * 4;
				headPixels[idx + 0] = hat.r;
				headPixels[idx + 1] = hat.g;
				headPixels[idx + 2] = hat.b;
				headPixels[idx + 3] = hat.a;
			}
		}
	}
	sf::Image head;
	head.resize({8, 8}, headPixels);

	// Step 8: Save
	if (!head.saveToFile(outputPath))
	{
		std::cerr << "[MojangAPI] Failed to save head to: " << outputPath << std::endl;
		return false;
	}

	std::cout << "[MojangAPI] Head saved to: " << outputPath << std::endl;
	return true;
	}
	catch (const json::parse_error &e)
	{
		std::cerr << "[MojangAPI] JSON parse error: " << e.what() << std::endl;
		return false;
	}
	catch (const json::exception &e)
	{
		std::cerr << "[MojangAPI] JSON error: " << e.what() << std::endl;
		return false;
	}
	catch (const std::exception &e)
	{
		std::cerr << "[MojangAPI] Error: " << e.what() << std::endl;
		return false;
	}
}
