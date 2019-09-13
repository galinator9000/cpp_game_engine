#include "FBX_Importer.h"

bool FBX_Importer::Load(
		const char* fileName,
		std::vector<Vertex>* _vertices,
		std::vector<unsigned int>* _indices,
		std::vector<Joint*>* _joints,
		std::vector<Animation*>* _animations,
		std::map<int, int>& _indexed_vertices,
		std::map<int, std::map<int, float>>& _indexed_joint_weights,
		const char* mainMeshName
	)
{
	FbxManager* fbxSdkManager = FbxManager::Create();
	FbxImporter* fbxImporter = FbxImporter::Create(fbxSdkManager, "");

	// Status object for holding errors.
	FbxStatus status;

	// Load file.
	if (!fbxImporter->Initialize(fileName, -1, fbxSdkManager->GetIOSettings())) {
		std::ostringstream myStream;
		myStream << fileName;
		myStream << " failed to load." << "\n";
		OutputDebugStringA(myStream.str().c_str());
		std::cout << myStream.str().c_str();

		return false;
	}

	// Create scene object.
	FbxScene* fbxScene = FbxScene::Create(fbxSdkManager, "scene");
	fbxImporter->Import(fbxScene);
	fbxImporter->Destroy();

	//// Process scene.
	FbxNode* rootNode = fbxScene->GetRootNode();

	// Triangulate meshes if needed.
	FbxGeometryConverter fbxGeometryConverter(fbxSdkManager);
	Triangulate(rootNode, fbxGeometryConverter);

	// Walk scene and print out all nodes.
	//FBX_Importer::printNode(rootNode);

	// Convert current scene's axis system to DirectX
	// Only affects child nodes of the root node's transformations, not control points.
	/*FbxAxisSystem fbxAxisSystem_DirectX = FbxAxisSystem(FbxAxisSystem::eDirectX);
	fbxAxisSystem_DirectX.ConvertScene(fbxScene);*/
	
	//// Process mesh.
	// Get mesh from scene object.
	FbxNode* meshNode = getMainMeshNode(rootNode, mainMeshName);
	if (meshNode == NULL) {
		std::ostringstream myStream;
		myStream << fileName;
		myStream << ": There isn't a mesh node in current scene." << "\n";
		OutputDebugStringA(myStream.str().c_str());
		std::cout << myStream.str().c_str();
		return false;
	}

	FbxMesh* mesh = meshNode->GetMesh();

	// Check if mesh contains non-triangle polygons, if exists, triangulate entire mesh.
	for (int p = 0; p < mesh->GetPolygonCount(); p++) {
		if (mesh->GetPolygonSize(p) != 3) {
			std::ostringstream myStream;
			myStream << fileName;
			myStream << " contains polygons with more than 3 vertices." << "\n";
			OutputDebugStringA(myStream.str().c_str());
			std::cout << myStream.str().c_str();
			return false;
		}
	}

	//// Process all vertices, indices, normals (per-vertex), and UV values from the mesh.
	// Get values that indicates how normals are represented in the mesh.
	// If faces of the mesh shares vertices, that means one vertex has more than one normal vector.
	// If that's the case, we duplicate the vertices and make faces independent from each other.
	FbxGeometryElementNormal* fbxNormalElement = mesh->GetElementNormal();
	FbxLayerElement::EMappingMode fbxNormalMapMode = fbxNormalElement->GetMappingMode();
	FbxLayerElement::EReferenceMode fbxNormalReferenceMode = fbxNormalElement->GetReferenceMode();

	// Get vertices (control points).
	FbxVector4* fbxVertices = mesh->GetControlPoints();

	// Get first UV set.
	FbxStringList uvNames;
	mesh->GetUVSetNames(uvNames);
	const char* uvName = uvNames[0];

	int polygonCount = mesh->GetPolygonCount();
	switch (fbxNormalMapMode) {
		// Comments are from Autodesk FBX SDK

		// The mapping is undetermined.
		case FbxGeometryElement::eNone:
			return false;
			break;

		// There will be one mapping coordinate for each surface control point/vertex.
		case FbxGeometryElement::eByControlPoint:
			break;

		// There will be one mapping coordinate for each vertex, for every polygon of which it is a part.
		// This means that a vertex will have as many mapping coordinates as polygons of which it is a part.
		case FbxGeometryElement::eByPolygonVertex:
			_vertices->reserve(
				(size_t)(polygonCount * 3)
			);

			// Collect all vertices.
			for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++) {
				// Process every vertex in this polygon (triangle)
				for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {

					// Get the control point index of the current vertex.
					int controlPointIndex = mesh->GetPolygonVertex(polygonIndex, vertexIndex);

					// Get the UV coordinates of the current vertex for texture.
					FbxVector2 texture_UV;
					bool unmapped;
					bool result = mesh->GetPolygonVertexUV(polygonIndex, vertexIndex, uvName, texture_UV, unmapped);

					float texture_U = 0;
					float texture_V = 0;

					if (result && !unmapped) {
						texture_U = (float) texture_UV.mData[0];
						texture_V = 1.0f - (float) texture_UV.mData[1];
					}
					Vertex* vertex = new Vertex{
						// Position of the Vertex
						{
							(float)fbxVertices[controlPointIndex].mData[0],
							(float)fbxVertices[controlPointIndex].mData[2],
							(float)fbxVertices[controlPointIndex].mData[1]
						},
						// Fill normals and colors of the Vertex with zeros temporarily.
						{
							0,0,0
						},
						// Texture UV coordinates of the Vertex
						{
							texture_U, texture_V
						}
					};

					// Fill joint indices and weights initially.
					for (int j = 0; j < MAX_JOINT_PER_VERTEX; j++) {
						vertex->jointIDs[j] = -1;
						vertex->jointWeights[j] = 0.0f;
					}

					_indexed_vertices[(int) _vertices->size()] = controlPointIndex;
					_vertices->push_back(*vertex);
				}
			}
			_vertices->shrink_to_fit();

			// Collect all indices.
			_indices->reserve(
				(size_t) (polygonCount * 3)
			);
			for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++) {
				// Process every vertex in this polygon (triangle)
				for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
					_indices->push_back(
						(unsigned int) polygonIndex * 3 + (2-vertexIndex)
					);
				}
			}
			_indices->shrink_to_fit();
			break;

		// There can be only one mapping coordinate for the whole polygon.
		case FbxGeometryElement::eByPolygon:
			break;

		// There will be one mapping coordinate for each unique edge in the mesh.
		// This is meant to be used with smoothing layer elements.
		case FbxGeometryElement::eByEdge:
			break;

		// There can be only one mapping coordinate for the whole surface.
		case FbxGeometryElement::eAllSame:
			break;
	}

	int indexByPolygonVertex = 0;
	switch (fbxNormalMapMode) {
		// Comments are from Autodesk FBX SDK

		// The mapping is undetermined.
		case FbxGeometryElement::eNone:
			break;

		// There will be one mapping coordinate for each surface control point/vertex.
		case FbxGeometryElement::eByControlPoint:
			break;
			/*
			for (int vertexIndex = 0; vertexIndex < mesh->GetControlPointsCount(); vertexIndex++) {
				int normalIndex = -1;

				switch (fbxNormalReferenceMode) {
				// This indicates that the mapping information for the n'th element is found in the n'th place of FbxLayerElementTemplate::mDirectArray.
				case FbxGeometryElement::eDirect:
					normalIndex = vertexIndex;
					break;

				// This indicates that the FbxLayerElementTemplate::mIndexArray contains, for the n'th element, an index in the FbxLayerElementTemplate::mDirectArray array of mapping elements.
				// eIndexToDirect is usually useful for storing eByPolygonVertex mapping mode elements coordinates.
				// Since the same coordinates are usually repeated many times, this saves spaces by storing the coordinate only one time and then referring to them with an index.
				// Materials and Textures are also referenced with this mode and the actual Material/Texture can be accessed via the FbxLayerElementTemplate::mDirectArray
				case FbxGeometryElement::eIndexToDirect:
					normalIndex = fbxNormalElement->GetIndexArray().GetAt(vertexIndex);
					break;
				}

				// Place normal to our Vertex object.
				assert(normalIndex != -1);

				_vertices->at(vertexIndex).normal.x = (float) fbxNormalElement->GetDirectArray().GetAt(normalIndex)[0];
				_vertices->at(vertexIndex).normal.y = (float) fbxNormalElement->GetDirectArray().GetAt(normalIndex)[2];
				_vertices->at(vertexIndex).normal.z = (float) fbxNormalElement->GetDirectArray().GetAt(normalIndex)[1];
			}
			*/

		// There will be one mapping coordinate for each vertex, for every polygon of which it is a part.
		// This means that a vertex will have as many mapping coordinates as polygons of which it is a part.
		case FbxGeometryElement::eByPolygonVertex:
			for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++) {
				// Process every vertex in this polygon (triangle)
				for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
					indexByPolygonVertex = polygonIndex * 3 + vertexIndex;
					int normalIndex = -1;

					switch (fbxNormalReferenceMode) {
						// This indicates that the mapping information for the n'th element is found in the n'th place of FbxLayerElementTemplate::mDirectArray.
						case FbxGeometryElement::eDirect:
							normalIndex = indexByPolygonVertex;
							break;

						// This indicates that the FbxLayerElementTemplate::mIndexArray contains, for the n'th element, an index in the FbxLayerElementTemplate::mDirectArray array of mapping elements.
						// eIndexToDirect is usually useful for storing eByPolygonVertex mapping mode elements coordinates.
						// Since the same coordinates are usually repeated many times, this saves spaces by storing the coordinate only one time and then referring to them with an index.
						// Materials and Textures are also referenced with this mode and the actual Material/Texture can be accessed via the FbxLayerElementTemplate::mDirectArray
						case FbxGeometryElement::eIndexToDirect:
							normalIndex = fbxNormalElement->GetIndexArray().GetAt(indexByPolygonVertex);
							break;
					}

					// Place normal to our Vertex object.
					assert(normalIndex != -1);

					_vertices->at(indexByPolygonVertex).normal.x = (float) fbxNormalElement->GetDirectArray().GetAt(normalIndex)[0];
					_vertices->at(indexByPolygonVertex).normal.y = (float) fbxNormalElement->GetDirectArray().GetAt(normalIndex)[2];
					_vertices->at(indexByPolygonVertex).normal.z = (float) fbxNormalElement->GetDirectArray().GetAt(normalIndex)[1];
				}
			}
			break;

		// There can be only one mapping coordinate for the whole polygon.
		case FbxGeometryElement::eByPolygon:
			break;

		// There will be one mapping coordinate for each unique edge in the mesh.
		// This is meant to be used with smoothing layer elements.
		case FbxGeometryElement::eByEdge:
			break;

		// There can be only one mapping coordinate for the whole surface.
		case FbxGeometryElement::eAllSame:
			break;
	}

	//// Process animation data for each joint while collecting joints.
	FbxTime::EMode timeMode = FbxTime::GetGlobalTimeMode();
	 
	// Get animation stack.
	FbxLongLong startFrame;
	FbxLongLong stopFrame;
	FbxAnimStack* fbxAnimStack = fbxScene->GetCurrentAnimationStack();
	if (fbxAnimStack != NULL) {
		const char* animStackName = fbxAnimStack->GetName();
		FbxTakeInfo* animStackTakeInfo = fbxScene->GetTakeInfo(animStackName);

		std::ostringstream myStream;
		myStream << fileName;
		myStream << ": Animation found: " << animStackName << "\n";
		OutputDebugStringA(myStream.str().c_str());
		std::cout << myStream.str().c_str();

		// Start and Stop points.
		FbxTime startTime = animStackTakeInfo->mLocalTimeSpan.GetStart();
		FbxTime stopTime = animStackTakeInfo->mLocalTimeSpan.GetStop();
		startFrame = startTime.GetFrameCount(timeMode);
		stopFrame = stopTime.GetFrameCount(timeMode);

		// Frame count and frame rate per-second.
		FbxLongLong frameCount = (int) stopFrame - startFrame;
		float frameRate = (float) FbxTime::GetFrameRate(timeMode);

		// Create animation objects.
		int fbxAnimLayerCount = fbxAnimStack->GetMemberCount();
		for (int animLayer = 0; animLayer < fbxAnimLayerCount; animLayer++) {
			Animation* animation = new Animation();
			animation->name = animStackName;
			animation->frameRate = frameRate;

			// Create keyframe for every frame.
			for (FbxLongLong frame = startFrame; frame < stopFrame; frame++) {
				Keyframe* keyframe = new Keyframe((float)(frame / frameRate));

				animation->gKeyFrames.push_back(keyframe);
			}

			animation->gKeyframeCount = (unsigned int) animation->gKeyFrames.size();
			animation->duration = animation->gKeyframeCount / animation->frameRate;

			_animations->push_back(animation);
		}
	}

	//// Process the deformer of the mesh. (Skeleton)
	if (mesh->GetDeformerCount() > 0) {
		// Check mesh deformer type if it is supported.
		if (mesh->GetDeformer(0)->GetDeformerType() == FbxDeformer::eSkin) {
			FbxSkin* skin = (FbxSkin*)mesh->GetDeformer(0, FbxDeformer::eSkin, &status);

			// Process each cluster of the deformer.
			int clusterCount = skin->GetClusterCount();
			for (int c = 0; c < clusterCount; c++) {
				FbxCluster* cluster = skin->GetCluster(c);

				// Get joint of the cluster.
				FbxNode* jointNode = cluster->GetLink();
				FbxCluster::ELinkMode linkMode = cluster->GetLinkMode();

				FbxAMatrix transformLinkMatrix;
				cluster->GetTransformLinkMatrix(transformLinkMatrix);

				// Create Joint object.
				Joint* joint = new Joint(c, jointNode->GetName());

				// Fill matrices of the joint object.
				// Current joint's matrix that transform model-space to joint-space.
				// transformLinkMatrix is joint's position in model space.
				joint->globalBindPoseInverseMatrix = *(FBX_Importer::MatrixFBXtoDX(
					transformLinkMatrix.Inverse()
				));

				// If joint node has parent, record it's pointer.
				FbxNode* parentJointNode = jointNode->GetParent();

				if (parentJointNode != NULL) {
					// If this value is NULL, then this joint don't have parent.
					Joint* parentJoint = FBX_Importer::getJointByName(parentJointNode->GetName(), _joints);

					if (parentJoint != NULL) {
						// If a joint has parent, it's child of it's parent.
						// Right? RIGHT??
						parentJoint->childJointIDs.push_back(joint->id);
						joint->parentJointID = parentJoint->id;

						// Calculate joint's bind pose transform relative to parent joint.
						FbxCluster* parentCluster = skin->GetCluster(parentJoint->id);
						FbxAMatrix parentTransformLinkMatrix;
						parentCluster->GetTransformLinkMatrix(parentTransformLinkMatrix);

						joint->jointLocalBindTransform = *(FBX_Importer::MatrixFBXtoDX(
							parentTransformLinkMatrix.Inverse() * transformLinkMatrix
						));
					}
					// If joint node has no parent, it's the root node.
					else {
						joint->isRoot = true;
					}
				}

				// Fill vertices' joint index and weight values.
				// Pair vertices' control point index with joint index-weight pair values.
				int* jointControlPointIndices = cluster->GetControlPointIndices();
				double* jointWeights = cluster->GetControlPointWeights();
				for (int v = 0; v < cluster->GetControlPointIndicesCount(); v++) {
					int controlPointIndex = jointControlPointIndices[v];
					_indexed_joint_weights[controlPointIndex][joint->id] = (float) jointWeights[v];
				}

				// Process transforms of the joint during animation.
				if (fbxAnimStack != NULL){
					FbxTime animTime;
					for (FbxLongLong frame = startFrame; frame < stopFrame; frame++) {
						animTime.SetFrame(frame, timeMode);
						
						// Get current joint's transformation in current time from the animation.
						_animations->at(0)->gKeyFrames.at((int)frame)->setJointKeyframeMatrix(
							joint->id,
							FBX_Importer::MatrixFBXtoDX(
								jointNode->EvaluateLocalTransform(animTime)
							)
						);
					}
				}

				// Push joint to vector array.
				_joints->push_back(joint);
			}
		}
		else {
			/*
			Other mesh deformer types are:
				FbxDeformer::eBlendShape:
				FbxDeformer::eUnknown:
				FbxDeformer::eVertexCache:
			*/

			std::ostringstream myStream;
			myStream << fileName;
			myStream << ": Mesh deformer found but deformer type is not supported." << "\n";
			OutputDebugStringA(myStream.str().c_str());
			std::cout << myStream.str().c_str();
		}
	}
	else {
		std::ostringstream myStream;
		myStream << fileName;
		myStream << ": There isn't any mesh deformer binded to mesh." << "\n";
		OutputDebugStringA(myStream.str().c_str());
		std::cout << myStream.str().c_str();
	}

	return true;
};

