#pragma once
#include "Entity/Mesh/MeshDeformer/Joint.h"
#include "Entity/Mesh/MeshDeformer/JointTransform.h"
#include "Entity/Mesh/MeshDeformer/Skeleton.h"

#include <string>
#include <DirectXMath.h>
#include "PxPhysicsAPI.h"

using namespace physx;
namespace dx = DirectX;

#define MAX_JOINT_COUNT 256
#define MAX_JOINT_PER_VERTEX 4

struct Vector2 {
	Vector2() {
		this->x = 0;
		this->y = 0;
	}

	Vector2(float x, float y) {
		this->x = x;
		this->y = y;
	}

	// Operators
	bool operator==(const Vector2& other) {
		return (this->x == other.x) && (this->y == other.y);
	}

	// Element wise calculations
	Vector2 operator+(const Vector2& other) {
		return Vector2(
			this->x + other.x,
			this->y + other.y
		);
	}

	Vector2 operator-(const Vector2& other) {
		return Vector2(
			this->x - other.x,
			this->y - other.y
		);
	}

	Vector2 operator/(const Vector2& other) {
		return Vector2(
			this->x / other.x,
			this->y / other.y
		);
	}

	Vector2 operator*(const Vector2& other) {
		return Vector2(
			this->x * other.x,
			this->y * other.y
		);
	}

	Vector2 operator*(const float& other) {
		return Vector2(
			this->x * other,
			this->y * other
		);
	}

	Vector2 operator/(const float& other) {
		return Vector2(
			this->x / other,
			this->y / other
		);
	}

	Vector2 normalize() {
		float _max = this->maximum();
		return Vector2(
			this->x / _max,
			this->y / _max
		);
	}

	float maximum() {
		float maxVal = -INFINITY;

		if (this->x > maxVal) {
			maxVal = this->x;
		}if (this->y > maxVal) {
			maxVal = this->y;
		}

		return maxVal;
	}

	float minimum() {
		float minVal = INFINITY;

		if (this->x < minVal) {
			minVal = this->x;
		}if (this->y < minVal) {
			minVal = this->y;
		}

		return minVal;
	}

	// DirectXMath & PhysX conversions
	void storeXMFLOAT(dx::XMFLOAT2* xmfloat) {
		this->x = xmfloat->x;
		this->y = xmfloat->y;
	}
	dx::XMFLOAT2 loadXMFLOAT() {
		return dx::XMFLOAT2(
			this->x,
			this->y
		);
	}
	void storeXMVECTOR(dx::XMVECTOR* xmvector) {
		dx::XMFLOAT2 xmfloat;

		dx::XMStoreFloat2(
			&xmfloat,
			*xmvector
		);

		this->storeXMFLOAT(&xmfloat);
	}
	dx::XMVECTOR loadXMVECTOR() {
		return dx::XMVectorSet(
			this->x,
			this->y,
			0,
			0
		);
	}
	PxVec2 toPxVec2() {
		return PxVec2(this->x, this->y);
	}

	float x;
	float y;
};

struct Vector3 {
	Vector3() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}
	
	Vector3(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	// Operators
	bool operator==(const Vector3& other) {
		return (this->x == other.x) && (this->y == other.y) && (this->z == other.z);
	}

	// Element wise calculations
	Vector3 operator+(const Vector3& other) {
		return Vector3(
			this->x + other.x,
			this->y + other.y,
			this->z + other.z
		);
	}

	Vector3 operator-(const Vector3& other) {
		return Vector3(
			this->x - other.x,
			this->y - other.y,
			this->z - other.z
		);
	}

	Vector3 operator/(const Vector3& other) {
		return Vector3(
			this->x / other.x,
			this->y / other.y,
			this->z / other.z
		);
	}

	Vector3 operator*(const Vector3& other) {
		return Vector3(
			this->x * other.x,
			this->y * other.y,
			this->z * other.z
		);
	}

	Vector3 operator*(const float& other) {
		return Vector3(
			this->x * other,
			this->y * other,
			this->z * other
		);
	}

	Vector3 operator/(const float& other) {
		return Vector3(
			this->x / other,
			this->y / other,
			this->z / other
		);
	}

	Vector3 normalize() {
		float _max = this->maximum();
		return Vector3(
			this->x / _max,
			this->y / _max,
			this->z / _max
		);
	}

	float maximum() {
		float maxVal = -INFINITY;

		if (this->x > maxVal) {
			maxVal = this->x;
		}if (this->y > maxVal) {
			maxVal = this->y;
		}if (this->z > maxVal) {
			maxVal = this->z;
		}

		return maxVal;
	}

	float minimum() {
		float minVal = INFINITY;

		if (this->x < minVal) {
			minVal = this->x;
		}if (this->y < minVal) {
			minVal = this->y;
		}if (this->z < minVal) {
			minVal = this->z;
		}

		return minVal;
	}

	// DirectXMath & PhysX conversions
	void storeXMFLOAT(dx::XMFLOAT3* xmfloat) {
		this->x = xmfloat->x;
		this->y = xmfloat->y;
		this->z = xmfloat->z;
	}
	dx::XMFLOAT3 loadXMFLOAT() {
		return dx::XMFLOAT3(
			this->x,
			this->y,
			this->z
		);
	}
	void storeXMVECTOR(dx::XMVECTOR* xmvector) {
		dx::XMFLOAT3 xmfloat;

		dx::XMStoreFloat3(
			&xmfloat,
			*xmvector
		);

		this->storeXMFLOAT(&xmfloat);
	}
	dx::XMVECTOR loadXMVECTOR() {
		return dx::XMVectorSet(
			this->x,
			this->y,
			this->z,
			0
		);
	}
	PxTransform toPxTransform() {
		return PxTransform(PxVec3(this->x, this->y, this->z));
	}
	PxVec3 toPxVec3() {
		return PxVec3(this->x, this->y, this->z);
	}

	float x;
	float y;
	float z;
};

