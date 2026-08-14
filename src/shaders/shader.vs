#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 modelview;
uniform mat4 projection;

out vec3 ViewPos;
out float height;

void main()
{
  vec4 view_position = modelview * vec4(aPos, 1.0);

  ViewPos = view_position.xyz;
  height = aPos.z;

  gl_Position = projection * modelview * vec4(aPos, 1.0);
}

