#version 330 core
out vec4 FragColor;  

uniform vec3 colorMin;
uniform vec3 colorMax;
uniform vec3 white;

in float probability;
uniform float maxProb;

float normProb = probability/maxProb;

void main()
{
    // the powers and the alpha multipliers were chosen by eye to make the color transition more visually appealing
    if (normProb > 0.5f)
        FragColor = vec4(mix(mix(colorMin,colorMax,pow(normProb,1)),white,pow(normProb,4)), (0.2+normProb)/2.0);
    else
        FragColor = vec4(mix(colorMin,colorMax,pow(normProb,0.7)), (0.2+normProb)/2.0);
        
    //FragColor = vec4(mix(colorMin,colorMax,normProb), normProb);  
}