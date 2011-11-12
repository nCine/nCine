#include <cmath>
#include "ncTexture.h"
#include "ncParticleSystem.h"
#include "ncApplication.h"
#include "ncFont.h"
#include "ncTextNode.h"

static const int numParticles = 400;
ncTexture *pTexture = NULL;
ncParticleSystem *pParticleSys = NULL;
ncTimer *pTimer = NULL;
unsigned long int ulEmitTime  = 0L;
ncFont *pFont = NULL;
ncTextNode *pFpsText = NULL;
char vFPS[16];
unsigned long int ulUpdateFpsTime  = 0L;

int test_scene(ncApplication::eCommand cmd)
{
	switch(cmd)
	{
		case ncApplication::CMD_INIT:
		{
			ncSceneNode &rRootNode = ncApplication::RootNode();

#ifdef __ANDROID__
			pFont = new ncFont("/sdcard/ncine/trebuchet16_128.dds", "/sdcard/ncine/trebuchet16_128.fnt");
			pTexture = new ncTexture("/sdcard/ncine/smoke_128.dds");
#else
			pFont = new ncFont("trebuchet32_256.png", "trebuchet32_256.fnt");
			pTexture = new ncTexture("smoke_256.png");
		//	ncTexture textureAlpha("smoke_256_red.png");
		//	pTexture->SetAlphaFromRed(&textureAlpha);
#endif

			pFpsText = new ncTextNode(&rRootNode, pFont);
			pFpsText->SetString("FPS: ");
			pFpsText->SetPosition((ncApplication::Width() - pFpsText->XAdvanceSum()), ncApplication::Height() - pFpsText->FontBase());

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

			pTimer = new ncTimer();
			pTimer->Reset();
			unsigned long int ulStartTime = pTimer->Now();
		}
			break;
		case ncApplication::CMD_FRAMESTART:
		{
			glEnable(GL_BLEND); // HACK: for alpha blending

			if (pTimer->Now() - ulUpdateFpsTime > 100)
			{
				ulUpdateFpsTime = pTimer->Now();
				sprintf(vFPS, (const char *)"FPS: %.0f", ncApplication::AverageFPS());
				pFpsText->SetString(vFPS);
				pFpsText->SetPosition((ncApplication::Width() - pFpsText->XAdvanceSum()), ncApplication::Height() - pFpsText->FontBase());
			}

			if (pTimer->Now() - ulEmitTime > 150)
			{
				ulEmitTime = pTimer->Now();
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
