#version 330 core

void main()
{
	gl_FragDepth = gl_FragCoord.z; //set depth buffer manually if we want (this is done auto though so who cares...)
}