#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/vec4.hpp>
#include <glm/geometric.hpp>
#include "stScene.h"

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

stScene::stScene(int width, int height)
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

	ambientColor.r = 0.3f;
	ambientColor.g = 0.3f;
	ambientColor.b = 0.3f;
//	D3DXCreateFont(d3ddev, 24, 0, 500, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,    DEFAULT_PITCH | FF_DONTCARE, L"Arial", &font);
	FPS = 0.0f;
	FrameCnt = 0;
	TimeElapsed = 0;

	specularEnabled = true;
	reflectionEnabled = true;

	objConfig = std::unique_ptr<ConfigLoader>(new ConfigLoader("/tmp/config.txt"));
	listPointLights = objConfig->GetLights();
	listMaterials = objConfig->GetMaterials();
	listSpheres = objConfig->GetSpheres();

	//init cubic environment
//	HRESULT hr = D3DXCreateTextureFromFile(d3ddev, L"res\\cubictexture_2.jpg", &(objConfig->m_cubemap->texture));
//	if (FAILED(hr)) {
//		std::cerr<<"Error while reading cubictexture.jpeg"<<std::endl;
//		return;
//	}


//	D3DSURFACE_DESC texDesc;
//	objConfig->m_cubemap->texture->GetLevelDesc(0, &texDesc);
	
	//texture FVF FORMAT is unsigned int X8R8G8B8
	//real texture size after load into DIRECTX (128x1024 (H*W) instead of 100x600)
	//div by 6 because of 6 edges of texture
//	objConfig->m_cubemap->h = texDesc.Height/6;
//	objConfig->m_cubemap->w = texDesc.Width;
	
	/*
	//debug code begin
	IDirect3DTexture9 * textToSave;
	hr = D3DXCreateTexture(d3ddev, texDesc.Width, texDesc.Height, 1, D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &textToSave);

	D3DLOCKED_RECT srcRect;
    ZeroMemory(&srcRect, sizeof(D3DLOCKED_RECT));
	
	D3DLOCKED_RECT dstRect;
    ZeroMemory(&dstRect, sizeof(D3DLOCKED_RECT));

	hr = objConfig->m_cubemap->texture->LockRect(0, &srcRect, null, D3DLOCK_READONLY);
	hr = textToSave->LockRect(0, &dstRect, null, D3DLOCK_READONLY);

	int selectedPart = 1;
	int texturePartWidth = texDesc.Width/6;
	for (int y = 0; y < texturePartWidth; y++)
		for (int x = 0; x < texDesc.Height; x++) {
			((unsigned int *)dstRect.pBits)[y + x * (dstRect.Pitch/4)] = ((unsigned int *)srcRect.pBits)[y + x * (srcRect.Pitch/4) + texturePartWidth * selectedPart];
		}
	objConfig->m_cubemap->texture->UnlockRect(0);
	textToSave->UnlockRect(0);
	hr = D3DXSaveTextureToFile(L"res\\debugtexture.jpg", D3DXIFF_JPG, textToSave, null);
	textToSave->Release();
	//debug code end
	*/
}

stScene::~stScene() {
	for (int i = 0; i < stSceneSizeY; ++i)
		delete rayArray[i];
	delete [] rayArray;
	listSpheres.clear();
}

void stScene::KeyPress(unsigned int  key)
{
	/*
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
	*/
}

void stScene::UpdateObjects()
{
}

void stScene::DrawObjects()
{	
	//unsigned int oldTime = timeGetTime();

	// calcObjects();
	//drawFPS();

	//calcFPS(oldTime);
}

