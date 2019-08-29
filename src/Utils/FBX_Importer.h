#pragma once
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "Structs.h"

// Utils
#include "fbxsdk.h"

class FBX_Importer {
public:
	static bool Load(EntityMaterial* entityMaterial, const char* fileName, std::vector<Vertex>*, std::vector<unsigned int>*);
	static void printNode(FbxNode* node, unsigned int level=0);
};