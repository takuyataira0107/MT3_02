#include "MyMath.h"
#include <imgui.h>

const char kWindowTitle[] = "LC1C_19_タイラタクヤ_タイトル";



// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// カメラの座標
	Vector3 cameraRotate = { 0.26f, 0.0f, 0.0f };
	Vector3 cameraTranslate = { 0.0f, 1.9f, -6.49f };
	const int kWindowWidth = 1280;
	const int kWindowHeight = 720;

	Sphere sphere = {
		{0.0f, 0.0f, 0.0f},
		0.6f
	};

	Plane plane = {
		{0.0f, 1.0f, 0.0f},
		1.0f
	};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///
		/// 


		//========================================  ビュー関連  ===========================================

		// カメラ
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
		// ビュー
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		// 透視投影
		Matrix4x4 projectMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectMatrix);
		// ビューポート変換
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		//=================================================================================================


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///


		// グリッド線の描画
		DrawGrid(viewProjectionMatrix, viewportMatrix);

		// 球を描画
		DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, WHITE);
		if (IsCollisionPlane(sphere, plane)) {
			DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, RED);
		}

		// 平面の描画
		DrawPlane(plane, viewProjectionMatrix, viewportMatrix, WHITE);


		// ImGui
		ImGui::Begin("Window");
		ImGui::DragFloat3("Sphere.Center", &sphere.center.x, 0.01f);
		ImGui::DragFloat("Sphere.Radius", &sphere.radius, 0.01f);
		ImGui::DragFloat3("Plane.Normal", &plane.normal.x, 0.01f);
		plane.normal = Normalize(plane.normal);
		ImGui::DragFloat("Plane.Distance", &plane.distance, 0.01f);
		ImGui::End();

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}