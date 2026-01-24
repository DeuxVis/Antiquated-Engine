#ifndef PARTICLE_GRAPHICS_H
#define	PARTICLE_GRAPHICS_H

#include "../RenderUtil/Sprites3D.h"


extern int		ParticleGraphicsCreate( const char* szTextureNum, float fGridScale, BOOL bUseRotation = FALSE, eSpriteGroupRenderFlags renderFlags = kSpriteRender_Default, int layer = 0 );

extern int		ParticleGraphicsCreateHandle( int hTex, float fGridScale, BOOL bUseRotation = FALSE, eSpriteGroupRenderFlags renderFlags = kSpriteRender_Default, int layer = 0 );

extern SPRITE_GROUP		ParticleGraphicsGetSpriteGroup( int nParticleGraphicID );

extern int		ParticleGraphicsGetTextureHandle(int nParticleGraphicID);

extern void		ParticleGraphicsShutdown( );

extern int		ParticleGraphicsGetNumActiveSpriteGroups();
#endif