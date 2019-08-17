#include "TriangleMesh.h"

// Utils
#include "utils/OBJ_Loader.h"
objl::Loader Loader;

TriangleMesh::TriangleMesh(PxVec3 size, PxVec3 position, PxVec3 rotation, PxVec3 material){
	PxQuat rotationQuaternion;

	// Graphics
	this->gSize = XMFLOAT3(size.x, size.y, size.z);
	this->gPosition = XMFLOAT3(position.x, position.y, position.z);
	this->gRotationQ = XMFLOAT4(rotationQuaternion.x, rotationQuaternion.y, rotationQuaternion.z, rotationQuaternion.w);

	// Set properties of the entity.
	this->type = ENTITY_TYPE::TRIANGLE_MESH;
	this->isDynamic = false;

	this->Update(true);
}

TriangleMesh::~TriangleMesh() {}

void TriangleMesh::Update(bool initial) {
	// Skip static and sleeping dynamic entities.
	this->shouldUpdateData = true;

	if (!initial) {
		// Static check.
		if (!this->isDynamic) {
			this->shouldUpdateData = false;
			return;
		}
	}

	this->updateConstantBuffer();
}

void TriangleMesh::updateConstantBuffer() {
	// Update constant buffer that held on class.
	dx::XMStoreFloat4x4(
		&(this->gConstBuffer.worldMatrix),
		dx::XMMatrixTranspose(
			dx::XMMatrixScaling(this->gSize.x, this->gSize.y, this->gSize.z) *
			dx::XMMatrixRotationQuaternion(dx::XMLoadFloat4(&this->gRotationQ)) *
			dx::XMMatrixTranslation(this->gPosition.x, this->gPosition.y, this->gPosition.z)
		)
	);
}

bool TriangleMesh::LoadVerticesAndIndicesOBJ(const char* fileName){
	// Load OBJ file
	bool loadResult = Loader.LoadFile(fileName);

	if (loadResult){
		objl::Mesh mesh = Loader.LoadedMeshes[0];

		// Process loaded mesh.
		this->gVertexCount = (UINT) mesh.Vertices.size();
		this->gIndexCount = (UINT) mesh.Indices.size();

		Vertex* vertices = new Vertex[this->gVertexCount];
		unsigned int* indices = new unsigned int[this->gIndexCount];

		// Load vertices.
		for (int v = 0; v < mesh.Vertices.size(); v++) {
			vertices[v] = Vertex(
				{
					// XMFLOAT3 position
					{
						mesh.Vertices.at(v).Position.X,
						mesh.Vertices.at(v).Position.Y,
						mesh.Vertices.at(v).Position.Z,
					}
				}
			);
		}

		// Load indices.
		for (int i = 0; i < mesh.Indices.size(); i+=3) {
			/*indices[i] = mesh.Indices.at(i);
			indices[i+1] = mesh.Indices.at(i+1);
			indices[i+2] = mesh.Indices.at(i+2);*/

			indices[i] = mesh.Indices.at(i + 2);
			indices[i + 1] = mesh.Indices.at(i + 1);
			indices[i + 2] = mesh.Indices.at(i);
		}

		// Copy filled array to class' property.
		this->gVertices = vertices;
		this->gIndices = indices;
	}

	return loadResult;
}