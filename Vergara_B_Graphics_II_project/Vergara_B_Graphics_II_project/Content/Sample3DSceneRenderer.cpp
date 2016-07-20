#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"

using namespace Vergara_B_Graphics_II_project;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_indexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	static const XMVECTORF32 eye = { 0.0f, 0.0f, -2.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, 0.1f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };
	XMStoreFloat4x4(&camera, XMMatrixLookAtRH(eye, at, up));
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, XMMatrixLookAtRH(eye, at, up))));
	XMStoreFloat4x4(&m_constantBufferDataStar.view, XMMatrixTranspose(XMMatrixInverse(0, XMMatrixLookAtRH(eye, at, up))));
	XMStoreFloat4x4(&m_constantBufferDataGround.view, XMMatrixTranspose(XMMatrixInverse(0, XMMatrixLookAtRH(eye, at, up))));
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
		);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(&m_constantBufferData.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

	XMStoreFloat4x4(&m_constantBufferDataStar.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

	XMStoreFloat4x4(&m_constantBufferDataGround.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	
}

using namespace Windows::UI::Core;
extern CoreWindow^ gwindow;
#include <atomic>
extern bool mouse_move;
extern float diffx;
extern float diffy;
extern bool w_down;
extern bool a_down;
extern bool s_down;
extern bool d_down;
extern bool left_click;

extern char buttons[256];

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	if (!m_tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		Rotate(radians);
	}

	XMMATRIX newCamera = XMLoadFloat4x4(&camera);
	
	if (buttons['W'])
	{
		newCamera.r[3] = newCamera.r[3] + newCamera.r[2] * -timer.GetElapsedSeconds() * 5.0;
	}
	
	if (a_down)
	{
		newCamera.r[3] = newCamera.r[3] + newCamera.r[0] * -timer.GetElapsedSeconds() *5.0;
	}
	
	if (s_down)
	{
		newCamera.r[3] = newCamera.r[3] + newCamera.r[2] * timer.GetElapsedSeconds() * 5.0;
	}
	
	if (d_down)
	{
		newCamera.r[3] = newCamera.r[3] + newCamera.r[0] * timer.GetElapsedSeconds() * 5.0;
	}
	
	Windows::UI::Input::PointerPoint^ point = nullptr;
	
	//if(mouse_move)/*This crashes unless a mouse event actually happened*/
	//point = Windows::UI::Input::PointerPoint::GetCurrentPoint(pointerID);
	
	if (mouse_move)
	{
		// Updates the application state once per frame.
		if (left_click)
		{
			XMVECTOR pos = newCamera.r[3];
			newCamera.r[3] = XMLoadFloat4(&XMFLOAT4(0, 0, 0, 1));
			newCamera = XMMatrixRotationX(-diffy*0.01f) * newCamera * XMMatrixRotationY(-diffx*0.01f);
			newCamera.r[3] = pos;
		}
	}
	
	XMStoreFloat4x4(&camera, newCamera);
	
	/*Be sure to inverse the camera & Transpose because they don't use pragma pack row major in shaders*/
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(0, newCamera)));
	XMStoreFloat4x4(&m_constantBufferDataStar.view, XMMatrixTranspose(XMMatrixInverse(0, newCamera)));
	XMStoreFloat4x4(&m_constantBufferDataGround.view, XMMatrixTranspose(XMMatrixInverse(0, newCamera)));

	mouse_move = false;/*Reset*/
}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(0)));
	XMStoreFloat4x4(&m_constantBufferDataStar.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
	XMStoreFloat4x4(&m_constantBufferDataGround.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	m_tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	// Each index is one 16-bit unsigned integer (short).
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);

	// Attach our pixel shader.
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	// Draw the objects.
	context->DrawIndexed(m_indexCount, 0, 0);


	//XMStoreFloat4x4(&m_constantBufferDataStar.view, XMMatrixTranslation(m_constantBufferDataStar.view._11 - 2.0f, m_constantBufferDataStar.view._21, m_constantBufferDataStar.view._31));
	
	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(m_constantBufferStar.Get(), 0, NULL, &m_constantBufferDataStar, 0, 0, 0);

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT strideStar = sizeof(S_VERTEX);
	UINT offsetStar = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBufferStar.GetAddressOf(), &strideStar, &offsetStar);

	// Each index is one 16-bit unsigned integer (short).
	context->IASetIndexBuffer(m_indexBufferStar.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayoutStar.Get());

	// Attach our vertex shader.
	context->VSSetShader(m_vertexShaderStar.Get(), nullptr, 0);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_constantBufferStar.GetAddressOf(), nullptr, nullptr);

	// Attach our pixel shader.
	context->PSSetShader(m_pixelShaderStar.Get(), nullptr, 0);

	// Draw the objects.
	context->DrawIndexed(m_indexCountStar, 0, 0);


	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(m_constantBufferGround.Get(), 0, NULL, &m_constantBufferDataGround, 0, 0, 0);

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT strideGround = sizeof(VertexPositionColor);
	UINT offsetGround = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBufferGround.GetAddressOf(), &strideGround, &offsetGround);

	// Each index is one 16-bit unsigned integer (short).
	context->IASetIndexBuffer(m_indexBufferGround.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayoutGround.Get());

	// Attach our vertex shader.
	context->VSSetShader(m_vertexShaderGround.Get(), nullptr, 0);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_constantBufferGround.GetAddressOf(), nullptr, nullptr);

	// Attach our pixel shader.
	context->PSSetShader(m_pixelShaderGround.Get(), nullptr, 0);

	// Draw the objects.
	context->DrawIndexed(m_indexCountGround, 0, 0);

}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");

	auto loadVSTask2 = DX::ReadDataAsync(L"Star_VertexShader.cso");
	auto loadPSTask2 = DX::ReadDataAsync(L"Star_PixelShader.cso");

	auto loadVSTask3 = DX::ReadDataAsync(L"Ground_VertexShader.cso");
	auto loadPSTask3 = DX::ReadDataAsync(L"Ground_PixelShader.cso");


