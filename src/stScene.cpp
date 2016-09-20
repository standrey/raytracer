#include <windows.h>
#include <mmsystem.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "stScene.h"
#pragma comment(lib,"WINMM.LIB")

float InvSqrt (float x){
    float xhalf = 0.5f*x;
    int i = *(int*)&x;
    i = 0x5f3759df - (i>>1);
    x = *(float*)&i;
    x = x*(1.5f - xhalf*x*x);
    return x;
}

float srgbEncode(float c)
{
    if (c <= 0.0031308f)
    {
        return 12.92f * c; 
    }
    else
    {
        return 1.055f * powf(c, 0.4166667f) - 0.055f; // Inverse gamma 2.4
    }
}

stScene::stScene(IDirect3DDevice9 * dev, INT width, INT height)
{
	string ResFolder("res");

	this->stSceneSizeY = width;
	this->stSceneSizeX = height;
	rayArray = new stRay*[width];

	for (int i = 0; i < width; ++i)
		rayArray[i] = new stRay[height];

	for (int i = 0; i < width; ++i)
		for (int j = 0; j < height; ++j)
		{
			rayArray[i][j].color.r = 255;
			rayArray[i][j].color.g = 255;
			rayArray[i][j].color.b = 255;
		}

	d3ddev = dev;
	ambientColor.r = 0.3f;
	ambientColor.g = 0.3f;
	ambientColor.b = 0.3f;
	D3DXCreateFont(d3ddev, 24, 0, 500, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,    DEFAULT_PITCH | FF_DONTCARE, L"Arial", &font);
	FPS = 0.0f;
	FrameCnt = 0;
	TimeElapsed = 0;

	specularEnabled = true;
	reflectionEnabled = true;

	objConfig = std::unique_ptr<ConfigLoader>(new ConfigLoader("res\\config.txt"));
	listPointLights = objConfig->GetLights();
	listMaterials = objConfig->GetMaterials();
	listSpheres = objConfig->GetSpheres();

	//init cubic environment
	HRESULT hr = D3DXCreateTextureFromFile(d3ddev, L"res\\cubictexture_2.jpg", &(objConfig->m_cubemap->texture));
	if (FAILED(hr)) {
		MessageBox(NULL, L"Error while reading cubictexture.jpeg", L"Error!", MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	D3DSURFACE_DESC texDesc;
	objConfig->m_cubemap->texture->GetLevelDesc(0, &texDesc);
	
	//texture FVF FORMAT is DWORD X8R8G8B8
	//real texture size after load into DIRECTX (128x1024 (H*W) instead of 100x600)
	//div by 6 because of 6 edges of texture
	objConfig->m_cubemap->h = texDesc.Height/6;
	objConfig->m_cubemap->w = texDesc.Width;

	//debug code begin
	IDirect3DTexture9 * textToSave;
	hr = D3DXCreateTexture(d3ddev, texDesc.Width, texDesc.Height, 1, D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &textToSave);

	D3DLOCKED_RECT srcRect;
    ZeroMemory(&srcRect, sizeof(D3DLOCKED_RECT));
	
	D3DLOCKED_RECT dstRect;
    ZeroMemory(&dstRect, sizeof(D3DLOCKED_RECT));

	hr = objConfig->m_cubemap->texture->LockRect(0, &srcRect, NULL, D3DLOCK_READONLY);
	hr = textToSave->LockRect(0, &dstRect, NULL, D3DLOCK_READONLY);

	int selectedPart = 1;
	int texturePartWidth = texDesc.Width/6;
	for (int y = 0; y < texturePartWidth; y++)
		for (int x = 0; x < texDesc.Height; x++) {
			((DWORD *)dstRect.pBits)[y + x * (dstRect.Pitch/4)] = ((DWORD *)srcRect.pBits)[y + x * (srcRect.Pitch/4) + texturePartWidth * selectedPart];
		}
	objConfig->m_cubemap->texture->UnlockRect(0);
	textToSave->UnlockRect(0);
	hr = D3DXSaveTextureToFile(L"res\\debugtexture.jpg", D3DXIFF_JPG, textToSave, NULL);
	textToSave->Release();
	//debug code end
}

stScene::~stScene() {
	for (int i = 0; i < stSceneSizeY; ++i)
		delete rayArray[i];
	delete [] rayArray;
	listSpheres.clear();
}

void stScene::KeyPress(UINT key)
{
	switch (key)
	{
	case VK_F1:
		if (specularEnabled)
			specularEnabled = false;
		else
			specularEnabled = true;
		break;
	case VK_F2:
		if (reflectionEnabled)
			reflectionEnabled = false;
		else
			reflectionEnabled = true;
		break;
	case VK_DOWN:
		_cprintf("Changed light pos\n");
		break;
	case VK_UP:
		_cprintf("Changed light pos\n");
		break;
	default:
		break;
	}
}

void stScene::UpdateObjects()
{
}

void stScene::DrawObjects()
{	
	DWORD oldTime = timeGetTime();
	d3ddev->Clear(0, 0, D3DCLEAR_TARGET , D3DCOLOR_XRGB(100, 100, 100), 1.0f, 0);
	d3ddev->BeginScene();
	calcObjects();
	drawFPS();
	d3ddev->EndScene();
	d3ddev->Present(0, 0, 0, 0);
	calcFPS(oldTime);
}

void stScene::calcFPS(DWORD oldTime)
{
      FrameCnt++;
      DWORD newTime = timeGetTime(); // записываем в ньютайм время после всех выполненных операциях
      DWORD deltatime = newTime - oldTime; // узнаем сколько времени проходит от начала до конца
      TimeElapsed += deltatime; // и прибавляем эту разницу к таймэлапседу
 
      if(TimeElapsed >= 1000) // если в таймэлапседе накопилось 0.5 сек, то
      {
        FPS = (1000.0f * FrameCnt) / TimeElapsed; // делим кол-во кадров прошедшее за полсекунды на таймэлапсед(равный примерно 0.5 сек)
        TimeElapsed = 0; // обнуляем таймэлапсед, для следующего подсчета
        FrameCnt = 0; // и обнуляем кол-во кадров
      }
}

void stScene::drawFPS()
{
	wchar_t textbuffer[255];
    swprintf( textbuffer, L"FPS: %.3f ", FPS);
    
	RECT FontPosition;
    FontPosition.top    = 10; 
    FontPosition.left   = 10;
    FontPosition.right  = 610;
    FontPosition.bottom = 30;

    font->DrawText( NULL, textbuffer, -1, &FontPosition, DT_LEFT, 0xFFFFFF80 );
}

D3DCOLORVALUE stScene::readTexture(int iTileTex, float uCoord, float vCoord, const D3DLOCKED_RECT & rect)
{
	float sizeU = (float)objConfig->m_cubemap->w;
	float sizeV = (float)objConfig->m_cubemap->h;
	D3DXCOLOR outputColor;
	memset(&outputColor, 0, sizeof(outputColor));
    float u = fabsf(uCoord);
    float v = fabsf(vCoord);
    int umin = int(sizeU * u);
    int vmin = int(sizeV * v);
    int umax = int(sizeU * u) + 1;
    int vmax = int(sizeV * v) + 1;
	
	umin = (int)min(max((float)umin, 0.0f), sizeU - 1.0f);
    umax = (int)min(max((float)umax, 0.0f), sizeU - 1.0f);
    vmin = (int)min(max((float)vmin, 0.0f), sizeV - 1.0f);
	vmax = (int)min(max((float)vmax, 0.0f), sizeV - 1.0f);

	//texture FVF FORMAT is DWORD X8R8G8B8
	//real texture size after load into DIRECTX (1024x128 (H*W) instead of 600x100)
	DWORD * dwColor = (DWORD *)rect.pBits + rect.Pitch/4*objConfig->m_cubemap->h*iTileTex;
	//чтобы произвести математические операции с цветом делаем из DWORD "правильную" структуру
	D3DXCOLOR colorP0(dwColor[umin + (rect.Pitch/4) * vmin]);
	D3DXCOLOR colorP1(dwColor[umax + (rect.Pitch/4) * vmin]);
	D3DXCOLOR colorP2(dwColor[umin + (rect.Pitch/4) * vmax]);
	D3DXCOLOR colorP3(dwColor[umax + (rect.Pitch/4) * vmax]);
	/*
	D3DCOLOR P0 = ((DWORD *)rect.pBits)[umin + (rect.Pitch/4) * vmin + (rect.Pitch/4)/6 * iTileTex];
	D3DCOLOR P1 = ((DWORD *)rect.pBits)[umax + (rect.Pitch/4) * vmin + (rect.Pitch/4)/6 * iTileTex];
	D3DCOLOR P2 = ((DWORD *)rect.pBits)[umin + (rect.Pitch/4) * vmax + (rect.Pitch/4)/6 * iTileTex];
	D3DCOLOR P3 = ((DWORD *)rect.pBits)[umax + (rect.Pitch/4) * vmax + (rect.Pitch/4)/6 * iTileTex];
	*/
	outputColor = ((1 - u) * colorP0 + u * colorP1) * (1-v) + ((1 - u) * colorP2 + u * colorP3) * v;
	
	return outputColor;
}

D3DCOLORVALUE stScene::readCubemap(stRay viewRay)
{
	D3DXCOLOR outputColor(0, 0, 0, 0);

	D3DLOCKED_RECT rect;
    ZeroMemory(&rect, sizeof(D3DLOCKED_RECT));
	HRESULT hr = objConfig->m_cubemap->texture->LockRect(0, &rect, NULL, D3DLOCK_READONLY);
	if (FAILED(hr)) {
		return outputColor;
	}

	if (screenTexture == NULL) {
		return outputColor;
	}

	if ((fabsf(viewRay.direction.x) >= fabsf(viewRay.direction.y))
		&& (fabsf(viewRay.direction.x) >= fabsf(viewRay.direction.z)))
	{
		if (viewRay.direction.x > 0.0f)
		{
			outputColor = readTexture(objConfig->m_cubemap->right,
					1.0f - (viewRay.direction.z / viewRay.direction.x + 1.0f) * 0.5f,
					(viewRay.direction.y/ viewRay.direction.x + 1.0f) * 0.5f,
					rect);
		}
		else if (viewRay.direction.x < 0.0f)
		{
			outputColor = readTexture(objConfig->m_cubemap->left,
					1.0f - (viewRay.direction.z / viewRay.direction.x + 1.0f) * 0.5f,
					1.0f - (viewRay.direction.y/ viewRay.direction.x + 1.0f) * 0.5f,
					rect);
		}
	}

	else if ((fabsf(viewRay.direction.y) >= fabsf(viewRay.direction.x))
		&& (fabsf(viewRay.direction.y) >= fabsf(viewRay.direction.z)))
	{
		if (viewRay.direction.y > 0.0f)
		{
			outputColor = readTexture(objConfig->m_cubemap->up,
					(viewRay.direction.x / viewRay.direction.y + 1.0f) * 0.5f,
					1.0f - (viewRay.direction.z/ viewRay.direction.y + 1.0f) * 0.5f,
					rect);
		}
		else if (viewRay.direction.y < 0.0f)
		{
			outputColor = readTexture(objConfig->m_cubemap->down,
					1.0f - (viewRay.direction.x / viewRay.direction.y + 1.0f) * 0.5f,
					(viewRay.direction.z/ viewRay.direction.y + 1.0f) * 0.5f,
					rect);
		}
	}

	else if ((fabsf(viewRay.direction.z) >= fabsf(viewRay.direction.y))
		&& (fabsf(viewRay.direction.z) >= fabsf(viewRay.direction.x)))
	{
		if (viewRay.direction.z > 0.0f)
		{
			outputColor = readTexture(objConfig->m_cubemap->forward,
					(viewRay.direction.x / viewRay.direction.z + 1.0f) * 0.5f,
					(viewRay.direction.y/ viewRay.direction.z + 1.0f) * 0.5f,
					rect);
		}
		else if (viewRay.direction.z < 0.0f)
		{
			outputColor = readTexture(objConfig->m_cubemap->backward,
					(viewRay.direction.x / viewRay.direction.z + 1.0f) * 0.5f,
					1.0f - (viewRay.direction.y/ viewRay.direction.z + 1.0f) * 0.5f,
					rect);
		}
	}

	objConfig->m_cubemap->texture->UnlockRect(0);
	
	if (objConfig->m_cubemap->get_bsRGB()) {
		// We make sure the data that was in sRGB storage mode is brought back to a
		// linear format. We don't need the full accuracy of the sRGBEncode function
		// so a powf should be sufficient enough.
		outputColor.b = powf(outputColor.b, 2.2f);
		outputColor.r = powf(outputColor.r, 2.2f);
		outputColor.g = powf(outputColor.g, 2.2f);
	}
	
	if (objConfig->m_cubemap->get_bExposed()) {
		// The LDR (low dynamic range) images were supposedly already
		// exposed, but we need to make the inverse transformation
		// so that we can expose them a second time.
		outputColor.b = -logf(1.001f - outputColor.b);
		outputColor.r = -logf(1.001f - outputColor.r);
		outputColor.g = -logf(1.001f - outputColor.g);
	}
	outputColor.b /= objConfig->m_cubemap->get_exposure();
	outputColor.r /= objConfig->m_cubemap->get_exposure();
	outputColor.g /= objConfig->m_cubemap->get_exposure();
	
	return outputColor;
}

float stScene::AutoExposure() {
	OutputDebugString(L"Start autoexposure calculating\n");
	//выдержка
	#define ACCUMULATION_SIZE 16
	float exposure = -1.0f;
	float accumulationFactor = float(max(stSceneSizeX, stSceneSizeY));
	accumulationFactor = accumulationFactor / ACCUMULATION_SIZE;
	float mediumPoint = 0;
	const float mediumPointWeight = 1.0f/(ACCUMULATION_SIZE*ACCUMULATION_SIZE);
	
	//проходимся по квадратной поверхности и генерируем лучи с учётом выдержки
	for (int y = 0; y < ACCUMULATION_SIZE; ++y)
	for (int x = 0; x < ACCUMULATION_SIZE; ++x) {
		stRay viewRay = stRay(D3DXVECTOR3(float(x) * accumulationFactor, float(y) * accumulationFactor, -1000.0f), D3DXVECTOR3(0.0f, 0.0f ,1.0f));
		D3DXCOLOR currentColor = addRay(viewRay);
		float luminance = 0.2126f * currentColor.r + 0.715160f * currentColor.g + 0.072169f * currentColor.b;
		mediumPoint += mediumPointWeight * (luminance * luminance);
	}
	float mediumLuminance = sqrtf(mediumPoint);
	if (mediumLuminance > 0.001f) {
		// put the medium luminance to an intermediate gray value
		exposure = logf(0.6f) / mediumLuminance;
	}
	OutputDebugString(L"End autoexposure calculating\n");
	return exposure;
}

D3DXCOLOR stScene::addRay(stRay viewRay) {
	
	D3DXCOLOR ambientColor(.2f, .2f, .2f, 1.0); //рассеяный свет
	D3DXCOLOR resPixelColor(0.0f, 0.0f, 0.0f, 0.0f);
	const int maxLevelReflection = 2;//уровень вложенност отражения луча
	int level = maxLevelReflection;//текущий уровень вложенности луча

	float coefReflect = 1.0f;//начальный коэффициет отражения
	
	//начинаем отражать луч наблюдения от всех объектов и собирать его цвета
	do
	{
		stSphere * curSphere = 0;
		float distance = 2000.0f; // ограничение для видимости сцены 
		// ищем пересечение луча со сферой
		for (std::vector<stSphere>::iterator i = listSpheres.begin(); i < listSpheres.end(); i++) { 
			float fDistance = 0;
			if ((*i).RayCollision(viewRay, fDistance) == false)
				continue;
			// пересекаются, запоминаем сферу 
			if (distance > fDistance) { 	
				curSphere = &(*i);
				distance = fDistance;
			}
		}
		//if (viewRay.startPoint.x == 200 && viewRay.startPoint.y == 1) ambientColor.a = 1.0f;

		//если не найдено ни одного пересечения, то получаем цвет текстуры из куба окружения или фоновый цвет и добавляем его
		if (curSphere == 0) {
			if (level == maxLevelReflection)
				resPixelColor = ambientColor;
			else 
				resPixelColor += readCubemap(viewRay);
			//выходим из цикла, т к луч уже не будет отражаться
			break;
		}
			
		D3DXVECTOR3 intPoint = viewRay.startPoint + distance * viewRay.direction; //точка пересечения в мировых координатах
		D3DXVECTOR3 normal = intPoint - curSphere->position; //нормаль к точке пересечения

		D3DXVec3Normalize(&viewRay.direction, &viewRay.direction); //нормализуем вектор луча
		D3DXVec3Normalize(&normal, &normal);//нормализуем нормаль
		stMaterial & curMaterial = listMaterials.at(curSphere->MaterialId);

		//изменяем нормаль в если это бумп поверхность
		if (curMaterial.bump != 0)
			normal = texture.SimpePerlin(curMaterial.bump, intPoint, normal);

		//добавляем цвет окружения к текущему пересечению сферы и луча
		resPixelColor.r += curMaterial.color.r * ambientColor.r;
		resPixelColor.g += curMaterial.color.g * ambientColor.g;
		resPixelColor.b += curMaterial.color.b * ambientColor.b;

		viewRay.direction = -viewRay.direction; //получаем направление в обратную сторону, т е от сферы к наблюдателю

		float dotViewNormal = D3DXVec3Dot(&viewRay.direction, &normal); //вычисляем стоит ли считать это пересечение
		if (dotViewNormal <= 0)
			break;

		//проходимся по всем источникам освещения в сцене
		for (std::vector<stLight>::iterator pPointLight = listPointLights.begin(); pPointLight < listPointLights.end(); pPointLight++) {
			D3DXVECTOR3 lightPos = (*pPointLight).position;
			D3DXVECTOR3 lightDirection = lightPos - intPoint; //вычисляем направление света в точке пересечения
			D3DXVec3Normalize(&lightDirection, &lightDirection);
			
			stSphere * curLightSphere = 0;
			float distance = 2000.0f;
			//вычисляем пересекает ли луч света одну из точку на этой сфере
			for (std::vector<stSphere>::iterator i = listSpheres.begin(); i < listSpheres.end(); i++) {
				float fDistance = 0;
				stRay lightRay = stRay((*pPointLight).position, -lightDirection);
				if ((*i).RayCollision(lightRay, fDistance) == false)
					continue;
				if (distance > fDistance) {	
					curLightSphere = &(*i);
					distance = fDistance;
				}
			}
			
			//если свет не светит на эту точку сферы
			if (curLightSphere != curSphere)
				continue;
				
			float dotLightNormal = D3DXVec3Dot(&lightDirection, &normal);
			if (dotLightNormal <= 0) //если не светит под острым углом 
				continue;
			
			//diffuse
			float lambert = coefReflect * dotLightNormal;
			resPixelColor += lambert * curMaterial.color * (*pPointLight).intensity;
				
			//add turbulent
			if (curMaterial.turbulence) {
				resPixelColor += texture.Turbulence(lambert, coefReflect, intPoint, curMaterial.color, curMaterial.color2, (*pPointLight).intensity);
				
			}

			//specular by Phong
			float phongTerm = 0;
			D3DXVECTOR3 reflectDir = 2 * normal * dotLightNormal - lightDirection;
			phongTerm = max(D3DXVec3Dot(&viewRay.direction, &reflectDir), 0.0f);
			phongTerm = curMaterial.specVal * powf(phongTerm, curMaterial.specPow) * coefReflect * (*pPointLight).intensity;
			resPixelColor.r += phongTerm;
			resPixelColor.g += phongTerm;
			resPixelColor.b += phongTerm;
		}

		//считаем новый отражённый луч 
		viewRay.startPoint = intPoint;
		viewRay.direction = 2 * normal * dotViewNormal - viewRay.direction;
		coefReflect *= curMaterial.reflectionCoef;
		level++;
	} while ((coefReflect > 0.0f) && (level <= 3));
	
	return resPixelColor;
}

//основная функция расчёта сцены
void stScene::calcObjects() {
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &d3dRenderSurface); //получаем поверхность буфера для рисования на ней

	if (d3dRenderSurface == NULL) 
		return;

	D3DLOCKED_RECT rect;
	D3DSURFACE_DESC d3dSurfDesc;
    d3dRenderSurface->LockRect(&rect, NULL, 0);
	d3dRenderSurface->GetDesc(&d3dSurfDesc);
	DWORD * bits = (DWORD *)rect.pBits; // получаем указатель на сырые данные буфера
	
	static const float autoExposure = AutoExposure();

	persp.typePersp = 1; // make it conic
	persp.clearPoint = 800;
	persp.dispersion = 5.0f;
	persp.FOV = 45.0f;
	float fTotalWeight;
	float projectionDistance = 0.0f;
	if (persp.typePersp == ScenePersp::conic && persp.FOV > 0.0f &&  persp.FOV < 189.0f) {
		projectionDistance = 0.5f * d3dSurfDesc.Width / tanf(0.5f*persp.FOV*M_PI_2);
	}

	//генерируем параллельные задания
	//#pragma omp parallel private(x, y)
	srand(GetTickCount());

	//проходим по всей поверхности и генерируем лучи
	OutputDebugString(L"Start scene calculating\n");
	for (UINT y = 0; y < d3dSurfDesc.Height; ++y)
	for (UINT x = 0; x < d3dSurfDesc.Width; ++x) {
		float R = 0.0f;
		float G = 0.0f;
		float B = 0.0f;
		
		D3DXCOLOR dxResColor(0,0,0,0);

		float fragmenty = (float)y;
		float fragmentx = (float)x; 

		//коэффициент суперсэмплинга
		
		float coefSupersampling = 1.0f;  {
		/*
		float  coefSupersampling = 0.25f;
		for(; fragmenty < y + 1.0f; fragmenty += 0.5f) 
		for(; fragmentx < x + 1.0f; fragmentx += 0.5f)  {
		*/

			if (persp.typePersp == ScenePersp::cubic) {
				D3DXVECTOR3 start(fragmentx, fragmenty, -1000.0f);
				D3DXVECTOR3 dir(0.0f, 0.0f, 1.0f);
				stRay viewRay = stRay(start, dir);
				dxResColor = addRay(viewRay);
			} else {
				fTotalWeight = 0.0f;
				D3DXVECTOR3 dir((fragmentx - 0.5f*d3dSurfDesc.Width)/projectionDistance,(fragmenty - 0.5f*d3dSurfDesc.Height)/projectionDistance, 1.0f);
				float norm = D3DXVec3Dot(&dir, &dir);
				if (norm == 0.0f)
					break;
				dir = dir/sqrt(norm);
				D3DXVECTOR3 start(0.5f*d3dSurfDesc.Width, 0.5f*d3dSurfDesc.Height, 0.0f);
				D3DXVECTOR3 ptAimed = start + persp.clearPoint*dir;
				int loop_max = 3; if (persp.dispersion == 0.0f) loop_max = 1;
				for (int i = 0; i < loop_max; ++i) {
					stRay viewRay = stRay(start, dir);
					if (persp.dispersion != 0.0f) {
						D3DXVECTOR3 vDistance((persp.dispersion/RAND_MAX)*float(rand()), (persp.dispersion/RAND_MAX)*float(rand()), 0.0f);
						viewRay.startPoint = viewRay.startPoint + vDistance;
						viewRay.direction = ptAimed - viewRay.startPoint;
						float norm = D3DXVec3Dot(&viewRay.direction, &viewRay.direction);
						if (norm == 0.0f)
							break;
						viewRay.direction = viewRay.direction/sqrt(norm);
					}
					D3DXCOLOR dxTmpColor = addRay(viewRay);
					fTotalWeight+=1.0f;
					dxResColor += dxTmpColor;
				}
				dxResColor = (1.0f/fTotalWeight) * dxResColor;
			}
			//после прогона луча суммируем его если включён суперсэмплинг
			R += dxResColor.r * coefSupersampling; 
			G += dxResColor.g * coefSupersampling; 
			B += dxResColor.b * coefSupersampling;
			}
			
			//exposure
			B = 1.0f - expf(B * autoExposure);
			R = 1.0f - expf(R * autoExposure);
			G = 1.0f - expf(G * autoExposure);
			
			//gamma correction
			R = srgbEncode(R);
			G = srgbEncode(G);
			B = srgbEncode(B);
			
			bits[y * rect.Pitch / 4 + x]  = D3DCOLOR_XRGB(min(UINT(R*255),255), min(UINT(G*255),255), min(UINT(B*255),255));
	}
	OutputDebugString(L"End scene calculating\n");
	d3dRenderSurface->UnlockRect();
	d3dRenderSurface->Release();
}