/*
void stScene::calcFPS(unsigned int oldTime)
{
      FrameCnt++;
      unsigned int newTime = timeGetTime(); // çàïèñûâàåì â íüþòàéì âðåìÿ ïîñëå âñåõ âûïîëíåííûõ îïåðàöèÿõ
      unsigned int deltatime = newTime - oldTime; // óçíàåì ñêîëüêî âðåìåíè ïðîõîäèò îò íà÷àëà äî êîíöà
      TimeElapsed += deltatime; // è ïðèáàâëÿåì ýòó ðàçíèöó ê òàéìýëàïñåäó
 
      if(TimeElapsed >= 1000) // åñëè â òàéìýëàïñåäå íàêîïèëîñü 0.5 ñåê, òî
      {
        FPS = (1000.0f * FrameCnt) / TimeElapsed; // äåëèì êîë-âî êàäðîâ ïðîøåäøåå çà ïîëñåêóíäû íà òàéìýëàïñåä(ðàâíûé ïðèìåðíî 0.5 ñåê)
        TimeElapsed = 0; // îáíóëÿåì òàéìýëàïñåä, äëÿ ñëåäóþùåãî ïîäñ÷åòà
        FrameCnt = 0; // è îáíóëÿåì êîë-âî êàäðîâ
      }
}
*/
void stScene::drawFPS()
{
	//wchar_t textbuffer[255];
    //swprintf( textbuffer, L"FPS: %.3f ", FPS);
    
	//RECT FontPosition;
    //FontPosition.top    = 10; 
    //FontPosition.left   = 10;
    //FontPosition.right  = 610;
    //FontPosition.bottom = 30;
    //font->DrawText( null, textbuffer, -1, &FontPosition, DT_LEFT, 0xFFFFFF80 );
}
/*
glm::vec3 stScene::readTexture(int iTileTex, float uCoord, float vCoord, const D3DLOCKED_RECT & rect)
{
	float sizeU = (float)objConfig->m_cubemap->w;
	float sizeV = (float)objConfig->m_cubemap->h;
	glm::vec3 outputColor;
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

	//texture FVF FORMAT is unsigned int X8R8G8B8
	//real texture size after load into DIRECTX (1024x128 (H*W) instead of 600x100)
	unsigned int * dwColor = (unsigned int *)rect.pBits + rect.Pitch/4*objConfig->m_cubemap->h*iTileTex;
	//÷òîáû ïðîèçâåñòè ìàòåìàòè÷åñêèå îïåðàöèè ñ öâåòîì äåëàåì èç DWORD "ïðàâèëüíóþ" ñòðóêòóðó
	glm::vec3 colorP0(dwColor[umin + (rect.Pitch/4) * vmin]);
	glm::vec3 colorP1(dwColor[umax + (rect.Pitch/4) * vmin]);
	glm::vec3 colorP2(dwColor[umin + (rect.Pitch/4) * vmax]);
	glm::vec3 colorP3(dwColor[umax + (rect.Pitch/4) * vmax]);
	
	///D3DCOLOR P0 = ((unsigned int *)rect.pBits)[umin + (rect.Pitch/4) * vmin + (rect.Pitch/4)/6 * iTileTex];
	///D3DCOLOR P1 = ((unsigned int *)rect.pBits)[umax + (rect.Pitch/4) * vmin + (rect.Pitch/4)/6 * iTileTex];
	///D3DCOLOR P2 = ((unsigned int *)rect.pBits)[umin + (rect.Pitch/4) * vmax + (rect.Pitch/4)/6 * iTileTex];
	///D3DCOLOR P3 = ((unsigned int *)rect.pBits)[umax + (rect.Pitch/4) * vmax + (rect.Pitch/4)/6 * iTileTex];
	
	outputColor = ((1 - u) * colorP0 + u * colorP1) * (1-v) + ((1 - u) * colorP2 + u * colorP3) * v;
	
	return outputColor;
}

glm::vec3 stScene::readCubemap(stRay viewRay)
{
	glm::vec3 outputColor(0, 0, 0, 0);

	D3DLOCKED_RECT rect;
    ZeroMemory(&rect, sizeof(D3DLOCKED_RECT));
	HRESULT hr = objConfig->m_cubemap->texture->LockRect(0, &rect, null, D3DLOCK_READONLY);
	if (FAILED(hr)) {
		return outputColor;
	}

	if (screenTexture == null) {
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
*/
float stScene::AutoExposure() {
	//âûäåðæêà
	#define ACCUMULATION_SIZE 16
	float exposure = -1.0f;
	float accumulationFactor = float(max(stSceneSizeX, stSceneSizeY));
	accumulationFactor = accumulationFactor / ACCUMULATION_SIZE;
	float mediumPoint = 0;
	const float mediumPointWeight = 1.0f/(ACCUMULATION_SIZE*ACCUMULATION_SIZE);
	
	//ïðîõîäèìñÿ ïî êâàäðàòíîé ïîâåðõíîñòè è ãåíåðèðóåì ëó÷è ñ ó÷¸òîì âûäåðæêè
	for (int y = 0; y < ACCUMULATION_SIZE; ++y)
	for (int x = 0; x < ACCUMULATION_SIZE; ++x) {
		stRay viewRay = stRay(glm::vec3(float(x) * accumulationFactor, float(y) * accumulationFactor, -1000.0f), glm::vec3(0.0f, 0.0f ,1.0f));
		glm::vec4 currentColor = addRay(viewRay);
		float luminance = 0.2126f * currentColor.r + 0.715160f * currentColor.g + 0.072169f * currentColor.b;
		mediumPoint += mediumPointWeight * (luminance * luminance);
	}
	float mediumLuminance = sqrtf(mediumPoint);
	if (mediumLuminance > 0.001f) {
		// put the medium luminance to an intermediate gray value
		exposure = logf(0.6f) / mediumLuminance;
	}
	return exposure;
}

