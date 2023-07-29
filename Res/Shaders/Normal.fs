#version 330

in vec2 fragTexCoord;

out vec4 fragColor;

void main()
{
    fragColor = texture(texture0, fragTexCoord);
}