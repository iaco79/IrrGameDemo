// Copyright (C) 2009-2010 Amundis
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// and OpenGL ES driver implemented by Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h
#define MAX_TEXTURE_UNITS 1
#define MAX_LIGHTS 1
precision lowp float;
precision lowp int;

#define SphereMap        10
#define Reflection2Layer 11

const vec4 red   = vec4(1.0, 0.0, 0.0, 1.0);
const vec4 green = vec4(0.0, 1.0, 0.0, 1.0);
const vec4 blue  = vec4(0.0, 0.0, 1.0, 1.0);
const vec4 white = vec4(1.0);
const vec4 black = vec4(0.0);

/* Vertex Attributes */
 
attribute lowp vec4 inVertexPosition;
attribute lowp vec4 inVertexColor;
attribute lowp vec4 inTexCoord0;
attribute lowp vec4 inTexCoord1;
attribute lowp vec3 inVertexNormal;

uniform int uRenderMode;

/* Matrix Uniforms */

uniform lowp mat4 uMvpMatrix;
uniform lowp mat4 uWorldMatrix;

uniform bool uNormalize;
uniform lowp vec3 uEyePos;

/* Light Uniforms */

uniform bool  uUseLight; //IACO         [MAX_LIGHTS];
uniform lowp vec4  uLightPosition; //IACO    [MAX_LIGHTS];
uniform lowp vec4  uLightAmbient; //IACO     [MAX_LIGHTS];
uniform lowp vec4  uLightDiffuse; //IACO     [MAX_LIGHTS];
#ifdef USE_SPECULAR
uniform lowp vec4  uLightSpecular; //IACO    [MAX_LIGHTS];
#endif
#ifdef USE_LIGHT_CUTOFF
uniform lowp vec3  uLightDirection; //IACO   [MAX_LIGHTS];
uniform float uLightCutoff; //IACO      [MAX_LIGHTS];
uniform float uLightExponent; //IACO    [MAX_LIGHTS];
#endif
uniform vec3  uLightAttenuation; //IACO [MAX_LIGHTS];
uniform vec4  uAmbientColor;
uniform bool  uLighting;

/* Material Uniforms */
uniform lowp vec4  uMaterialAmbient;
uniform lowp vec4  uMaterialEmission;
uniform lowp vec4  uMaterialDiffuse;
uniform lowp vec4  uMaterialSpecular;
uniform lowp float uMaterialShininess;
uniform int   uColorMaterial;

#define ECM_NONE     0
#define ECM_DIFFUSE  1
#define ECM_AMBIENT  2
#define ECM_EMISSIVE 3
#define ECM_SPECULAR 4
#define ECM_DIFFUSE_AND_AMBIENT 5

/* Texture Uniforms */
uniform bool uUseTexture; //IACO    [MAX_TEXTURE_UNITS];
uniform lowp mat4 uTextureMatrix; //IACO [MAX_TEXTURE_UNITS];
uniform bool uUseTexMatrix; //IACO  [MAX_TEXTURE_UNITS];

/* Clip Plane Uniforms */
uniform bool uClip;
uniform lowp vec4 uClipPlane;
varying lowp float varClipDist;

/* Varyings */

varying lowp vec4 varTexCoord; //IACO[MAX_TEXTURE_UNITS];
varying lowp vec4 varVertexColor;
varying float eyeDist;

/* shader variables */

lowp vec3 gNormal;
lowp vec3 gWorldPos;
lowp vec4 gColor;

struct material {
  lowp vec4   Ambient;
  lowp vec4   Diffuse;
  lowp vec4   Specular;
  lowp vec4   Emission;
  lowp float   Shininess;
} ;

material gMaterial;

lowp vec4 lightEquation(int lidx)
{		
	vec4 color = vec4(0.0);
	
	float att = 1.0;
	vec3 lightDir;
	
	
	if( uLightPosition.w == 0.0) // Directional light
	{
		lightDir = -uLightPosition.xyz;
	}
	else
	{
		lightDir = uLightPosition.xyz - inVertexPosition.xyz;
		att = 1.0 / (uLightAttenuation.y * length(lightDir));
		lightDir = normalize(lightDir);
		
#ifdef USE_LIGHT_CUTOFF
		if(uLightCutoff < 180.0)
		{
			// compute spot factor
			float spotEffect = dot(-lightDir, uLightDirection);
			if( spotEffect >= cos( radians( uLightCutoff)))
				spotEffect = pow( spotEffect, uLightExponent);
			else
				spotEffect = 0.0;
			att *= spotEffect;
		}
#endif
	}
	
	if(att >= 0.0 )
	{
		color += uLightAmbient * gMaterial.Ambient;
		
		//Compute cos(Light, Normal)
		float NdotL = max(dot(normalize(gNormal), lightDir), 0.0);
		color += NdotL * uLightDiffuse * gMaterial.Diffuse;
		
		//Compute cos(hvec, Normal)
#ifdef USE_SPECULAR
		vec3 hvec = normalize(lightDir + vec3(0.0, 0.0, 1.0));
		float NdotH = dot(gNormal, hvec);
		if(NdotH > 0.0)
		{
			color += pow(NdotH, uMaterialShininess) * uLightSpecular * gMaterial.Specular;
		}
#endif
		color *= att;
	}
	return color;
}

vec4 computeLighting(void)
{
	vec4 color = gMaterial.Emission + gMaterial.Ambient * uAmbientColor;

	//for ( int i = 0 ; i < MAX_LIGHTS; ++i)
	{
		if ( uUseLight )
		{
			color += lightEquation(0) ;
		}
	}
	color.a = gMaterial.Diffuse.a;
	return color;
}

void ReplaceColorMaterial(void)
{
	gMaterial.Ambient = uMaterialAmbient;
	gMaterial.Diffuse = uMaterialDiffuse;
	gMaterial.Emission = uMaterialEmission;
	gMaterial.Specular = uMaterialSpecular;
	gMaterial.Shininess = uMaterialShininess;

	if(uColorMaterial == ECM_DIFFUSE)
		gMaterial.Diffuse = gColor;
	else if(uColorMaterial == ECM_AMBIENT)
		gMaterial.Ambient = gColor;
	else if(uColorMaterial == ECM_DIFFUSE_AND_AMBIENT)
	{
		gMaterial.Diffuse = gColor;
		gMaterial.Ambient = gColor;
	}	
	else if(uColorMaterial == ECM_EMISSIVE)
		gMaterial.Emission = gColor;
	else if(uColorMaterial == ECM_SPECULAR)
		gMaterial.Specular = gColor;
}

void main(void)
{
	gl_Position = uMvpMatrix * inVertexPosition;
	
	gWorldPos = (uWorldMatrix * inVertexPosition).xyz;
	
	gColor = inVertexColor.bgra;

	varVertexColor = gColor;

		
	
	varTexCoord = vec4(0.0);
	
	varTexCoord = inTexCoord0;
	
	if(uUseTexMatrix)
	{
		varTexCoord = uTextureMatrix  * vec4(inTexCoord0.x,inTexCoord0.y,0.0,1.0);
	}
	
	varVertexColor.rgb  = clamp(varVertexColor.rgb, 0.0, 1.0);
	varVertexColor.a = gColor.a;
}

 