struct Vector4 {
	Vector4() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->w = 0;
	}

	Vector4(float x, float y, float z, float w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	// Operators
	bool operator==(const Vector4& other) {
		return (this->x == other.x) && (this->y == other.y) && (this->z == other.z) && (this->w == other.w);
	}

	// Element wise calculations
	Vector4 operator+(const Vector4& other) {
		return Vector4(
			this->x + other.x,
			this->y + other.y,
			this->z + other.z,
			this->w + other.w
		);
	}

	Vector4 operator-(const Vector4& other) {
		return Vector4(
			this->x - other.x,
			this->y - other.y,
			this->z - other.z,
			this->w - other.w
		);
	}

	Vector4 operator/(const Vector4& other) {
		return Vector4(
			this->x / other.x,
			this->y / other.y,
			this->z / other.z,
			this->w / other.w
		);
	}

	Vector4 operator*(const Vector4& other) {
		return Vector4(
			this->x * other.x,
			this->y * other.y,
			this->z * other.z,
			this->w * other.w
		);
	}

	Vector4 operator*(const float& other) {
		return Vector4(
			this->x * other,
			this->y * other,
			this->z * other,
			this->w * other
		);
	}

	Vector4 operator/(const float& other) {
		return Vector4(
			this->x / other,
			this->y / other,
			this->z / other,
			this->w / other
		);
	}

	Vector4 normalize() {
		float _max = this->maximum();
		return Vector4(
			this->x / _max,
			this->y / _max,
			this->z / _max,
			this->w / _max
		);
	}

	float maximum() {
		float maxVal = -INFINITY;

		if (this->x > maxVal) {
			maxVal = this->x;
		}if (this->y > maxVal) {
			maxVal = this->y;
		}if (this->z > maxVal) {
			maxVal = this->z;
		}if (this->w > maxVal) {
			maxVal = this->w;
		}

		return maxVal;
	}

	float minimum() {
		float minVal = INFINITY;

		if (this->x < minVal) {
			minVal = this->x;
		}if (this->y < minVal) {
			minVal = this->y;
		}if (this->z < minVal) {
			minVal = this->z;
		}if (this->w < minVal) {
			minVal = this->w;
		}

		return minVal;
	}

	// DirectXMath & PhysX conversions
	void storeXMFLOAT(dx::XMFLOAT4* xmfloat) {
		this->x = xmfloat->x;
		this->y = xmfloat->y;
		this->z = xmfloat->z;
		this->w = xmfloat->w;
	}
	dx::XMFLOAT4 loadXMFLOAT() {
		return dx::XMFLOAT4(
			this->x,
			this->y,
			this->z,
			this->w
		);
	}
	void storeXMVECTOR(dx::XMVECTOR* xmvector) {
		dx::XMFLOAT4 xmfloat;

		dx::XMStoreFloat4(
			&xmfloat,
			*xmvector
		);

		this->storeXMFLOAT(&xmfloat);
	}
	dx::XMVECTOR loadXMVECTOR() {
		return dx::XMVectorSet(
			this->x,
			this->y,
			this->z,
			this->w
		);
	}
	PxVec4 toPxVec4() {
		return PxVec4(this->x, this->y, this->z, this->w);
	}
	PxQuat toPxQuat() {
		return PxQuat(this->x, this->y, this->z, this->w);
	}

	float x;
	float y;
	float z;
	float w;
};

