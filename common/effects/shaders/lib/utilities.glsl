mat3 compute_tbn(mat3 normal_matrix, vec3 normal, vec3 tangent, vec3 bitangents)
{
    //compute T/B/N
    vec3 t_pixel = normalize(normal_matrix * tangent);
    vec3 n_pixel = normalize(normal_matrix * normal);
	vec3 b_pixel = normalize(normal_matrix * bitangents);
	//as matrix
	return transpose(mat3(t_pixel.x, b_pixel.x, n_pixel.x,
				          t_pixel.y, b_pixel.y, n_pixel.y,
                          t_pixel.z, b_pixel.z, n_pixel.z));
}