glm::vec4 stScene::addRay(stRay viewRay) {
	
	glm::vec4 ambientColor(.2f, .2f, .2f, 1.0);
	glm::vec4 resPixelColor(0.0f, 0.0f, 0.0f, 0.0f);
	const int maxLevelReflection = 2;
	int level = maxLevelReflection;

	float coefReflect = 1.0f;
	
	//íà÷èíàåì îòðàæàòü ëó÷ íàáëþäåíèÿ îò âñåõ îáúåêòîâ è ñîáèðàòü åãî öâåòà
	do
	{
		stSphere * curSphere = 0;
		float distance = 2000.0f; // îãðàíè÷åíèå äëÿ âèäèìîñòè ñöåíû 
		// èùåì ïåðåñå÷åíèå ëó÷à ñî ñôåðîé
		for (std::vector<stSphere>::iterator i = listSpheres.begin(); i < listSpheres.end(); i++) { 
			float fDistance = 0;
			if ((*i).RayCollision(viewRay, fDistance) == false)
				continue;
			// ïåðåñåêàþòñÿ, çàïîìèíàåì ñôåðó 
			if (distance > fDistance) { 	
				curSphere = &(*i);
				distance = fDistance;
			}
		}
		//if (viewRay.startPoint.x == 200 && viewRay.startPoint.y == 1) ambientColor.a = 1.0f;

		//åñëè íå íàéäåíî íè îäíîãî ïåðåñå÷åíèÿ, òî ïîëó÷àåì öâåò òåêñòóðû èç êóáà îêðóæåíèÿ èëè ôîíîâûé öâåò è äîáàâëÿåì åãî
		if (curSphere == 0) {
			if (level == maxLevelReflection)
				resPixelColor = ambientColor;
			else 
				// STADNIK 2016 
				// resPixelColor += readCubemap(viewRay);
				resPixelColor = glm::vec4(1,1,1,1);
				// DEFAULT COLOR
			//âûõîäèì èç öèêëà, ò ê ëó÷ óæå íå áóäåò îòðàæàòüñÿ
			break;
		}
			
		glm::vec3 intPoint = viewRay.startPoint + distance * viewRay.direction; //òî÷êà ïåðåñå÷åíèÿ â ìèðîâûõ êîîðäèíàòàõ
		glm::vec3 normal = intPoint - curSphere->position; //íîðìàëü ê òî÷êå ïåðåñå÷åíèÿ

		viewRay.direction = glm::normalize(viewRay.direction); //íîðìàëèçóåì âåêòîð ëó÷à
		normal = glm::normalize(normal);//íîðìàëèçóåì íîðìàëü
		stMaterial & curMaterial = listMaterials.at(curSphere->MaterialId);

		//èçìåíÿåì íîðìàëü â åñëè ýòî áóìï ïîâåðõíîñòü
		if (curMaterial.bump != 0)
			normal = texture.SimpePerlin(curMaterial.bump, intPoint, normal);

		//äîáàâëÿåì öâåò îêðóæåíèÿ ê òåêóùåìó ïåðåñå÷åíèþ ñôåðû è ëó÷à
		resPixelColor.r += curMaterial.color.r * ambientColor.r;
		resPixelColor.g += curMaterial.color.g * ambientColor.g;
		resPixelColor.b += curMaterial.color.b * ambientColor.b;

		viewRay.direction = -viewRay.direction; //ïîëó÷àåì íàïðàâëåíèå â îáðàòíóþ ñòîðîíó, ò å îò ñôåðû ê íàáëþäàòåëþ

		float dotViewNormal = glm::dot(viewRay.direction, normal); //âû÷èñëÿåì ñòîèò ëè ñ÷èòàòü ýòî ïåðåñå÷åíèå
		if (dotViewNormal <= 0)
			break;

		//ïðîõîäèìñÿ ïî âñåì èñòî÷íèêàì îñâåùåíèÿ â ñöåíå
		for (std::vector<stLight>::iterator pPointLight = listPointLights.begin(); pPointLight < listPointLights.end(); pPointLight++) {
			glm::vec3 lightPos = (*pPointLight).position;
			glm::vec3 lightDirection = lightPos - intPoint; //âû÷èñëÿåì íàïðàâëåíèå ñâåòà â òî÷êå ïåðåñå÷åíèÿ
			lightDirection = glm::normalize(lightDirection);
			
			stSphere * curLightSphere = 0;
			float distance = 2000.0f;
			//âû÷èñëÿåì ïåðåñåêàåò ëè ëó÷ ñâåòà îäíó èç òî÷êó íà ýòîé ñôåðå
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
			
			//åñëè ñâåò íå ñâåòèò íà ýòó òî÷êó ñôåðû
			if (curLightSphere != curSphere)
				continue;
				
			float dotLightNormal = glm::dot(lightDirection, normal);
			if (dotLightNormal <= 0) //åñëè íå ñâåòèò ïîä îñòðûì óãëîì 
				continue;
			
			//diffuse
			float lambert = coefReflect * dotLightNormal;
			resPixelColor += lambert * curMaterial.color * (*pPointLight).intensity;
				
			//add turbulent
			if (curMaterial.turbulence) {
				glm::vec3 tmpTurbulenceColor = texture.Turbulence(lambert, coefReflect, intPoint, curMaterial.color, curMaterial.color2, (*pPointLight).intensity);
				resPixelColor.r += tmpTurbulenceColor.r;
				resPixelColor.g += tmpTurbulenceColor.g;
				resPixelColor.b += tmpTurbulenceColor.b;
			}

			//specular by Phong
			float phongTerm = 0;
			glm::vec3 reflectDir = 2.0f * normal * dotLightNormal - lightDirection;
			phongTerm = max(glm::dot(viewRay.direction, reflectDir), 0.0f);
			phongTerm = curMaterial.specVal * powf(phongTerm, curMaterial.specPow) * coefReflect * (*pPointLight).intensity;
			resPixelColor.r += phongTerm;
			resPixelColor.g += phongTerm;
			resPixelColor.b += phongTerm;
		}

		//ñ÷èòàåì íîâûé îòðàæ¸ííûé ëó÷ 
		viewRay.startPoint = intPoint;
		viewRay.direction = 2.0f * normal * dotViewNormal - viewRay.direction;
		coefReflect *= curMaterial.reflectionCoef;
		level++;
	} while ((coefReflect > 0.0f) && (level <= 3));
	
	return resPixelColor;
}

