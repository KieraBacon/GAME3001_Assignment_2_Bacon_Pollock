#include "TextureManager.h"
#include <SDL_image.h>

#include <utility>

TextureManager* TextureManager::s_pInstance = nullptr;

TextureManager::TextureManager()
= default;

TextureManager::~TextureManager()
= default;

inline bool TextureManager::m_exists(const std::string & id)
{
	return m_textureMap.find(id) != m_textureMap.end();
}

bool TextureManager::load(const std::string& file_name, const std::string& id, SDL_Renderer * renderer)
{
	if(m_exists(id))
	{
		return true;
	}

	const auto pTempSurface(Config::make_resource(IMG_Load(file_name.c_str())));
	

	if (pTempSurface == nullptr)
	{
		return false;
	}

	const auto pTexture(Config::make_resource(SDL_CreateTextureFromSurface(renderer, pTempSurface.get())));

	// everything went ok, add the texture to our list
	if (pTexture != nullptr)
	{
		m_textureMap[id] = pTexture;
		return true;
	}

	return false;
}

void TextureManager::draw(const std::string& id, const int x, const int y, const int width, const int height, SDL_Renderer * renderer, const SDL_RendererFlip flip)
{
	SDL_Rect srcRect;
	SDL_Rect destRect;
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = destRect.w = width;
	srcRect.h = destRect.h = height;
	destRect.x = x;
	destRect.y = y;
	SDL_RenderCopyEx(renderer, m_textureMap[id].get(), &srcRect, &destRect, 0, nullptr, flip);
}

void TextureManager::draw(const std::string& id, const int x, const int y, SDL_Renderer * renderer, const bool centered, const SDL_RendererFlip flip)
{
	SDL_Rect srcRect;
	SDL_Rect destRect;

	srcRect.x = 0;
	srcRect.y = 0;

	int textureWidth, textureHeight;

	SDL_QueryTexture(m_textureMap[id].get(), nullptr, nullptr, &textureWidth, &textureHeight);

	srcRect.w = destRect.w = textureWidth;
	srcRect.h = destRect.h = textureHeight;

	if (centered) {
		const int xOffset = textureWidth * 0.5;
		const int yOffset = textureHeight * 0.5;
		destRect.x = x - xOffset;
		destRect.y = y - yOffset;
	}
	else {
		destRect.x = x;
		destRect.y = y;
	}

	SDL_RenderCopyEx(renderer, m_textureMap[id].get(), &srcRect, &destRect, 0, nullptr, flip);
}

void TextureManager::draw(const std::string& id, const int x, const int y, const int width, const int height, SDL_Renderer * renderer, const double angle, const int alpha, const SDL_RendererFlip flip)
{
	SDL_Rect srcRect;
	SDL_Rect destRect;
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = destRect.w = width;
	srcRect.h = destRect.h = height;
	destRect.x = x;
	destRect.y = y;

	SDL_SetTextureAlphaMod(m_textureMap[id].get(), alpha);
	SDL_RenderCopyEx(renderer, m_textureMap[id].get(), &srcRect, &destRect, angle, nullptr, flip);
}

void TextureManager::draw(const std::string& id, const int x, const int y, SDL_Renderer * renderer, const double angle, const int alpha, const bool centered, const SDL_RendererFlip flip)
{
	SDL_Rect srcRect;
	SDL_Rect destRect;

	srcRect.x = 0;
	srcRect.y = 0;

	int textureWidth, textureHeight;

	SDL_QueryTexture(m_textureMap[id].get(), nullptr, nullptr, &textureWidth, &textureHeight);

	srcRect.w = destRect.w = textureWidth;
	srcRect.h = destRect.h = textureHeight;

	if (centered) {
		const int xOffset = textureWidth * 0.5;
		const int yOffset = textureHeight * 0.5;
		destRect.x = x - xOffset;
		destRect.y = y - yOffset;
	}
	else {
		destRect.x = x;
		destRect.y = y;
	}

	SDL_SetTextureAlphaMod(m_textureMap[id].get(), alpha);
	SDL_RenderCopyEx(renderer, m_textureMap[id].get(), &srcRect, &destRect, angle, nullptr, flip);
}

void TextureManager::drawFrame(const std::string& id, const int x, const int y, const int width, const int height, const int current_row, const int current_frame, SDL_Renderer * renderer, const SDL_RendererFlip flip)
{
	SDL_Rect srcRect;
	SDL_Rect destRect;

	srcRect.x = width * current_frame;
	srcRect.y = height * (current_row - 1);
	srcRect.w = destRect.w = width;
	srcRect.h = destRect.h = height;
	destRect.x = x;
	destRect.y = y;
	SDL_RenderCopyEx(renderer, m_textureMap[id].get(), &srcRect, &destRect, 0, nullptr, flip);
}

void TextureManager::drawFrame(const std::string& id, const int x, const int y, const int current_row, const int current_frame, SDL_Renderer * renderer, const bool centered, const SDL_RendererFlip flip)
{
	SDL_Rect srcRect;
	SDL_Rect destRect;

	int textureWidth, textureHeight;

	SDL_QueryTexture(m_textureMap[id].get(), nullptr, nullptr, &textureWidth, &textureHeight);

	srcRect.x = textureWidth * current_frame;
	srcRect.y = textureHeight * (current_row - 1);
	srcRect.w = destRect.w = textureWidth;
	srcRect.h = destRect.h = textureHeight;

	if (centered) {
		const int xOffset = textureWidth * 0.5;
		const int yOffset = textureHeight * 0.5;
		destRect.x = x - xOffset;
		destRect.y = y - yOffset;
	}
	else {
		destRect.x = x;
		destRect.y = y;
	}

	SDL_RenderCopyEx(renderer, m_textureMap[id].get(), &srcRect, &destRect, 0, nullptr, flip);
}

