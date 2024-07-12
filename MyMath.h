#pragma once
#include "MakeMatrix.h"
#include <Novice.h>
#include <numbers>


struct Sphere {
	Vector3 center;  //!< 中心点
	float radius;    //!< 半径
};

struct Line {
	Vector3 origin;  //!< 始点
	Vector3 diff;    //!< 終点への差分ベクトル
};

struct Ray {
	Vector3 origin;  //!< 始点
	Vector3 diff;    //!< 終点への差分ベクトル
};

struct Segment {
	Vector3 origin;  //!< 始点
	Vector3 diff;    //!< 終点への差分ベクトル
};

struct Plane {
	Vector3 normal;  //!< 法線 
	float distance;  //!< 距離
};

struct Triangle {
	Vector3 vertices[3]; //!< 頂点
	Vector3 normal;
};

struct AABB {
	Vector3 min;
	Vector3 max;
};

//===========================================  表示  ==============================================

static const int kRowHeight = 20;
static const int kColumnWidth = 60;
void VectorScreenPrintf(int x, int y, const Vector3& vector) {
	Novice::ScreenPrintf(x, y, "%.03f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.03f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.03f", vector.z);
}

void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix) {
	for (int row = 0; row < 4; ++row) {
		for (int column = 0; column < 4; ++column) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + row * kRowHeight, "%6.02f", matrix.m[row][column]);
		}
	}
}

//=================================================================================================


//======================================  ベクトルの加算  =========================================
Vector3 AddVector(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result = { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	return result;
}
//=================================================================================================

//=======================================  ベクトルの減算  ==========================================
Vector3 SubtractVector(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result = { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
	return result;
}
//=================================================================================================

//=================================================================================================
Vector3 MultiplyVector(const float& k, const Vector3& v) {
	Vector3 result;
	result = { k * v.x, k * v.y, k * v.z };
	return result;
}
//=================================================================================================


//===========================================  正規化  =============================================
Vector3 Normalize(const Vector3& vector) {
	Vector3 result;
	float length = std::sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	float mag = 1 / length;
	result = { vector.x * mag, vector.y * mag, vector.z * mag };
	return result;
}
//=================================================================================================

//===========================================  内積  =============================================
float Dot(const Vector3& v1, const Vector3& v2) {
	float result;
	result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return result;
}

float DotFloat(const Vector3& vector, const float& a) {
	float result;
	result = vector.x * a + vector.y * a + vector.z * a;
	return result;
}
//=================================================================================================

//===========================================  距離  =============================================
/*float Length(const Vector3& v1, const Vector3 v2) {
	float result;
	result = sqrtf(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2) + pow(v2.z - v1.z, 2));
	return result;
}*/

//=================================================================================================


//======================================  正射影ベクトル  =========================================
Vector3 Project(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	float t = (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z) / (std::sqrtf(v2.x * v2.x + v2.y * v2.y + v2.z * v2.z) * std::sqrtf(v2.x * v2.x + v2.y * v2.y + v2.z * v2.z));
	result = { v2.x * t, v2.y * t, v2.z * t };
	return result;
}
//=================================================================================================

//========================================  垂直なベクトル  =========================================
Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return { -vector.y, vector.x, 0.0f };
	}
	return { 0.0f, -vector.z, vector.y };
}
//=================================================================================================

//=========================================  最近接点  =============================================
Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 result;
	Vector3 project = Project(SubtractVector(point, segment.origin), segment.diff);
	result = { segment.origin.x + project.x, segment.origin.y + project.y, segment.origin.z + project.z };
	return result;
}
//=================================================================================================


//======================================  球同士の衝突判定  ==========================================
bool IsCollisionSphere(const Sphere& s1, const Sphere& s2) {
	float x = (s2.center.x - s1.center.x) * (s2.center.x - s1.center.x);
	float y = (s2.center.y - s1.center.y) * (s2.center.y - s1.center.y);
	float z = (s2.center.z - s1.center.z) * (s2.center.z - s1.center.z);

	if (s1.radius + s2.radius >= sqrtf(x + y + z)) {
		return true;
	}
	else {
		return false;
	}
}
//=================================================================================================

