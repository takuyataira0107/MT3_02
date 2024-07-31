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
	Vector3 cameraScale = { 1.0f, 1.0f, 1.0f };
	Vector3 cameraRotate = { 0.26f, 0.0f, 0.0f };
	Vector3 cameraTranslate = { 0.0f, 1.9f, -6.49f };
	const int kWindowWidth = 1280;
	const int kWindowHeight = 720;


	AABB aabb{
		.min{-0.5f, -0.5f, -0.5f},
		.max{0.5f, 0.5f, 0.5f}
	};
	Segment segment{
		.origin{-0.7f, 0.3f, 0.0f},
		.diff{2.0f, -0.5f, 0.0f}
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
		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraScale, cameraRotate, cameraTranslate);
		// ビュー
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		// 透視投影
		Matrix4x4 projectMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectMatrix);
		// ビューポート変換
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		//=================================================================================================

		aabb.min.x = (std::min)(aabb.min.x, aabb.max.x);
		aabb.min.y = (std::min)(aabb.min.y, aabb.max.y);
		aabb.min.z = (std::min)(aabb.min.z, aabb.max.z);
		aabb.max.x = (std::max)(aabb.min.x, aabb.max.x);
		aabb.max.y = (std::max)(aabb.min.y, aabb.max.y);
		aabb.max.z = (std::max)(aabb.min.z, aabb.max.z);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///


		// グリッド線の描画
		DrawGrid(viewProjectionMatrix, viewportMatrix);

		// segmentの描画
		DrawLineSegment(segment, viewProjectionMatrix, viewportMatrix, WHITE);
		
		// AABBの描画
		if (IsCollisionAABBSeg(aabb, segment)) {
			DrawAABB(aabb, viewProjectionMatrix, viewportMatrix, RED);
		} else {
			DrawAABB(aabb, viewProjectionMatrix, viewportMatrix, WHITE);
		}

		// ImGui
		ImGui::Begin("Window");
		if (ImGui::CollapsingHeader("camera")) {
			ImGui::DragFloat3("cameraScale", &cameraScale.x, 0.01f);
			ImGui::DragFloat3("cameraRotate", &cameraRotate.x, 0.01f);
			ImGui::DragFloat3("cameraTranslate", &cameraTranslate.x, 0.01f);
		}
		if (ImGui::CollapsingHeader("Segment")) {
			ImGui::DragFloat3("Segment.Origin", &segment.origin.x, 0.01f);
			ImGui::DragFloat3("Segment.Diff", &segment.diff.x, 0.01f);
		}
		if (ImGui::CollapsingHeader("aabb")) {
			ImGui::DragFloat3("aabb.min", &aabb.min.x, 0.01f);
			ImGui::DragFloat3("aabb.max", &aabb.max.x, 0.01f);
		}
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