#pragma region Skybox VS & PS
	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader
				)
			);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc [] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
				)
			);
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
			)
		);

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_constantBuffer
			)
		);
	});
#pragma endregion
	
#pragma region Star VS & PS
	auto createVSTask2 = loadVSTask2.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShaderStar
			)
		);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc2[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMALS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc2,
				ARRAYSIZE(vertexDesc2),
				&fileData[0],
				fileData.size(),
				&m_inputLayoutStar
			)
		);
	});

	auto createPSTask2 = loadPSTask2.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShaderStar
			)
		);

		CD3D11_BUFFER_DESC constantBufferDescStar(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDescStar,
				nullptr,
				&m_constantBufferStar
			)
		);
	});
#pragma endregion

#pragma region Ground VS & PS
	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask3 = loadVSTask3.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShaderGround
			)
		);

		static const D3D11_INPUT_ELEMENT_DESC vertexDescGround[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDescGround,
				ARRAYSIZE(vertexDescGround),
				&fileData[0],
				fileData.size(),
				&m_inputLayoutGround
			)
		);
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask3 = loadPSTask3.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShaderGround
			)
		);

		CD3D11_BUFFER_DESC constantBufferDescGround(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDescGround,
				nullptr,
				&m_constantBufferGround
			)
		);
	});
#pragma endregion


#pragma region Create Skybox
	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this] () {

		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPositionColor cubeVertices[] = 
		{
			{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
			{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
			{XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f)},
			{XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
			{XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
			{XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
			{XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
		};

		D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
				)
			);

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the 
		// first triangle of this mesh.
		static const unsigned short cubeIndices [] =
		{
			0,2,1, // -x
			1,2,3,

			4,5,6, // +x
			5,7,6,

			0,1,5, // -y
			0,5,4,

			2,6,7, // +y
			2,7,3,

			0,4,6, // -z
			0,6,2,

			1,3,7, // +z
			1,7,5,
		};

		m_indexCount = ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = {0};
		indexBufferData.pSysMem = cubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_indexBuffer
				)
			);
	});
#pragma endregion

