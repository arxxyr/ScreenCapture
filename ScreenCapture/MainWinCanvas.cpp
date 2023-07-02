#include "MainWin.h"



void MainWin::initCanvas()
{
    ComPtr<ID2D1Factory> d2dFactory;
    {
        //���������ڴ��� Direct2D ��Դ�Ĺ�������
        D2D1_FACTORY_OPTIONS fo = {};
        #ifdef _DEBUG
            fo.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
        #endif
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, fo, d2dFactory.GetAddressOf());
        if (!SUCCEEDED(hr)) return;
    }
    ComPtr<IDWriteFactory> dwrite;
    {
        //����һ��DirectWrite�����������ں�����������DirectWrite����
        //����������йأ���ʱû�õ�
        HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_ISOLATED, 
            __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(dwrite.GetAddressOf()));
        if (!SUCCEEDED(hr)) return;
    }
    ComPtr<ID2D1Factory1> d2dfactory1;
    {
        //����ͨ���� ID2D1GdiInteropRenderTarget �ĳ�����Ŀ���ϵ��� QueryInterface��
        //�� Direct2D ����Ŀ����ʹ�� GDI ���г��֣��ó���Ŀ���Ͼ��п����ڼ��� GDI �豸�����ĵ� GetDC �� ReleaseDC ������
        //������������D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE��־�ĳ���Ŀ��ʱ������ͨ�� GDI ���г��֡� 
        //�������Ҫʹ�� Direct2D ���ֵ�������չ��ģ�ͻ���Ҫ GDI ���ֹ��ܵ����������ݵ�Ӧ�ó���ǳ����á�
        HRESULT hr =d2dFactory->QueryInterface(d2dfactory1.GetAddressOf());
        if (!SUCCEEDED(hr)) return;
    }
    ComPtr<ID3D11Device> d3d11Device;
    {
        //������ʾ��ʾ���������豸��
        UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        #ifdef _DEBUG
            flags |= D3D11_CREATE_DEVICE_DEBUG;
        #endif
        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };
        HRESULT hr = D3D11CreateDevice(nullptr,
            D3D_DRIVER_TYPE_HARDWARE, //todo D3D_DRIVER_TYPE_WARP
            nullptr, flags, featureLevels,
            sizeof(featureLevels) / sizeof(*featureLevels),
            D3D11_SDK_VERSION, d3d11Device.GetAddressOf(), nullptr, nullptr);
        if (!SUCCEEDED(hr)) return;
    }
    ComPtr<IDXGIDevice> dxgiDevice;
    {
        //IDXGIDevice �ӿ�Ϊ����ͼ�����ݵ� DXGI ����ʵ�������ࡣ
        HRESULT hr = d3d11Device->QueryInterface(dxgiDevice.GetAddressOf());
        if (!SUCCEEDED(hr)) return;
    }
    ComPtr<ID2D1Device> d2d1Device;
    {
        //��ʾһ����Դ���������豸�����Ŀ���һ��ʹ�á�
        HRESULT hr = d2dfactory1->CreateDevice(dxgiDevice.Get(), d2d1Device.GetAddressOf());
        if (!SUCCEEDED(hr)) return;
    }    
    {
        //��ʾ���ڳ��ָ�Ŀ���һ��״̬�����������
        //�豸�����Ŀ��Գ���ΪĿ��λͼ�������б�
        HRESULT hr = d2d1Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, context.GetAddressOf());
        if (!SUCCEEDED(hr)) return;
    }
    ComPtr<IDXGIAdapter> dxgiAdapter;
    {
        //��ʾ��ʾ��ϵͳ�� (����һ������ GPU��DAC ����Ƶ�ڴ�) ��
        HRESULT hr = dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());
        if (!SUCCEEDED(hr)) return;
    }
    ComPtr<IDXGIFactory2> dxgiFactory;
    {
        //�����������б�IDXGISwapChain���๦�ܵ��°汾�������ͼ������3D���ܵķ�����
        HRESULT hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)dxgiFactory.GetAddressOf());
        if (!SUCCEEDED(hr)) return;
    }    
    {
        //�ṩ��IDXGISwapChain��ǿ�ı�ʾ���ܡ���Щ��ʾ���ܰ���ָ������κ͹����������Ż���ʾ��
        DXGI_SWAP_CHAIN_DESC1 props = {};
        props.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        props.SampleDesc.Count = 1;
        props.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        props.BufferCount = 2;
        HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(d3d11Device.Get(), hwnd, &props, nullptr, nullptr, dxgiSwapChain.GetAddressOf());
        if (!SUCCEEDED(hr)) return;
    }
    ComPtr<IDXGISurface> dxgiSurface;
    {
        //idxgissurface�ӿ�ʵ����ͼ�����ݶ���ķ�����
        IDXGISurface* surface = nullptr;
        HRESULT hr = dxgiSwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void**)&surface);
        if (!SUCCEEDED(hr))return;
        dxgiSurface = surface;
    } 
    ComPtr<ID2D1Bitmap1> d2dBitmap;
    {
        //��DXGI���洴��λͼ�����Խ�������Ϊtarget surface��ָ��������ɫ��������Ϣ��
        auto props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
        HRESULT hr = context->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), props, d2dBitmap.GetAddressOf());
        if (!SUCCEEDED(hr)) return;
    } 
    context->SetTarget(d2dBitmap.Get());
    
}

