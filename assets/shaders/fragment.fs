#version 330 core
out vec4 FragColor;  

uniform vec3 colorMin;
uniform vec3 colorMax;
uniform vec3 white;

uniform float prob;
uniform float maxProb;

float normProb = prob/maxProb;

void main()
{
    
    if (normProb > 0.5f)
        FragColor = vec4(mix(mix(colorMin,colorMax,pow(normProb,1)),white,pow(normProb,4)), normProb);
    else
        FragColor = vec4(mix(colorMin,colorMax,pow(normProb,0.7)), normProb);
        
    //FragColor = vec4(mix(colorMin,colorMax,normProb), normProb);  
}