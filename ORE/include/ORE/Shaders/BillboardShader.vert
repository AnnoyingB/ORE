#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec3 aTexCoord;

out vec2 TexCoord;

uniform vec3 billboardPos;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform bool scale = false;

void main() {
	TexCoord = aTexCoord.xy;
	if(scale) {
		vec4 view_pos = view * model * vec4(billboardPos, 1.0);
		float dist = -view_pos.z;
		gl_Position = projection * (view_pos + vec4(aPos.xy*dist,0,0));
	} else {
		gl_Position = projection * (view * model * vec4(billboardPos, 1.0) + vec4(aPos.xy, 0, 0));
	}
}