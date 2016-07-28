#pragma once

namespace Vergara_B_Graphics_II_project
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};

	struct S_VERTEX
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 uvs;
		DirectX::XMFLOAT3 normals;
	};

	struct N_VERTEX
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 uvs;
		DirectX::XMFLOAT3 normals;
	};

	struct LIGHTING
	{
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT3 direction;
		float padding;

		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 direction2;
		float lightRadius;
		DirectX::XMFLOAT3 position;
		float padding2;

		DirectX::XMFLOAT4 color2;
		DirectX::XMFLOAT3 direction3;
		float coneRatio;
		DirectX::XMFLOAT3 position2;
		DirectX::XMFLOAT4 cameraPosition;
		float padding3;
	};

	struct OBJECT
	{
		DirectX::XMFLOAT3 vertices;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 normals;
	};

}