void TextureManager::drawFrame(const std::string& id, const int x, const int y, const int width, const int height, const int current_row, const int current_frame, SDL_Renderer * renderer, const double angle, const int alpha, const SDL_RendererFlip flip)
{
	SDL_Rect srcRect;
	SDL_Rect destRect;

	srcRect.x = width * current_frame;
	srcRect.y = height * current_row;
	srcRect.w = destRect.w = width;
	srcRect.h = destRect.h = height;
	destRect.x = x;
	destRect.y = y;

	SDL_SetTextureAlphaMod(m_textureMap[id].get(), alpha);
	SDL_RenderCopyEx(renderer, m_textureMap[id].get(), &srcRect, &destRect, angle, nullptr, flip);
}

void TextureManager::drawFrame(const std::string& id, const int x, const int y, const int current_row, const int current_frame, SDL_Renderer * renderer, const double angle, const int alpha, const bool centered, const SDL_RendererFlip flip)
{
	SDL_Rect srcRect;
	SDL_Rect destRect;

	int textureWidth, textureHeight;

	SDL_QueryTexture(m_textureMap[id].get(), nullptr, nullptr, &textureWidth, &textureHeight);

	srcRect.x = textureWidth * current_frame;
	srcRect.y = textureHeight * current_row;
	srcRect.w = destRect.w = textureWidth;
	srcRect.h = destRect.h = textureHeight;

	if (centered) {
		const int xOffset = textureWidth * 0.5;
		const int yOffset = textureHeight * 0.5;
		destRect.x = x - xOffset;
		destRect.y = y - yOffset;
	}
	else {
		destRect.x = x;
		destRect.y = y;
	}

	SDL_SetTextureAlphaMod(m_textureMap[id].get(), alpha);
	SDL_RenderCopyEx(renderer, m_textureMap[id].get(), &srcRect, &destRect, angle, nullptr, flip);
}

void TextureManager::drawText(const std::string& id, const int x, const int y, SDL_Renderer * renderer, const double angle, const int alpha, const bool centered, const SDL_RendererFlip flip)
{
	SDL_Rect srcRect;
	SDL_Rect destRect;

	srcRect.x = 0;
	srcRect.y = 0;

	int textureWidth, textureHeight;

	SDL_QueryTexture(m_textureMap[id].get(), nullptr, nullptr, &textureWidth, &textureHeight);

	srcRect.w = destRect.w = textureWidth;
	srcRect.h = destRect.h = textureHeight;

	if (centered) {
		const int xOffset = textureWidth * 0.5;
		const int yOffset = textureHeight * 0.5;
		destRect.x = x - xOffset;
		destRect.y = y - yOffset;
	}
	else {
		destRect.x = x;
		destRect.y = y;
	}

	SDL_SetTextureAlphaMod(m_textureMap[id].get(), alpha);
	SDL_RenderCopyEx(renderer, m_textureMap[id].get(), &srcRect, &destRect, angle, nullptr, flip);
}

glm::vec2 TextureManager::getTextureSize(const std::string& id)
{
	int width, height;
	SDL_QueryTexture(m_textureMap[id].get(), nullptr, nullptr, &width, &height);
	glm::vec2 size;

	size.x = width;
	size.y = height;

	return size;
}

void TextureManager::setAlpha(const std::string& id, const Uint8 new_alpha)
{
	auto pTexture = m_textureMap[id];
	SDL_SetTextureAlphaMod(pTexture.get(), new_alpha);
	pTexture = nullptr;
}

void TextureManager::setColour(const std::string& id, const Uint8 red, const Uint8 green, const Uint8 blue)
{
	auto pTexture = m_textureMap[id];
	SDL_SetTextureColorMod(pTexture.get(), red, green, blue);
	pTexture = nullptr;
}

bool TextureManager::addTexture(const std::string& id, std::shared_ptr<SDL_Texture> texture)
{
	if(m_exists(id))
	{
		return true;
	}
	
	m_textureMap[id] = std::move(texture);
	
	return true;
}

SDL_Texture * TextureManager::getTexture(const std::string& id)
{
	return m_textureMap[id].get();
}

void TextureManager::removeTexture(const std::string& id)
{
	m_textureMap.erase(id);
}

int TextureManager::getTextureMapSize() const
{
	return m_textureMap.size();
}

void TextureManager::clean()
{
	m_textureMap.clear();
	std::cout << "TextureMap Cleared,  TextureMap Size: " << m_textureMap.size() << std::endl;
}

void TextureManager::displayTextureMap()
{
	std::cout << "------------ Displaying Texture Map -----------" << std::endl;
	std::cout << "Texture Map size: " << m_textureMap.size() << std::endl;
	auto it = m_textureMap.begin();
	while (it != m_textureMap.end())
	{
		std::cout << it->first << std::endl;
		++it;
	}
}

