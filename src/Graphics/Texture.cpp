#include "Texture.h"

Texture::Texture(const char* __textureReferenceName, const char* __ddsFileName){
	std::string _ddsFileName(__ddsFileName);
	std::string _textureReferenceName(__textureReferenceName);

	std::wstring ddsFileName(_ddsFileName.begin(), _ddsFileName.end());
	std::wstring textureReferenceName(_textureReferenceName.begin(), _textureReferenceName.end());

	this->fileName = ddsFileName;
	this->textureReferenceName = textureReferenceName;
}