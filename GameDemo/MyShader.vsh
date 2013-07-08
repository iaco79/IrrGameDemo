#ifdef GL_ES
precision highp float;
precision highp int;
#endif

attribute vec4 inVertexPosition;
attribute vec4 inVertexColor;
attribute vec4 inTexCoord0;
attribute vec3 inVertexNormal;

//attribute vec4 inTexCoord1;


uniform int uRenderMode;

/* Matrix Uniforms */

uniform mat4 uMvpMatrix;
uniform mat4 uWorldMatrix;
uniform float  uTimer;



 

varying vec4 varVertexColor;
varying vec4 varTexCoord;

vec4 gColor;

/* shader variables */




void main(void)
{
	
	float tt = clamp(uTimer/100.0,0.01,0.9)*inVertexNormal.z;
	
	vec4 speed= vec4(inVertexNormal.x,inVertexNormal.y,1.0,1.0) ;
		
	
	
	
	vec4 vp =  inVertexPosition 
		    - inVertexPosition * vec4(tt,tt,0.0,0.0)
			+ speed*vec4(uTimer,uTimer,1.0,1.0);
			 
			 
	
	 
	 
	gColor = inVertexColor.bgra;
	
	
	gl_Position = uMvpMatrix * vp;
	
	
	varTexCoord = inTexCoord0;
	varVertexColor.rgb  = clamp(gColor.rgb, 0.0 , 1.0);
		 
	varVertexColor.a = gColor.a;
	
	
	
	
}
