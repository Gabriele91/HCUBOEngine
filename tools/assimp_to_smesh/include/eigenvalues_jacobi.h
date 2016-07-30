#pragma once
#include <glm/mat3x3.hpp>

namespace glm
{
	/**
	* This is defined in glm/ext.hpp but for whatever reason ext.hpp can't be
	* included by CUDA code, so I decided to redefine it.
	*/
	GLM_FUNC_QUALIFIER glm::mat3 diagonal3x3(glm::vec3 &v)
	{
		glm::mat3 ret(0.0f);
		ret[0][0] = v[0];
		ret[1][1] = v[1];
		ret[2][2] = v[2];
		return ret;
	}

	GLM_FUNC_QUALIFIER void eigenvalues_rotate(glm::mat3& mat, double& c, double& s, int i0, int j0, int i1, int j1)
	{
		double a = c * mat[i0][j0] - s * mat[i1][j1];
		double b = s * mat[i0][j0] + c * mat[i1][j1];
		mat[i0][j0] = a;
		mat[i1][j1] = b;
	}

	/**
	* Diagonize matrix using Jacobi rotations.
	* @remark Method does not check if matrix is diagonizable.
	* Passing non diagonizable matrix and infinite max_iter (= -1)
	* May result in infinite loop.
	*/
	GLM_FUNC_QUALIFIER glm::vec3 eigenvalues_jacobi(glm::mat3& mat, int max_iter, glm::mat3& E)
	{
		glm::vec3 ret;

		// initial eigenvalues
		ret[0] = mat[0][0];
		ret[1] = mat[1][1];
		ret[2] = mat[2][2];

		mat[0][0] = 1.0;
		mat[1][1] = 1.0;
		mat[2][2] = 1.0;

		E = glm::mat3(1.0);

		for(int z = 0; z!=max_iter; ++z)
		{
			for (int i = 0; i != 3; ++i)
			{
				for (int j = i + 1; j < 3; j++)
				{
					double mii = ret[i];
					double mjj = ret[j];
					double mij = mat[i][j];
					double phi = 0.5 * std::atan2(2 * mij, mjj - mii);
					double c = std::cos(phi);
					double s = std::sin(phi);
					double mii1 = c * c * mii - 2 * s * c * mij + s * s * mjj;
					double mjj1 = s * s * mii + 2 * s * c * mij + c * c * mjj;
					if (abs(mii - mii1) < 0.00001 || (mjj - mjj1) < 0.00001) {
						ret[i] = mii1;
						ret[j] = mjj1;
						mat[i][j] = 0.0;
						for (unsigned int k = 0; k < i; k++) eigenvalues_rotate(mat, c, s, k, i, k, j);
						for (int k = i + 1; k < j; k++) eigenvalues_rotate(mat, c, s, i, k, k, j);
						for (int k = j + 1; k < 3; k++) eigenvalues_rotate(mat, c, s, i, k, j, k);
						//rotate eigenvectors
						for (int k = 0; k < 3; k++) eigenvalues_rotate(E, c, s, k, i, k, j);
					}
				}
			}
		}
		return ret;
	}
};