// Walks the node child-by-child and works recursively.
// Function simply prints out name and child count of the node.
void FBX_Importer::printNode(FbxNode* node, unsigned int level) {
	std::ostringstream myStream;

	if (level > 0) {
		myStream << '^' << std::string(level, '-');
	}
	myStream << node->GetName() << ", " << node->GetChildCount() << "\n";

	OutputDebugStringA(myStream.str().c_str());
	std::cout << myStream.str().c_str();

	for (int c=0; c < node->GetChildCount(); c++) {
		FBX_Importer::printNode(node->GetChild(c), level+1);
	}
}

// Walks all of the meshes in given node recursively, checks if polygon vertex count is greater than 3.
void FBX_Importer::Triangulate(FbxNode* node, FbxGeometryConverter& geometryConverter) {
	// Check if node is mesh.
	FbxMesh* mesh = node->GetMesh();
	if (mesh != NULL) {
		bool triangulate = false;

		for (int p = 0; p < mesh->GetPolygonCount(); p++) {
			if (mesh->GetPolygonSize(p) > 3) {
				triangulate = true;
				break;
			}
		}

		if (triangulate) {
			geometryConverter.Triangulate(
				mesh,
				true
			);
		}
	}

	for (int cn = 0; cn < node->GetChildCount(); cn++) {
		Triangulate(node->GetChild(cn), geometryConverter);
	}
}

