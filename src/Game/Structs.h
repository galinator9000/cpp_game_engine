#pragma once
#include <string>
#include <DirectXMath.h>

namespace dx = DirectX;

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

	Color(float r, float g, float b, float a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	float r;
	float g;
	float b;
	float a;
};

// Each vertex on every mesh is represented by this struct.
struct Vertex {
	Vector3 position;
	Vector3 normal;
	Vector2 TextureUV;
};

// Each joint on mesh deformer is represented by this struct.
struct Joint {
	unsigned int id;
	std::string name;
	Joint* parent;
};

struct EntityMaterial {
	Color color;
	float specularIntensity;
	float specularPower;
};

//// Graphics
// Entity constant buffer for Vertex Shader.
struct EntityVSConstantBuffer {
	// Transformation matrix for entity.
	dx::XMFLOAT4X4 worldMatrix;
};

// Entity constant buffer for Pixel Shader.
struct EntityPSConstantBuffer {
	dx::XMFLOAT3 color;
	bool useTexture;
	dx::XMFLOAT4 specularHighlight;
};

// Each light object holds this, light's intensity, direction and position values.
struct LightPSConstantBuffer {
	float intensity;
	dx::XMFLOAT3 direction;
	dx::XMFLOAT3 position;
	unsigned int type;
};

// Each camera object holds this, camera's position and projection information held.
struct CameraVSConstantBuffer {
	dx::XMFLOAT4X4 viewMatrix;
	dx::XMFLOAT4X4 projectionMatrix;
	dx::XMFLOAT4 cameraPosition;
};
