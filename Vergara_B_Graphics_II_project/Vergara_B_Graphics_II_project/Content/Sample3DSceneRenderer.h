#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include <iostream>
#include <ctime>
#include <cmath>
#include <string.h>
#include "DDSTextureLoader.h"
#include <DirectXMath.h>
#include <vector>
#include <thread>

using namespace std;
using namespace DirectX;

namespace Vergara_B_Graphics_II_project
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }


	private:
		void Rotate(float radians);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_shaderResourceViewSky;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32	m_indexCount;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
		
		XMFLOAT4X4 world, camera, projection, camera2;
		XMMATRIX newCamera, newCamera2;

		bool spotFlip = false;

		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_lightBuffer;
		LIGHTING m_lightsBufferData;

		//STAR

		// Direct3D resources for star geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayoutStar;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBufferStar;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBufferStar;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShaderStar;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShaderStar;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBufferStar;

		// System resources for star geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferDataStar;
		uint32	m_indexCountStar;


		// Direct3D resources for Ground geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayoutGround;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBufferGround;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBufferGround;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShaderGround;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShaderGround;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBufferGround;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_shaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_shaderResourceViewG;

		// System resources for Ground geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferDataGround;
		uint32	m_indexCountGround;

	};
}

