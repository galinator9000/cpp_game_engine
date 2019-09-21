#include "FBX_Importer.h"

bool FBX_Importer::Load(
		const char* fileName, const char* mainMeshName,
		Vertex*& _vertices, unsigned int*& _indices,
		unsigned int& _vertices_count, unsigned int& _indices_count,
		Joint**& _joints, unsigned int& _jointCount,
		Animation**& _animations, unsigned int& _animationCount,
		std::map<int, int>& _indexed_vertices,
		std::map<int, std::map<int, float>>& _indexed_joint_weights
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
	/*FbxAxisSystem fbxAxisSystem_DirectX = FbxAxisSystem::eDirectX;
	FbxAxisSystem fbxAxisSystem_Maya = FbxAxisSystem::eMayaYUp;
	fbxAxisSystem_DirectX.ConvertChildren(
		rootNode,
		fbxAxisSystem_Maya
	);*/

	// Apply global scaling to nodes.
	const FbxSystemUnit::ConversionOptions fbxConversionOptions = {false,true,true,true,true,true};
	FbxSystemUnit fbxTargetSystemUnit(1, 1);
	fbxTargetSystemUnit.ConvertScene(fbxScene, fbxConversionOptions);
	double systemUnit_scaleFactor = fbxTargetSystemUnit.GetScaleFactor();
	
	//// Process mesh.
	// Get mesh from scene object.
	FbxNode* meshNode = getMainMeshNode(rootNode, mainMeshName);
	FbxMesh* mesh;
	if (meshNode == NULL) {
		std::ostringstream myStream;
		myStream << fileName;
		myStream << ": There isn't a mesh node in current scene with given name." << "\n";
		OutputDebugStringA(myStream.str().c_str());
		std::cout << myStream.str().c_str();
		return false;
	}
	mesh = meshNode->GetMesh();

	FbxDouble3 meshNodeTranslation = meshNode->LclTranslation;
	FbxDouble3 meshNodeRotation = meshNode->LclRotation;
	FbxDouble3 meshNodeScaling = meshNode->LclScaling;

	//// Read mesh elements.
	// Normal
	FbxGeometryElementNormal* fbxNormalElement = mesh->GetElementNormal();
	FbxLayerElement::EMappingMode fbxNormalMapMode = fbxNormalElement->GetMappingMode();
	FbxLayerElement::EReferenceMode fbxNormalReferenceMode = fbxNormalElement->GetReferenceMode();
	// UV
	FbxGeometryElementUV* fbxUVElement = mesh->GetElementUV();
	FbxLayerElement::EMappingMode fbxUVMapMode = fbxUVElement->GetMappingMode();
	FbxLayerElement::EReferenceMode fbxUVReferenceMode = fbxUVElement->GetReferenceMode();
	FbxStringList uvNames;
	mesh->GetUVSetNames(uvNames);
	const char* uvName = uvNames[0];

	// Get vertices (control points).
	FbxVector4* fbxVertices = mesh->GetControlPoints();
	int controlPointsCount = mesh->GetControlPointsCount();
	int polygonCount = mesh->GetPolygonCount();

	int vertexArrayIndex = 0;
	int controlPointIndex = 0;

	// Create arrays.
	_vertices_count = polygonCount * 3;
	_indices_count = polygonCount * 3;
	_vertices = new Vertex[_vertices_count];
	_indices = new unsigned int[_indices_count];

	// Collect all vertices.
	for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++) {
		// Process every vertex in this polygon (triangle)
		for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {

			// Get the control point index of the current vertex.
			controlPointIndex = mesh->GetPolygonVertex(polygonIndex, vertexIndex);
			vertexArrayIndex = polygonIndex * 3 + vertexIndex;

			Vertex* vertex = new Vertex{
				// Position of the Vertex
				{
					(float) (fbxVertices[controlPointIndex].mData[0] * systemUnit_scaleFactor),
					(float) (fbxVertices[controlPointIndex].mData[1] * systemUnit_scaleFactor),
					(float) (fbxVertices[controlPointIndex].mData[2] * -systemUnit_scaleFactor)
				}
			};

			// Fill joint indices and weights initially.
			for (int j = 0; j < MAX_JOINT_PER_VERTEX; j++) {
				vertex->jointIDs[j] = -1;
				vertex->jointWeights[j] = 0.0f;
			}

			_indexed_vertices[vertexArrayIndex] = controlPointIndex;
			_vertices[vertexArrayIndex] = *vertex;
		}
	}

	// Collect all indices.
	for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++) {
		// Process every vertex in this polygon (triangle)
		for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
			vertexArrayIndex = polygonIndex * 3 + vertexIndex;

			_indices[vertexArrayIndex] = (
				// Reverse winding. (RH to LH)
				(unsigned int) polygonIndex * 3 + (2 - vertexIndex)
				//(unsigned int) polygonIndex * 3 + (vertexIndex)
			);
		}
	}

	// Collect normal layer data.
	if (fbxNormalMapMode == FbxGeometryElement::eByPolygonVertex) {
		for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++) {
			// Process every vertex in this polygon (triangle)
			for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
				vertexArrayIndex = polygonIndex * 3 + vertexIndex;

				int normalIndex = -1;
				if (fbxNormalReferenceMode == FbxGeometryElement::eDirect) {
					normalIndex = vertexArrayIndex;
				}
				else if(fbxNormalReferenceMode == FbxGeometryElement::eIndexToDirect){
					normalIndex = fbxNormalElement->GetIndexArray().GetAt(vertexArrayIndex);
				}
				assert(normalIndex != -1);

				_vertices[vertexArrayIndex].normal.x = (float) fbxNormalElement->GetDirectArray().GetAt(normalIndex)[0];
				_vertices[vertexArrayIndex].normal.y = (float) fbxNormalElement->GetDirectArray().GetAt(normalIndex)[1];
				_vertices[vertexArrayIndex].normal.z = (float) -fbxNormalElement->GetDirectArray().GetAt(normalIndex)[2];
			}
		}
	}
	else if (fbxNormalMapMode == FbxGeometryElement::eByControlPoint) {
		for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++) {
			for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
				vertexArrayIndex = polygonIndex * 3 + vertexIndex;
				controlPointIndex = mesh->GetPolygonVertex(polygonIndex, vertexIndex);

				int normalIndex = -1;
				if (fbxNormalReferenceMode == FbxGeometryElement::eDirect) {
					normalIndex = controlPointIndex;
				}
				else if (fbxNormalReferenceMode == FbxGeometryElement::eIndexToDirect) {
					normalIndex = fbxNormalElement->GetIndexArray().GetAt(controlPointIndex);
				}
				assert(normalIndex != -1);

				_vertices[vertexArrayIndex].normal.x = (float) fbxNormalElement->GetDirectArray().GetAt(normalIndex)[0];
				_vertices[vertexArrayIndex].normal.y = (float) fbxNormalElement->GetDirectArray().GetAt(normalIndex)[1];
				_vertices[vertexArrayIndex].normal.z = (float) -fbxNormalElement->GetDirectArray().GetAt(normalIndex)[2];
			}
		}
	}

	// Collect UV layer data.
	for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++) {
		for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
			vertexArrayIndex = polygonIndex * 3 + vertexIndex;

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

			_vertices[vertexArrayIndex].TextureUV.x = texture_U;
			_vertices[vertexArrayIndex].TextureUV.y = texture_V;
		}
	}
	
	//// Process animation data for each joint while collecting joints.
	FbxTime::EMode timeMode = FbxTime::GetGlobalTimeMode();
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

		_animationCount = fbxAnimLayerCount;
		_animations = new Animation*[_animationCount];

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

			_animations[animLayer] = animation;
		}
	}

	//// Process the deformer of the mesh. (Skeleton)
	if (mesh->GetDeformerCount() > 0) {
		// Check mesh deformer type if it is supported.
		if (mesh->GetDeformer(0)->GetDeformerType() == FbxDeformer::eSkin) {
			FbxSkin* skin = (FbxSkin*) mesh->GetDeformer(0, FbxDeformer::eSkin, &status);
			int clusterCount = skin->GetClusterCount();

			// Create array of joints.
			_jointCount = clusterCount;
			_joints = new Joint*[_jointCount];
			memset(
				_joints,
				NULL,
				sizeof(Joint*) * _jointCount
			);

			// Process each cluster of the deformer.
			for (int c = 0; c < clusterCount; c++) {
				FbxCluster* cluster = skin->GetCluster(c);

				// Get joint of the cluster.
				FbxNode* jointNode = cluster->GetLink();
				FbxCluster::ELinkMode linkMode = cluster->GetLinkMode();

				// Get matrices of cluster and mesh node.
				FbxAMatrix transformMatrix;
				cluster->GetTransformMatrix(transformMatrix);
				FbxAMatrix transformLinkMatrix;
				cluster->GetTransformLinkMatrix(transformLinkMatrix);
				FbxAMatrix geometryTransformMatrix = FbxAMatrix(
					meshNode->GetGeometricTranslation(FbxNode::eSourcePivot),
					meshNode->GetGeometricRotation(FbxNode::eSourcePivot),
					meshNode->GetGeometricScaling(FbxNode::eSourcePivot)
				);

				// Create Joint object.
				Joint* joint = new Joint(c, jointNode->GetName());

				// Fill matrices of the joint object.
				// Current joint's matrix that transform model-space to joint-space.
				// transformLinkMatrix is joint's position in model space.

				// Transforms in global-space.
				/*
				joint->globalBindPoseInverseMatrix = *(FBX_Importer::MatrixFBXtoDX(
					transformLinkMatrix.Inverse() * transformMatrix * geometryTransformMatrix
				));
				joint->globalBindPoseMatrix = *(FBX_Importer::MatrixFBXtoDX(
					transformLinkMatrix * transformMatrix * geometryTransformMatrix
				));
				*/

				// Transforms relative to parent.
				joint->globalBindPoseInverseMatrix = *(FBX_Importer::MatrixFBXtoDX(
					transformLinkMatrix.Inverse()
				));
				joint->globalBindPoseMatrix = *(FBX_Importer::MatrixFBXtoDX(
					transformLinkMatrix
				));

				// If joint node has parent, record it's pointer.
				FbxNode* parentJointNode = jointNode->GetParent();
				FbxAMatrix parentTransformLinkMatrix;
				parentTransformLinkMatrix.SetIdentity();
				if (parentJointNode != NULL) {
					// If this value is NULL, then this joint don't have parent.
					Joint* parentJoint = FBX_Importer::getJointByName(parentJointNode->GetName(), _joints, _jointCount);

					if (parentJoint != NULL) {
						// If a joint has parent, it's child of it's parent.
						// Right? RIGHT??
						parentJoint->childJointIDs.push_back(joint->id);
						joint->parentJointID = parentJoint->id;

						// Bindpose transform of the current joint relative to parent joint space.
						FbxCluster* parentCluster = skin->GetCluster(parentJoint->id);

						parentCluster->GetTransformLinkMatrix(parentTransformLinkMatrix);
						joint->jointLocalBindTransform = *(FBX_Importer::MatrixFBXtoDX(
							transformLinkMatrix * parentTransformLinkMatrix.Inverse()
						));
					}
					// If joint node has no parent, it's the root node.
					else {
						joint->isRoot = true;

						joint->jointLocalBindTransform = *(FBX_Importer::MatrixFBXtoDX(
							parentTransformLinkMatrix
						));
					}
				}

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
						FbxAMatrix meshCurrentTransformOffset = meshNode->EvaluateGlobalTransform(animTime) * geometryTransformMatrix;
						_animations[0]->gKeyFrames.at((int) frame)->setJointKeyframeMatrix(
							joint->id,
							FBX_Importer::MatrixFBXtoDX(
								// Transforms in global-space.
								//meshCurrentTransformOffset.Inverse() * jointNode->EvaluateGlobalTransform(animTime)

								// Transforms relative to parent.
								jointNode->EvaluateLocalTransform(animTime)
							)
						);
					}
				}

				// Push joint to vector array.
				_joints[c] = joint;
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
		std::string meshName = std::string(mesh->GetName());
		if (meshName.compare(mainMeshName) == 0) {
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

Joint* FBX_Importer::getJointByName(const char* jointName, Joint** joints, unsigned int jointCount) {
	for (unsigned int j = 0; j < jointCount; j++) {
		if (joints[j] == NULL) {
			continue;
		}
		if (std::string(jointName).compare(joints[j]->name) == 0) {
			return joints[j];
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
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			(*dxMATRIX)(i, j) = (float) fbxMatrix.Get(i, j);
		}
	}

	return dxMATRIX;
}