//=====================================  球と平面の衝突判定  =========================================
bool IsCollisionPlane(const Sphere& sphere, const Plane& plane) {
	float d = DotFloat(plane.normal, plane.distance);
	float k = fabs(Dot(plane.normal, sphere.center) - d);

	if (k <= sphere.radius) {
		return true;
	}
	else {
		return false;
	}
}
//=================================================================================================

//=====================================  線と平面の衝突判定  =========================================
bool IsCollisionSegment(const Segment& segment, const Plane& plane) {
	// まず垂直判定を行うために、法線と線の内積を求める
	float dot = Dot(plane.normal, segment.diff);

	// 垂直=平行であるので、衝突しているはずがない
	if (dot == 0.0f) {
		return false;
	}

	// tを求める
	float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;

	// tの値と線の種類によって衝突しているかを判断する
	if (t >= 0 && t <= 1) {
		return true;
	}
	else {
		return false;
	}
}
//=================================================================================================

//====================================  線と三角形の衝突判定  ========================================

bool IsCollisionTriangle(const Triangle& triangle, const Segment& segment) {


	Vector3 v01 = { triangle.vertices[1].x - triangle.vertices[0].x, triangle.vertices[1].y - triangle.vertices[0].y, triangle.vertices[1].z - triangle.vertices[0].z };
	Vector3 v12 = { triangle.vertices[2].x - triangle.vertices[1].x, triangle.vertices[2].y - triangle.vertices[1].y, triangle.vertices[2].z - triangle.vertices[1].z };
	Vector3 v20 = { triangle.vertices[0].x - triangle.vertices[2].x, triangle.vertices[0].y - triangle.vertices[2].y, triangle.vertices[0].z - triangle.vertices[2].z };


	Vector3 v0p = { (segment.origin.x + segment.diff.x) - triangle.vertices[0].x, (segment.origin.y + segment.diff.y) - triangle.vertices[0].y, (segment.origin.z + segment.diff.z) - triangle.vertices[0].z };
	Vector3 v1p = { (segment.origin.x + segment.diff.x) - triangle.vertices[1].x, (segment.origin.y + segment.diff.y) - triangle.vertices[1].y, (segment.origin.z + segment.diff.z) - triangle.vertices[1].z };
	Vector3 v2p = { (segment.origin.x + segment.diff.x) - triangle.vertices[2].x, (segment.origin.y + segment.diff.y) - triangle.vertices[2].y, (segment.origin.z + segment.diff.z) - triangle.vertices[2].z };

	// 各辺を結んだベクトルと、頂点と衝突点pを結んだベクトルのクロス積を取る
	Vector3 cross01 = Cross(v01, v1p);
	Vector3 cross12 = Cross(v12, v2p);
	Vector3 cross20 = Cross(v20, v0p);

	if (Dot(cross01, triangle.normal) >= 0.0f &&
		Dot(cross12, triangle.normal) >= 0.0f &&
		Dot(cross20, triangle.normal) >= 0.0f) {
		return true;
	}
	else {
		return false;
	}

}

//=================================================================================================

//======================================  AABBの衝突判定  ==========================================
bool isCollisionAABB(const AABB& a, const AABB& b) {
	if ((a.min.x <= b.max.x && a.max.x >= b.min.x) && // x軸
		(a.min.y <= b.max.y && a.max.y >= b.min.y) && // y軸
		(a.min.z <= b.max.z && a.max.z >= b.min.z)) { // z軸
		return true;
	}

	return false;
}
//=================================================================================================


