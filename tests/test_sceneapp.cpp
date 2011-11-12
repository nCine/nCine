#include <cmath>
#include "ncTexture.h"
#include "ncSprite.h"
#include "ncApplication.h"

static const int numTextures = 4;
static const int numSprites = 8;
float fAngles[numTextures] = { 0.0f, 0.0f, 0.0f, 0.0f};
ncTexture *pTextures[numTextures] = {NULL, NULL, NULL, NULL};
ncSprite *pSprites[numSprites];

int test_scene(ncApplication::eCommand cmd)
{
	switch(cmd)
	{
		case ncApplication::CMD_INIT:
		{
			ncSceneNode &rRootNode = ncApplication::RootNode();

#ifdef __ANDROID__
			pTextures[0] = new ncTexture("/sdcard/ncine/texture1.pkm"); // 145x121
			pTextures[1] = new ncTexture("/sdcard/ncine/texture2.pkm"); // 100x100
			pTextures[2] = new ncTexture("/sdcard/ncine/texture3.pkm"); // 96x96
			pTextures[3] = new ncTexture("/sdcard/ncine/texture4.pkm"); // 96x96
#else
			pTextures[0] = new ncTexture("texture1.png");
			pTextures[1] = new ncTexture("texture2.png");
			pTextures[2] = new ncTexture("texture3.png");
			pTextures[3] = new ncTexture("texture4.png");
#endif

			pSprites[0] = new ncSprite(pTextures[0], 0, 0);
			pSprites[0]->SetTexRect(ncRect(40, 0, pSprites[0]->Width()-80, pSprites[0]->Height()-30));
			pSprites[0]->SetScale(0.75f);
			rRootNode.AddChildNode(pSprites[0]);

			for (int i = 1; i < numSprites; i++)
			{
				pSprites[i] = new ncSprite(pSprites[i-1], pTextures[i%numTextures], 0, 0);
				pSprites[i]->SetScale(0.5f);
			}
		}
			break;
		case ncApplication::CMD_FRAMESTART:
		{
			float fSinus[numTextures];
			float fCosine[numTextures];

			for (int i = 0; i < numTextures; i++)
			{
				fAngles[i] += (0.25f + 0.025f*i) * ncApplication::Interval();
				fSinus[i] = sinf(fAngles[i] * 0.01f);
				fCosine[i] = cosf(2*fAngles[i] * 0.01f);
			}

			pSprites[0]->x = ncApplication::Width()*0.5f + fSinus[0]*100;
			pSprites[0]->y = ncApplication::Height()*0.5f + fCosine[0]*50;

			for (int i = 1; i < numSprites; i++)
			{
				pSprites[i]->x = fSinus[i%numTextures]*50;
				pSprites[i]->y = fCosine[i%numTextures]*50;
			}
		}
			break;
		case ncApplication::CMD_SHUTDOWN:
		{
			delete pSprites[0]; // and all its children
			for (int i = 0; i < numTextures; i++)
				delete pTextures[i];
		}
			break;
		default:
			break;
	}
}
