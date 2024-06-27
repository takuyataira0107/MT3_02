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


//======================================  ベクトルの減算  =========================================
Vector3 SubtractVector(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result = { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
	return result;
}
//=================================================================================================


//======================================  ベクトルの加算  =========================================
Vector3 AddVector(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result = { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	return result;
}
//=================================================================================================


//=========================================  グリッド  ============================================
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


//=======================================  スフィア生成  ==========================================
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


//======================================  正射影ベクトル  =========================================
Vector3 Project(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	float t = (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z) / (std::sqrtf(v2.x * v2.x + v2.y * v2.y + v2.z * v2.z) * std::sqrtf(v2.x * v2.x + v2.y * v2.y + v2.z * v2.z));
	result = { v2.x * t, v2.y * t, v2.z * t };
	return result;
}
//=================================================================================================


//=========================================  最近接点  ============================================
Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 result;
	Vector3 tb = Project(SubtractVector(point, segment.origin), segment.diff);
	result = { segment.origin.x + tb.x, segment.origin.y + tb.y, segment.origin.z + tb.z };
	return result;
}
//=================================================================================================
