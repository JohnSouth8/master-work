#version 330 core

// inputs
in vec2 vertPosition;


// out vec3 fragColor;

void main() {

	gl_Position = vec4( vertPosition, 0.0, 1.0);

}
