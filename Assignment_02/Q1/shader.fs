#version 330
//in vec4 pos;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;

// lightingShader.setVec3("viewPos", camera.Position);
// lightingShader.setVec3("lightPos", lightPos); 

void main()
{


float specularStrength = 0.6;
vec3 lightColor=vec3(0,0,0.8);
vec3 objectColor=vec3(1,1,1);
float ambientStrength = 0.2;
vec3 specColor=vec3(1,1,1);
vec3 ambient = ambientStrength * lightColor;

vec3 norm = normalize(Normal);
vec3 lightDir = normalize(lightPos - FragPos);


vec3 viewDir = normalize(viewPos - FragPos);
vec3 reflectDir = reflect(-lightDir, norm);  

float diff = max(dot(norm, lightDir), 0.0);
vec3 diffuse = diff * lightColor;

float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
//vec3 specular = specularStrength * spec * lightColor;
vec3 specular = specularStrength * spec * specColor;

vec3 result = (ambient +diffuse + specular) * objectColor;
gl_FragColor = vec4(result, 1.0);
//gl_FragColor=vec4(1.0,0.0,0.0,1.0);
}