//îñíîâíàÿ ôóíêöèÿ ðàñ÷¸òà ñöåíû
float * stScene::calcPixels(unsigned int screen_width, unsigned int screen_height) {

	static float* pixels = new float[screen_width*screen_width*3];
	
	static const float autoExposure = AutoExposure();

	persp.typePersp = ScenePersp::cubic; // make it conic
	persp.clearPoint = 800;
	persp.dispersion = 5.0f;
	persp.FOV = 45.0f;
	float fTotalWeight;
	float projectionDistance = 0.0f;
	if (persp.typePersp == ScenePersp::conic && persp.FOV > 0.0f &&  persp.FOV < 189.0f) {
		projectionDistance = 0.5f * screen_width / tanf(0.5f*persp.FOV*M_PI_2);
	}

	//ãåíåðèðóåì ïàðàëëåëüíûå çàäàíèÿ
	//#pragma omp parallel private(x, y)
	srand(time(0));

	//ïðîõîäèì ïî âñåé ïîâåðõíîñòè è ãåíåðèðóåì ëó÷è
	for (unsigned int  y = 0; y < screen_height; ++y)
	for (unsigned int  x = 0; x < screen_width; ++x) {
		float R = 0.0f;
		float G = 0.0f;
		float B = 0.0f;
		
		glm::vec3 dxResColor(0,0,0);

		float fragmenty = (float)y;
		float fragmentx = (float)x; 

		//êîýôôèöèåíò ñóïåðñýìïëèíãà
		
		float coefSupersampling = 1.0f;  {
		/*
		float  coefSupersampling = 0.25f;
		for(; fragmenty < y + 1.0f; fragmenty += 0.5f) 
		for(; fragmentx < x + 1.0f; fragmentx += 0.5f)  {
		*/

			if (persp.typePersp == ScenePersp::cubic) {
				glm::vec3 start(fragmentx, fragmenty, -1000.0f);
				glm::vec3 dir(0.0f, 0.0f, 1.0f);
				stRay viewRay = stRay(start, dir);
				dxResColor = addRay(viewRay);
			} else {
				fTotalWeight = 0.0f;
				glm::vec3 dir((fragmentx - 0.5f*screen_width)/projectionDistance,(fragmenty - 0.5f*screen_height)/projectionDistance, 1.0f);
				float norm = glm::dot(dir, dir);
				if (norm == 0.0f)
					break;
				dir = dir/sqrt(norm);
				glm::vec3 start(0.5f*screen_width, 0.5f*screen_height, 0.0f);
				glm::vec3 ptAimed = start + persp.clearPoint*dir;
				int loop_max = 3; if (persp.dispersion == 0.0f) loop_max = 1;
				for (int i = 0; i < loop_max; ++i) {
					stRay viewRay = stRay(start, dir);
					if (persp.dispersion != 0.0f) {
						glm::vec3 vDistance((persp.dispersion/RAND_MAX)*float(rand()), (persp.dispersion/RAND_MAX)*float(rand()), 0.0f);
						viewRay.startPoint = viewRay.startPoint + vDistance;
						viewRay.direction = ptAimed - viewRay.startPoint;
						float norm = glm::dot(viewRay.direction, viewRay.direction);
						if (norm == 0.0f)
							break;
						viewRay.direction = viewRay.direction/sqrt(norm);
					}
					glm::vec3 dxTmpColor = addRay(viewRay);
					fTotalWeight+=1.0f;
					dxResColor += dxTmpColor;
				}
				dxResColor = (1.0f/fTotalWeight) * dxResColor;
				
				
			}
			//ïîñëå ïðîãîíà ëó÷à ñóììèðóåì åãî åñëè âêëþ÷¸í ñóïåðñýìïëèíã
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
			
			// write finel results to array
			pixels[y*screen_width+x+0] = std::min(R, 1.0f);
			pixels[y*screen_width+x+1] = std::min(G, 1.0f);
			pixels[y*screen_width+x+2] = std::min(B, 1.0f);
	}
	
	return pixels;
}