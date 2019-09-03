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
		std::vector<Joint*>* _joints,
		std::map<int, int>& _indexed_vertices,
		std::map<int, std::map<int, double>>& _indexed_joint_weights
	);
	static void printNode(FbxNode* node, unsigned int level=0);
	static Joint* getJointByName(const char* jointName, std::vector<Joint*>* _joints);
	static dx::XMFLOAT4X4* MatrixFBXtoDX(FbxAMatrix matrix4x4);
};