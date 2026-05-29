#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aProb; // probability attribute

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float probability;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    probability = aProb; // pass the probability to the fragment shader
    //gl_PointSize = 2.f+aProb; // cant decide which one looks better
    gl_PointSize = 2.f; // set the size of the point
}