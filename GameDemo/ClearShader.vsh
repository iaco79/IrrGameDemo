#ifdef GL_ES
precision highp float;
precision highp int;
#endif

attribute vec4 inVertexPosition;
attribute vec4 inVertexColor;
attribute vec4 inTexCoord0;
attribute vec3 inVertexNormal;

uniform mat4 uMvpMatrix;
uniform mat4 uWorldMatrix;


varying vec4 varVertexColor;
varying vec4 varTexCoord;

vec4 gColor;


void main(void)
{
	
	gColor = inVertexColor.bgra;
	gl_Position = uMvpMatrix * inVertexPosition;
	varTexCoord = inTexCoord0;
	varVertexColor.rgb  = clamp(gColor.rgb, 0.0 , 1.0);
	varVertexColor.a = gColor.a;
	
	
	
	
}