struct Color {
	Color() {
		// Default: White.
		this->r = 1.0f;
		this->g = 1.0f;
		this->b = 1.0f;
		this->a = 1.0f;
	}

	Color(float r, float g, float b, float a=1.0f) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	// Operators
	bool operator==(const Color& other) {
		return (this->r == other.r) && (this->g == other.g) && (this->b == other.b) && (this->a == other.a);
	}

	// DirectXMath & PhysX conversions
	dx::XMFLOAT4 loadXMFLOAT() {
		return dx::XMFLOAT4(
			this->r,
			this->g,
			this->b,
			this->a
		);
	}

	float r;
	float g;
	float b;
	float a;
};

// Each vertex on every mesh is represented by this struct.
struct Vertex {
	Vector3 position;
	Vector2 textureUV;
	Vector3 normal;
	Vector3 tangent;
	Vector3 binormal;

	// Joint ID and corresponding weights for vertex
	float jointWeights[MAX_JOINT_PER_VERTEX];
	int jointIDs[MAX_JOINT_PER_VERTEX];
};

// Entity related structs.
struct EntityMaterial {
	Color color;
	float specularIntensity;
	float specularPower;
};

struct CollisionMaterial {
	CollisionMaterial(
		float staticFriction = 1.0f,
		float dynamicFriction = 1.0f,
		float restitution = 0.0f,
		float density = 10.0f
	) {
		this->staticFriction = staticFriction;
		this->dynamicFriction = dynamicFriction;
		this->restitution = restitution;
		this->density = density;
	}

	float staticFriction = 1.0f;
	float dynamicFriction = 1.0f;
	float restitution = 0.0f;
	float density = 10.0f;
};

// Used for creating entities.
struct EntityProperties {
	// Default properties of an entity.
	EntityProperties(
		Vector3 size = Vector3(1, 1, 1), Vector3 position = Vector3(0, 0, 0), Vector4 rotationQ = Vector4(1, 0, 0, 0),
		Color color = Color(1, 1, 1, 1), CollisionMaterial collisionMaterial=CollisionMaterial(),
		void* pMesh = NULL,
		void* pCollisionShape = NULL,
		void* pMollisionActor = NULL
	) {
		this->size = size;
		this->position = position;
		// Check quaternion vector.
		if (rotationQ == Vector4(0, 0, 0, 0)) {
			rotationQ.x = 1.0f;
		}
		this->rotationQ = rotationQ;
		this->color = color;
		this->pMesh = pMesh;
		this->pCollisionShape = pCollisionShape;
		this->pCollisionActor = pMollisionActor;
		this->collisionMaterial = collisionMaterial;
	}

	Vector3 size = Vector3(1,1,1);
	Vector3 position = Vector3(0,0,0);
	Vector4 rotationQ = Vector4(1,0,0,0);
	Color color = Color(1,1,1,1);
	CollisionMaterial collisionMaterial;
	void* pMesh = NULL;
	void* pCollisionShape = NULL;
	void* pCollisionActor = NULL;
};

// Used for creating characters.
struct CharacterProperties {
	// Default properties of an haracter.
	CharacterProperties(float movementSpeed = 0.1f, Vector3 facingDirection = {0, 0, 1}) {
		this->movementSpeed = movementSpeed;
		this->facingDirection = facingDirection;
	}

	float movementSpeed = 0.1f;
	Vector3 facingDirection = Vector3(0, 0, 1);
};

//// Graphics
// Entity constant buffer for Vertex Shader.
struct EntityVSConstantBuffer {
	// Transformation matrix for entity.
	dx::XMFLOAT4X4 worldMatrix;
	bool useMeshDeformer;
	dx::XMFLOAT3 padding;
};

// Entity constant buffer for Pixel Shader.
struct EntityPSConstantBuffer {
	dx::XMFLOAT3 color;
	bool useTexture;
	bool useNormalMapping;
	dx::XMFLOAT4 specularHighlight;
};

// Mesh Deformer constant buffer for Vertex Shader.
struct MeshDeformerVSConstantBuffer {
	dx::XMFLOAT4X4 jointsTransformMatrix[MAX_JOINT_COUNT];
};

// Each light object holds this, light's intensity, direction and position values.
struct LightPSConstantBuffer {
	dx::XMFLOAT4 color;
	float intensity;
	dx::XMFLOAT3 position;
	dx::XMFLOAT3 direction;
	unsigned int type;
	float halfSpotAngle;
	dx::XMFLOAT3 padding;
};

// Each camera object holds this, camera's position and projection information held.
struct CameraVSConstantBuffer {
	dx::XMFLOAT4X4 viewMatrix;
	dx::XMFLOAT4X4 projectionMatrix;
	dx::XMFLOAT3 cameraPosition;
	float padding;
};