#pragma region Create Star
	auto CreateStarTask = (createPSTask2 && createVSTask2).then([this]() {

		S_VERTEX star[12];

		for (int i = 0; i < 12; ++i)
		{
			if (i == 0 || i == 11)
			{
				star[i].position.x = 0.0f;
				star[i].position.y = 0.0f;

				if (i == 0)
					star[i].position.z = -0.2f;
				else if (i == 11)
					star[i].position.z = 0.2f;
				star[i].position.w = 1.0f;
				star[i].uvs.x = 1.0f;
				star[i].uvs.y = 1.0f;
				star[i].uvs.z = 1.0f;
				star[i].uvs.w = 1.0f;
			}
			else
			{
				star[i].position.x = cos(XMConvertToRadians(i) * 36);
				star[i].position.y = sin(XMConvertToRadians(i) * 36);
				star[i].position.z = 0.0f;
				star[i].position.w = 1.0f;
				star[i].uvs.x = 0.0f;
				star[i].uvs.y = 0.0f;
				star[i].uvs.z = 1.0f;
				star[i].uvs.w = 1.0f;

				if (i % 2 == 0)
				{
					star[i].position.x = cos(XMConvertToRadians(i) * 36) / 2;
					star[i].position.y = sin(XMConvertToRadians(i) * 36) / 2;
				}

			}
		}

		for (int i = 0; i < 12; ++i)
		{
			star[i].normals.x = 0;
			star[i].normals.y = 1;
			star[i].normals.z = 0;
		}

		D3D11_SUBRESOURCE_DATA vertexBufferDataStar = { 0 };
		vertexBufferDataStar.pSysMem = star;
		vertexBufferDataStar.SysMemPitch = 0;
		vertexBufferDataStar.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDescStar(sizeof(star), D3D11_BIND_VERTEX_BUFFER);

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDescStar,
				&vertexBufferDataStar,
				&m_vertexBufferStar
			)
		);

		unsigned int indices[60] = { 11, 2, 1, 11, 3, 2, 11, 4, 3, 11, 5, 4, 11, 6, 5, 11, 7, 6, 11, 8, 7, 11, 9, 8, 11, 10, 9, 11, 1, 10, 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 6, 0, 6, 7, 0, 7, 8, 0, 8, 9, 0, 9, 10, 0, 10, 1 };

		m_indexCountStar = ARRAYSIZE(indices);

		D3D11_SUBRESOURCE_DATA indexBufferDataStar = { 0 };
		indexBufferDataStar.pSysMem = indices;
		indexBufferDataStar.SysMemPitch = 0;
		indexBufferDataStar.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDescStar(sizeof(indices), D3D11_BIND_INDEX_BUFFER);
		
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDescStar,
				&indexBufferDataStar,
				&m_indexBufferStar
			)
		);

	});
#pragma endregion

#pragma region Create Ground
	auto CreateGroundTask = (createPSTask3 && createVSTask3).then([this]() {

		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPositionColor ground[] =
		{
			{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
		};

		D3D11_SUBRESOURCE_DATA vertexBufferDataGround = { 0 };
		vertexBufferDataGround.pSysMem = ground;
		vertexBufferDataGround.SysMemPitch = 0;
		vertexBufferDataGround.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDescGround(sizeof(ground), D3D11_BIND_VERTEX_BUFFER);

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDescGround,
				&vertexBufferDataGround,
				&m_vertexBufferGround
			)
		);

		static const unsigned short indices[] =
		{
			0,1,2, // -x
			1,2,3,

			4,5,6, // +x
			5,7,6,

			0,1,5, // -y
			0,5,4,

			2,6,7, // +y
			2,7,3,

			0,4,6, // -z
			0,6,2,

			1,3,7, // +z
			1,7,5,
		};

		m_indexCountGround = ARRAYSIZE(indices);

		D3D11_SUBRESOURCE_DATA indexBufferDataGround = { 0 };
		indexBufferDataGround.pSysMem = indices;
		indexBufferDataGround.SysMemPitch = 0;
		indexBufferDataGround.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDescGround(sizeof(indices), D3D11_BIND_INDEX_BUFFER);

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDescGround,
				&indexBufferDataGround,
				&m_indexBufferGround
			)
		);

	});
#pragma endregion

	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this]() {m_loadingComplete = true; });

}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();

	m_inputLayoutStar.Reset();
	m_vertexBufferStar.Reset();
	m_indexBufferStar.Reset();
	m_vertexShaderStar.Reset();
	m_pixelShaderStar.Reset();
	m_constantBufferStar.Reset();

	m_inputLayoutGround.Reset();
	m_vertexBufferGround.Reset();
	m_indexBufferGround.Reset();
	m_vertexShaderGround.Reset();
	m_pixelShaderGround.Reset();
	m_constantBufferGround.Reset();
}