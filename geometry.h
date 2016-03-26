/*
 * geometry.h
 *
 *  Created on: 22.03.2016
 *      Author: dima
 */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <GL/glew.h>
#include <iostream>
#include "math_3d.h"

class geometry {
public:
	GLuint VAO;
	GLuint VBO;
	GLuint VBO_TEX;
	GLuint IBO;
	GLuint IBO_TEX;
	GLuint VBO_NORMALS;
	float radius;
	int count;
	geometry();
	void GenBuffers(	unsigned int *indexes, unsigned int size_ind,
			Vector3f *arr, 	Vector2f *arr_tex, 	Vector3f *arr_normals, unsigned int size_arr);
	inline void PutData(float _radius, int _count){radius = _radius; count = _count;}
	virtual ~geometry();
};

#endif /* GEOMETRY_H_ */
