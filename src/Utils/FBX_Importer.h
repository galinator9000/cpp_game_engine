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
	static bool Load(
		const char* fileName,
		std::vector<Vertex>* _vertices,
		std::vector<unsigned int>* _indices,
		std::vector<Joint>* _joints
	);
	static void printNode(FbxNode* node, unsigned int level=0);
};