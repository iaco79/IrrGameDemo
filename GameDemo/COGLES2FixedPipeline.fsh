precision lowp float;
precision lowp int;

// Copyright (C) 2009-2010 Amundis
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// and OpenGL ES driver implemented by Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h
#define MAX_TEXTURE_UNITS 4
#define FOG_EXP 1
#define FOG_LINEAR 2

#define TwoD             24
#define Solid            0
#define Solid2Layer      1
#define LightMap         2
#define DetailMap		 9
#define SphereMap		 10
#define Reflection2Layer 11



#define TransparentAlphaChannel 13
#define TransparentVertexAlpha 15


lowp vec4  red   = vec4(1.0, 0.0, 0.0, 1.0);
lowp vec4  green = vec4(0.0, 1.0, 0.0, 1.0);
lowp vec4  blue  = vec4(0.0, 0.0, 1.0, 1.0);


/* Varyings */

varying lowp vec4 varTexCoord; //IACO [MAX_TEXTURE_UNITS];
varying lowp vec4 varVertexColor;


/* Uniforms */

uniform lowp int uRenderMode;

uniform bool  uAlphaTest;
uniform lowp float uAlphaValue;

/* Fog Uniforms */

uniform bool  uFog;
uniform int   uFogType;
uniform lowp vec4  uFogColor;
uniform float uFogStart;
uniform lowp float uFogEnd;
uniform lowp float uFogDensity;

/* Texture Uniforms */

uniform sampler2D uTextureUnit0;
uniform sampler2D uTextureUnit1;
uniform bool      uUseTexture; //IACO [MAX_TEXTURE_UNITS];

lowp vec4 render2D(void)
{
	vec4 color = varVertexColor;
	vec4 texel = texture2D(uTextureUnit0, varTexCoord.xy);
	//if(uUseTexture)
	color *= uUseTexture?texel:vec4(1.0,1.0,1.0,1.0);
	
	return color;
}

lowp vec4 renderSolid(void)
{
	lowp vec4 color = varVertexColor;
	lowp vec4 texel = texture2D(uTextureUnit0, varTexCoord.xy);
	
	color *= uUseTexture?texel:vec4(1.0,1.0,1.0,1.0);
		
	
	return color;
}

lowp vec4 renderTransparentVertexAlpha(void)
{
	lowp vec4 color = varVertexColor;
	color.a = varVertexColor.a;
	return color;
}

lowp vec4 renderTransparentAlphaChannel(void)
{
	lowp vec4 texel = texture2D(uTextureUnit0, varTexCoord.xy);
	
	lowp vec4 color = varVertexColor;

//	if(uUseTexture)
	{
		 color = texel * varVertexColor;				 		 
		 color.a = texel.a;
	}


	return color;
}



void main (void)
{
	
/*	 if(uRenderMode == TwoD)
		gl_FragColor = render2D();
  else if( uRenderMode == Solid)
		gl_FragColor = renderSolid();
	else if(uRenderMode == TransparentAlphaChannel)
		gl_FragColor = renderTransparentAlphaChannel();
 	else
*/
		gl_FragColor  =		(uRenderMode == TwoD) ? render2D() : 
                   		 (uRenderMode == Solid) ? renderSolid():  																							 		
                          (uRenderMode == TransparentAlphaChannel) ? renderTransparentAlphaChannel():
                             (uRenderMode == TransparentVertexAlpha)? renderTransparentVertexAlpha(): vec4(1.0,1.0,1.0,1.0);

		
	
}