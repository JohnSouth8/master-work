#version 330 core

// inputs
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 inColor;

out vec3 fragColor;

void main() {

	gl_Position = vec4( position, 0.0, 1.0);
	gl_PointSize = 10.0;
	fragColor = inColor;

}