void MainWin::paintBg()
{
    static ComPtr<ID2D1Bitmap> bgImg;
    if (!bgImg.Get())
    {
        D2D1_BITMAP_PROPERTIES bmpPorp;
        bmpPorp.dpiX = 0.0f;
        bmpPorp.dpiY = 0.0f;
        bmpPorp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        bmpPorp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
        D2D1_SIZE_U size = D2D1::SizeU(w, h);
        context->CreateBitmap(size, bgPixels, 4 * w, bmpPorp, bgImg.GetAddressOf());
        delete[] bgPixels;
    }
    context->DrawBitmap(bgImg.Get(), D2D1::RectF(0, 0, w, h));
    //D2D1_RECT_F rect = D2D1::RectF(100.0, 100.0, 400.0, 600.0);
    //ID2D1SolidColorBrush* maskBrush;
    //context->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1), &maskBrush);
    //context->FillRectangle(&rect, maskBrush);
}

void MainWin::paint()
{
    context->BeginDraw();
    context->SetTransform(D2D1::Matrix3x2F::Identity());
    paintBg();
    paintMask();
    context->EndDraw();
    context->Flush();
    DXGI_PRESENT_PARAMETERS parameters = { 0 };
    dxgiSwapChain->Present1(1, 0, &parameters);
}

void MainWin::paintMask()
{
    static ComPtr<ID2D1Layer> layer;
    if (!layer.Get()) {
        context->CreateLayer(NULL, &layer);
    }
    static ComPtr<ID2D1SolidColorBrush> bgBrush;
    if (!bgBrush.Get()) {
        context->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.68f), &bgBrush);
    }
    static ComPtr<ID2D1SolidColorBrush> borderBrush;
    if (!borderBrush.Get()) {
        context->CreateSolidColorBrush(D2D1::ColorF(0x9ACD32, 1.0f), &borderBrush);
    }
    auto param = D2D1::LayerParameters(D2D1::InfiniteRect(),NULL,D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
        D2D1::IdentityMatrix(),1.0,NULL,D2D1_LAYER_OPTIONS_NONE);
    context->PushLayer(param, layer.Get());
    context->FillRectangle(D2D1::RectF(0, 0, w, h), bgBrush.Get());
    context->DrawRectangle(cutRect, borderBrush.Get(),8.0f);
    context->PushAxisAlignedClip(cutRect, D2D1_ANTIALIAS_MODE_ALIASED);
    context->Clear(D2D1::ColorF(0, 0, 0, 0));
    context->PopAxisAlignedClip();
    context->PopLayer();
}