FbxNode* FBX_Importer::getMainMeshNode(FbxNode* node, const char* mainMeshName) {
	// Check if node is mesh.
	FbxMesh* mesh = node->GetMesh();
	if (mesh != NULL) {
		if (std::string(mesh->GetName()).compare(mainMeshName) == 0) {
			return node;
		}
	}

	for (int cn = 0; cn < node->GetChildCount(); cn++) {
		FbxNode* childNode = getMainMeshNode(node->GetChild(cn), mainMeshName);
		if (childNode != NULL) {
			return childNode;
		}
	}
	return NULL;
}

Joint* FBX_Importer::getJointByName(const char* jointName, std::vector<Joint*>* _joints) {
	for (int j = 0; j < _joints->size(); j++) {
		if (std::string(jointName).compare(_joints->at(j)->name) == 0) {
			return _joints->at(j);
		}
	}
	return NULL;
}

// Converts FBX matrix to DirectX matrix.
dx::XMFLOAT4X4* FBX_Importer::MatrixFBXtoDX(FbxAMatrix fbxMatrix) {
	dx::XMFLOAT4X4* dxMATRIX = new dx::XMFLOAT4X4;

	// Decompose given matrix.
	FbxVector4 rotation = fbxMatrix.GetR();
	FbxVector4 translation = fbxMatrix.GetT();

	// Apply necessary axis conversions.
	rotation.Set(-rotation.mData[0], -rotation.mData[1], rotation.mData[2]);
	translation.Set(translation.mData[0], translation.mData[1], -translation.mData[2]);

	// Set them back.
	fbxMatrix.SetR(rotation);
	fbxMatrix.SetT(translation);

	// Copy FBX matrix do DX matrix.
	for (int i = 0; i<4; i++) {
		for (int j = 0; j < 4; j++) {
			(*dxMATRIX)(i, j) = (float) fbxMatrix.Get(i, j);
		}
	}

	return dxMATRIX;
}