//=========================================  グリッド  =============================================
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix)
{
	const float kGridHalfWidth = 2.0f;                                       // Gridの半分の幅
	const uint32_t kSubdivision = 10;                                        // 分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);  // 1つ分の長さ

	// 奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHalfWidth + (xIndex * kGridEvery);
		unsigned int color = 0xAAAAAAFF;

		Vector3 start{ x, 0.0f, -kGridHalfWidth };
		Vector3 end{ x, 0.0f, kGridHalfWidth };

		Vector3 startScreen = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 endScreen = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		if (x == 0.0f)
		{
			color = BLACK;
		}

		Novice::DrawLine(int(startScreen.x), int(startScreen.y), int(endScreen.x), int(endScreen.y), color);
	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfWidth + (zIndex * kGridEvery);
		unsigned int color = 0xAAAAAAFF;

		Vector3 start{ -kGridHalfWidth , 0.0f, z };
		Vector3 end{ kGridHalfWidth , 0.0f, z };

		Vector3 startScreen = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 endScreen = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);

		if (z == 0.0f)
		{
			color = BLACK;
		}

		Novice::DrawLine(int(startScreen.x), int(startScreen.y), int(endScreen.x), int(endScreen.y), color);
	}
}
//=================================================================================================

//=======================================  スフィア描画  ==========================================
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 12;
	const float kLonEvery = 2 * std::numbers::pi_v<float> / kSubdivision;  // 経度
	const float kLatEvery = std::numbers::pi_v<float> / kSubdivision;      // 緯度
	// 緯度の方向に分割 -π/2 ～ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -std::numbers::pi_v<float> / 2.0f + kLatEvery * latIndex;  // 現在の緯度
		// 経度の方向に分割 0 ～ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;  // 現在の経度
			// world座標系でのa,b,cを求める
			Vector3 a, b, c;
			a = {
				sphere.radius * (std::cos(lat) * std::cos(lon)) + sphere.center.x,
				sphere.radius * std::sin(lat) + sphere.center.y,
				sphere.radius * (std::cos(lat) * std::sin(lon)) + sphere.center.z
			};

			b = {
				sphere.radius * (std::cos(lat + kLatEvery) * std::cos(lon)) + sphere.center.x,
				sphere.radius * std::sin(lat + kLatEvery) + sphere.center.y,
				sphere.radius * (std::cos(lat + kLatEvery) * std::sin(lon)) + sphere.center.z
			};

			c = {
				sphere.radius * (std::cos(lat) * std::cos(lon + kLonEvery)) + sphere.center.x,
				sphere.radius * std::sin(lat) + sphere.center.y,
				sphere.radius * (std::cos(lat) * std::sin(lon + kLonEvery)) + sphere.center.z
			};

			// a,b,cをScreen座標系まで変換...
			Vector3 aScreen = Transform(Transform(a, viewProjectionMatrix), viewportMatrix);
			Vector3 bScreen = Transform(Transform(b, viewProjectionMatrix), viewportMatrix);
			Vector3 cScreen = Transform(Transform(c, viewProjectionMatrix), viewportMatrix);

			// ab,bcで線を引く
			Novice::DrawLine(int(aScreen.x), int(aScreen.y), int(bScreen.x), int(bScreen.y), color);
			Novice::DrawLine(int(aScreen.x), int(aScreen.y), int(cScreen.x), int(cScreen.y), color);
		}
	}
}
//=================================================================================================

//***
//========================================  線分の描画  ============================================
void DrawLineSegment(const Segment& segment, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, int32_t color) {
	Vector3 start = Transform(Transform(segment.origin, viewProjectionMatrix), viewportMatrix);
	Vector3 end = Transform(Transform(AddVector(segment.origin, segment.diff), viewProjectionMatrix), viewportMatrix);

	Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), color);
}
//=================================================================================================

