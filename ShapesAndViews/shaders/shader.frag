#version 430 core

in Fragment{
	vec4 color;
}fragment;

out vec4 fColor;

void main(){
	fColor = fragment.color;
     //fColor = vec4(0.0, 1.0, 1.0, 1.0);
}
