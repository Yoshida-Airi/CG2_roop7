#include<Windows.h>
#include<cstdint>
#include <string>
#include<format>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")


void Log(const std::string& message)
{
	OutputDebugStringA(message.c_str());
}



//wstring����string�֕ϊ�����֐�
std::wstring ConvertString(const std::string& str)
{
	if (str.empty())
	{
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
	if (sizeNeeded == 0)
	{
		return std::wstring();
	}
	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
	return result;
}

std::string ConvertString(const std::wstring& str)
{
	if (str.empty())
	{
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
	if (sizeNeeded == 0)
	{
		return std::string();
	}
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
	return result;
}



//�E�B���h�E�v���V�[�W��
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//���b�Z�[�W�ɉ����ăQ�[���ŗL�̏������s��
	switch (msg)
	{
		//�E�B���h�E���j�󂳂ꂽ
	case WM_DESTROY:
		//OS�ɑ΂��āA�A�v���̏I����`����
		PostQuitMessage(0);
		return 0;
	}
	//�W���̃��b�Z�[�W�������s��
	return DefWindowProc(hwnd, msg, wparam, lparam);
}



//Windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{


	WNDCLASS wc{};
	//�E�B���h�E�v���V�[�W��
	wc.lpfnWndProc = WindowProc;
	//�E�B���h�E�N���X��(�Ȃ�ł��ǂ�)
	wc.lpszClassName = L"CG2WindowClass";
	//�C���X�^���X�n���h��
	wc.hInstance = GetModuleHandle(nullptr);
	//�J�[�\��
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//�E�B���h�E�N���X��o�^����
	RegisterClass(&wc);



	//�N���C�A���g�̈�̃T�C�Y
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;

	//�E�B���h�E�T�C�Y��\���\���̂ɃN���C�A���g�̈������
	RECT wrc = { 0,0,kClientWidth,kClientHeight };

	//�N���C�A���g�̈�����ƂɎ��ۂ̃T�C�Y��wrc��ύX���Ă��炤
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);



	//�E�B���h�E�̐���
	HWND hwnd = CreateWindow
	(
		wc.lpszClassName,		//���p����N���X��
		L"CG2",					//�^�C�g���o�[�̕���(���ł��ǂ�)
		WS_OVERLAPPEDWINDOW,	//�悭����E�B���h�E�X�^�C��
		CW_USEDEFAULT,			//�\��X���W(Windows�ɔC����)
		CW_USEDEFAULT,			//�\��Y���W(WindowsOS�ɔC����)
		wrc.right - wrc.left,	//�E�B���h�E����
		wrc.bottom - wrc.top,	//�E�B���h�E�c��
		nullptr,				//�e�E�B���h�E�n���h��
		nullptr,				//���j���[�n���h��
		wc.hInstance,			//�C���X�^���X�n���h��
		nullptr					//�I�v�V����
	);

	//�E�B���h�E��\������
	ShowWindow(hwnd, SW_SHOW);



	//DXGI�t�@�N�g���[�̐���
	IDXGIFactory7* dxgiFactory = nullptr;
	//HRESULT��Windows�n�̃G���[�R�[�h�ł���A
	//�֐��������������ǂ�����SUCCEEDED�}�N���Ŕ���ł���
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	//�������̍��{�I�ȕ����ŃG���[���o���ꍇ�̓v���O�������Ԉ�Ă��邩
	//�ǂ��ɂ��ł��Ȃ��ꍇ�������̂�assert�ɂ��Ă���
	assert(SUCCEEDED(hr));



	//�g�p����A�_�v�^�p�̕ϐ��B�ŏ���nullptr�����Ă���
	IDXGIAdapter4* useAdapter = nullptr;
	//�ǂ����ɃA�_�v�^�𗊂�
	for (UINT i = 0;
		dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND;
		++i)
	{
		//�A�_�v�^�[�̏����擾����
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));//�擾�ł��Ȃ��͈̂�厖
		//�\�t�g�E�F�A�A�_�v�^�łȂ���΍̗p�I
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE))
		{
			//�̗p�����A�_�v�^�̏������O�ɏo�́Bwstring�̕��Ȃ̂Œ���
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
		}
	}





	ID3D12Device* device = nullptr;
	//�@�\���x���ƃ��O�o�͗p�̕�����
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] =
	{
		"12.2",
		"12.1",
		"12.0"
	};
	//�������ɐ����ł��邩�����Ă���
	for (size_t i = 0; i < _countof(featureLevels); ++i)
	{
		//�̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
		//�w�肵���@�\���x���Ńf�o�C�X�������o���������m�F
		if (SUCCEEDED(hr))
		{
			//�����ł����̂Ń��O�o�͂��s���ă��[�v�𔲂���
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	//�f�o�C�X�̐��������܂������Ȃ������̂ŋN���ł��Ȃ�
	assert(device != nullptr);
	Log("Complete create D3D12Device!!!\n");//�����������̃��O������



	//****���C�����[�v****//
	MSG msg{};
	//�E�B���h�E�́~�{�^�����������܂Ń��[�v
	while (msg.message != WM_QUIT)
	{
		//Windows�Ƀ��b�Z�[�W�����Ă���ŗD��ŏ���������
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//�Q�[���̏���
		}
	}



	//�o�̓E�B���h�E�ւ̕����o��
	OutputDebugStringA("Hello,DirectX!\n");

	return 0;
}


