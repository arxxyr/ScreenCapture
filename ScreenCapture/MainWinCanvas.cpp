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
        HRESULT hr = d2d1Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &context);
        if (!SUCCEEDED(hr)) return;
    }
    ComPtr<IDXGIAdapter> dxgiAdapter;
    {
        //��ʾ��ʾ��ϵͳ�� (����һ������ GPU��DAC ����Ƶ�ڴ�) ��
        HRESULT hr = dxgiDevice->GetAdapter(&dxgiAdapter);
        if (!SUCCEEDED(hr)) return;
    }
    ComPtr<IDXGIFactory2> dxgiFactory;
    {
        //�����������б�IDXGISwapChain���๦�ܵ��°汾�������ͼ������3D���ܵķ�����
        HRESULT hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&dxgiFactory);
        if (!SUCCEEDED(hr)) return;
    }    
    {
        //�ṩ��IDXGISwapChain��ǿ�ı�ʾ���ܡ���Щ��ʾ���ܰ���ָ������κ͹����������Ż���ʾ��
        DXGI_SWAP_CHAIN_DESC1 props = {};
        props.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        props.SampleDesc.Count = 1;
        props.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        props.BufferCount = 2;
        props.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        props.AlphaMode = DXGI_ALPHA_MODE_IGNORE;


        HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(d3d11Device.Get(), hwnd, &props, nullptr, nullptr, &dxgiSwapChain);
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
    
    {
        //��DXGI���洴��λͼ�����Խ�������Ϊtarget surface��ָ��������ɫ��������Ϣ��
        auto props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
        HRESULT hr = context->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), props, &d2dBitmap);
        if (!SUCCEEDED(hr)) return;
    }   

    
}

void MainWin::paintBg()
{
    static ComPtr<ID2D1Bitmap> bgImg;
    if (!bgImg.Get())
    {
        D2D1_BITMAP_PROPERTIES imgConfig;
        imgConfig.dpiX = 0.0f;
        imgConfig.dpiY = 0.0f;
        imgConfig.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        imgConfig.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
        context->CreateBitmap(D2D1::SizeU(w, h), bgPixels, 4 * w, imgConfig, bgImg.GetAddressOf());       
        delete[] bgPixels;
    }    
    context->DrawBitmap(bgImg.Get(), D2D1::RectF(0, 0, w, h));
    

    //static ComPtr<ID2D1Layer> layer;
    //if (!layer.Get()) {
    //    context->CreateLayer(NULL, &layer);
    //}
    //static ComPtr<ID2D1SolidColorBrush> defaultBrush;
    //if (!defaultBrush.Get()) {
    //    context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &defaultBrush);
    //}
    //static ComPtr<ID2D1SolidColorBrush> clearBrush;
    //if (!clearBrush.Get()) {
    //    context->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1), &clearBrush);
    //}
    //auto param = D2D1::LayerParameters(D2D1::InfiniteRect(), NULL, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
    //    D2D1::IdentityMatrix(), 1.0, clearBrush.Get(), D2D1_LAYER_OPTIONS_NONE);
    //context->PushLayer(param, layer.Get());
    //context->DrawLine(D2D1::Point2F(0.0f, 0.0f), D2D1::Point2F(w, h), defaultBrush.Get(), 16.5f);
    //context->DrawLine(D2D1::Point2F(0.0f, h), D2D1::Point2F(w, 0), defaultBrush.Get(),16.5f);
    //context->PopLayer();
}

void MainWin::paintCanvas()
{
    if (!canvasImg.Get())
    {
        unsigned int dataSize = w * h * 4;
        char* pixels = new char[dataSize];
        memset(pixels, 0, dataSize);
        auto props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
        context->CreateBitmap(D2D1::SizeU(w, h), pixels, 4 * w, props, &canvasImg);
        delete[] pixels;
    }
    static ComPtr<ID2D1SolidColorBrush> defaultBrush;
    if (!defaultBrush.Get()) {
        context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &defaultBrush);
    }
    context->SetTarget(canvasImg.Get());
    context->FillRectangle(D2D1::RectF(80.0f, 80.0f, 1280.0f, 1280.0f ), defaultBrush.Get());
    //defaultBrush->SetColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
    //context->DrawLine(D2D1::Point2F(0.0f, h), D2D1::Point2F(w, 0), defaultBrush.Get(),16.5f);
    context->SetTarget(d2dBitmap.Get());
    context->DrawBitmap(canvasImg.Get(), D2D1::RectF(0, 0, w, h));
}

void MainWin::paint()
{
    context->SetTarget(d2dBitmap.Get());
    context->BeginDraw();
    context->SetTransform(D2D1::Matrix3x2F::Identity());
    paintBg();
    if (state != State::Eraser) {
        paintCanvas();
    }
    else {
        paintEraser();
    } 
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
    auto param = D2D1::LayerParameters1(D2D1::InfiniteRect(),NULL,D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
        D2D1::IdentityMatrix(),1,NULL, D2D1_LAYER_OPTIONS1_NONE);
    context->PushLayer(param, layer.Get());
    context->FillRectangle(D2D1::RectF(0, 0, w, h), bgBrush.Get());
    context->DrawRectangle(cutRect, borderBrush.Get(),8.0f);
    context->PushAxisAlignedClip(cutRect, D2D1_ANTIALIAS_MODE_ALIASED);
    context->Clear(D2D1::ColorF(0, 0, 0, 0));
    context->PopAxisAlignedClip();
    context->PopLayer();
}

void MainWin::paintEraser()
{
    //D2D1_BITMAP_BRUSH_PROPERTIES1 propertiesXClampYClamp = D2D1::BitmapBrushProperties1(D2D1_EXTEND_MODE_CLAMP,
    //                D2D1_EXTEND_MODE_CLAMP,D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    //ComPtr<ID2D1BitmapBrush1> bitmapBrush;
    //context->CreateBitmapBrush(canvasImg.Get(), propertiesXClampYClamp, &bitmapBrush);
    //context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
    //context->FillOpacityMask(eraserImg.Get(),bitmapBrush.Get(),
    //    D2D1_OPACITY_MASK_CONTENT_GRAPHICS);
    //context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        
    static ComPtr<ID2D1Effect> compositeEffect;
    if (!compositeEffect.Get()) {
        context->CreateEffect(CLSID_D2D1Composite, &compositeEffect);
    }
    compositeEffect->SetInput(0, eraserImg.Get());
    context->SetTarget(canvasImg.Get());
    context->DrawImage(compositeEffect.Get(), D2D1_INTERPOLATION_MODE_LINEAR,D2D1_COMPOSITE_MODE_SOURCE_OUT);
    context->SetTarget(d2dBitmap.Get());
    context->DrawBitmap(canvasImg.Get(), D2D1::RectF(0, 0, w, h));
}