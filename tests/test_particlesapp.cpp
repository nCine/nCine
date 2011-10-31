#include <cmath>
#include "ncTexture.h"
#include "ncParticleSystem.h"
#include "ncApplication.h"

static const int numParticles = 400;
ncTexture *pTexture = NULL;
ncParticleSystem *pParticleSys = NULL;
ncTimer *pTimer = NULL;
unsigned long int ulStartTime  = 0L;

int test_scene(ncApplication::eCommand cmd)
{
	switch(cmd)
	{
		case ncApplication::CMD_INIT:
		{
			ncSceneNode &rRootNode = ncApplication::RootNode();

#ifdef __ANDROID__
//			pTexture = new ncTexture("/sdcard/smoke.pkm");
//			ncTexture textureAlpha("/sdcard/smoke_red.pkm");
//			pTexture->SetAlphaFromRed(&textureAlpha);
//			if (ncApplication::GfxDevice().CheckGLExtension("GL_OES_framebuffer_object") == true)
//				ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, "test_scene - GL_OES_framebuffer_object is supported");
//			else
//				ncServiceLocator::GetLogger().Write(ncILogger::LOG_WARN, "test_scene - GL_OES_framebuffer_object is NOT supported");
			pTexture = new ncTexture("/sdcard/smoke.dds");
#else
			pTexture = new ncTexture("smoke.png");
		//	ncTexture textureAlpha("smoke_red.png");
		//	pTexture->SetAlphaFromRed(&textureAlpha);
#endif

			pParticleSys = new ncParticleSystem(numParticles, pTexture, 0.225f);
			pParticleSys->SetPosition(ncApplication::Width()*0.5f, ncApplication::Height()*0.33f);

		//	pParticleSys->AddAffector(new ncAccelerationAffector(0.000025f, 0.0f));
			ncColorAffector *colAffector = new ncColorAffector();
			colAffector->AddColorStep(0.0f, ncColor(0.86f, 0.39f, 0.0f, 0.05f));
			colAffector->AddColorStep(0.65f, ncColor(0.86f, 0.59f, 0.0f, 0.55f));
			colAffector->AddColorStep(0.7f, ncColor(0.86f, 0.7f, 0.0f, 0.295));
			colAffector->AddColorStep(1.0f, ncColor(0.0f, 0.0f, 1.0f, 0.59));
			pParticleSys->AddAffector(colAffector);
			ncSizeAffector *sizeAffector = new ncSizeAffector(0.25f);
			sizeAffector->AddSizeStep(0.0f, 0.01f);
			sizeAffector->AddSizeStep(0.7f, 1.6f);
			sizeAffector->AddSizeStep(1.0f, 0.4f);
			pParticleSys->AddAffector(sizeAffector);

			rRootNode.AddChildNode(pParticleSys);

	//		ncSprite *pSprite = new ncSprite(&rRootNode, pTexture);
	//		pSprite->SetPosition(128, 128);
	//		pSprite->SetScale(0.25f);

			pTimer = new ncTimer();
			pTimer->Reset();
			unsigned long int ulStartTime = pTimer->Now();
		}
			break;
		case ncApplication::CMD_FRAMESTART:
		{
			glEnable(GL_BLEND); // HACK: for alpha blending
			if (pTimer->Now() - ulStartTime > 150)
			{
				ulStartTime = pTimer->Now();
				pParticleSys->Emit(25, 3000, ncVector2f(0.0f, 0.1f));
			}
		}
			break;
		case ncApplication::CMD_SHUTDOWN:
		{
			delete pTimer;
			delete pParticleSys;
			delete pTexture;
		}
			break;
		default:
			break;
	}
}
