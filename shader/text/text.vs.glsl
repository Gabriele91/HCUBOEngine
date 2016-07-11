layout(location = 0) in int character;
out int v_character;
out int v_position;

void main()
{
    v_character = character;
    v_position  = gl_VertexID;
    gl_Position = vec4(0, 0, 0, 1);
}