//========================================  平面の描画  ============================================
void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 center = MultiplyVector(plane.distance, plane.normal);  // 1
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Perpendicular(plane.normal));  // 2
	perpendiculars[1] = { -perpendiculars[0].x, -perpendiculars[0].y, -perpendiculars[0].z };  // 3
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]);  // 4
	perpendiculars[3] = { -perpendiculars[2].x, -perpendiculars[2].y, -perpendiculars[2].z };  // 5
	// 6
	Vector3 points[4];
	for (uint32_t index = 0; index < 4; ++index) {
		Vector3 extend = MultiplyVector(2.0f, perpendiculars[index]);
		Vector3 point = AddVector(center, extend);
		points[index] = Transform(Transform(point, viewProjectionMatrix), viewportMatrix);
	}

	Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[3].x), int(points[3].y), color);
	Novice::DrawLine(int(points[2].x), int(points[2].y), int(points[1].x), int(points[1].y), color);
	Novice::DrawLine(int(points[3].x), int(points[3].y), int(points[1].x), int(points[1].y), color);
}
//=================================================================================================

//=======================================  三角形の描画  ============================================

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 screenVertices[3];
	for (uint32_t index = 0; index < 3; ++index) {
		screenVertices[index] = Transform(Transform(triangle.vertices[index], viewProjectionMatrix), viewportMatrix);
	}
	Novice::DrawTriangle(
		int(screenVertices[0].x), int(screenVertices[0].y),
		int(screenVertices[1].x), int(screenVertices[1].y),
		int(screenVertices[2].x), int(screenVertices[2].y),
		color,
		kFillModeWireFrame
	);
}

//=================================================================================================

//========================================  aabbの描画  =============================================
void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 square1[4];
	square1[0] = { aabb.min.x, aabb.min.y, aabb.min.z };
	square1[1] = { aabb.min.x, aabb.min.y, aabb.max.z };
	square1[2] = { aabb.max.x, aabb.min.y, aabb.max.z };
	square1[3] = { aabb.max.x, aabb.min.y, aabb.min.z };
	Vector3 square2[4];
	square2[0] = { aabb.min.x, aabb.max.y, aabb.min.z };
	square2[1] = { aabb.min.x, aabb.max.y, aabb.max.z };
	square2[2] = { aabb.max.x, aabb.max.y, aabb.max.z };
	square2[3] = { aabb.max.x, aabb.max.y, aabb.min.z };
	
	Vector3 screenSquare1[4];
	Vector3 screenSquare2[4];
	for (uint32_t index = 0; index < 4; ++index) {
		screenSquare1[index] = Transform(Transform(square1[index], viewProjectionMatrix), viewportMatrix);
		screenSquare2[index] = Transform(Transform(square2[index], viewProjectionMatrix), viewportMatrix);
	}

	// 描画
	for (uint32_t index = 0; index < 4; ++index) {
		Novice::DrawLine(int(screenSquare1[index].x), int(screenSquare1[index].y), int(screenSquare2[index].x), int(screenSquare2[index].y), color);
	}
	Novice::DrawLine(int(screenSquare1[0].x), int(screenSquare1[0].y), int(screenSquare1[1].x), int(screenSquare1[1].y), color);
	Novice::DrawLine(int(screenSquare2[0].x), int(screenSquare2[0].y), int(screenSquare2[1].x), int(screenSquare2[1].y), color);
	Novice::DrawLine(int(screenSquare1[0].x), int(screenSquare1[0].y), int(screenSquare1[3].x), int(screenSquare1[3].y), color);
	Novice::DrawLine(int(screenSquare2[0].x), int(screenSquare2[0].y), int(screenSquare2[3].x), int(screenSquare2[3].y), color);
	Novice::DrawLine(int(screenSquare1[2].x), int(screenSquare1[2].y), int(screenSquare1[3].x), int(screenSquare1[3].y), color);
	Novice::DrawLine(int(screenSquare2[2].x), int(screenSquare2[2].y), int(screenSquare2[3].x), int(screenSquare2[3].y), color);
	Novice::DrawLine(int(screenSquare1[1].x), int(screenSquare1[1].y), int(screenSquare1[2].x), int(screenSquare1[2].y), color);
	Novice::DrawLine(int(screenSquare2[1].x), int(screenSquare2[1].y), int(screenSquare2[2].x), int(screenSquare2[2].y), color);
}
//=================================================================================================