/*
 * geometry.cpp
 *
 *  Created on: 22.03.2016
 *      Author: dima
 */

#include "geometry.h"

geometry::geometry() {
	VAO = -1;
	IBO = -1;
	IBO_TEX = -1;
	VBO_TEX = -1;
	VBO_NORMALS = -1;
	radius = 0;
	count = 0;
}

void geometry::GenBuffers(	unsigned int *indexes, unsigned int size_ind,
							Vector3f *arr, 	Vector2f *arr_tex, 	Vector3f *arr_normals, 	unsigned int size_arr)
{
	//VAO
	if ((arr != NULL) && (size_arr != 0))
	{
		glGenVertexArrays(1, &VAO);
			if (VAO < 0){
				std::cout << "could not generate buffers" << std::endl;
				return;
		}
			glBindVertexArray(VAO);

			//VBO
			glGenBuffers(
				1, 						//Количество объектов которое надо создать
				&VBO					//Ссылка на массив с объектами
				);						//vbos - идентификатор

			glBindBuffer(
				GL_ARRAY_BUFFER,		//Будет хранить массив вершин
				VBO						//Здеся
				);						//Привязываем указатель к названию цели
			glBufferData(
				GL_ARRAY_BUFFER, 		//Название цели
				size_arr*sizeof(Vector3f), 				//Размер данных в байтах
				arr, 					//Адрес массива вершин
				GL_STATIC_DRAW			//Говорит о том, что мы не будем изменять значения буфера
				);						//Заполняем данными


			//IBO

			if ((indexes != NULL) && (size_ind != 0))
			{
				glGenBuffers(1, &IBO);
				if (IBO < 0){
					std::cout << "could not generate index buffers" << std::endl;
					return;
				}
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, size_ind, indexes, GL_STATIC_DRAW);

			}
	}
	else
		return;

	//VBO_TEX
	if ((arr_tex != NULL) && (size_arr != 0))
	{
		glGenBuffers(
				1, 						//Количество объектов которое надо создать
				&VBO_TEX					//Ссылка на массив с объектами
				);						//vbos - идентификатор

		glBindBuffer(
				GL_ARRAY_BUFFER,		//Будет хранить массив вершин
				VBO_TEX						//Здеся
				);						//Привязываем указатель к названию цели
		glBufferData(
				GL_ARRAY_BUFFER, 		//Название цели
				size_arr*sizeof(Vector2f), 				//Размер данных в байтах
				arr_tex, 					//Адрес массива вершин
				GL_STATIC_DRAW			//Говорит о том, что мы не будем изменять значения буфера
				);						//Заполняем данными
	}

	//VBO_NORMALS
	if ((arr_normals != NULL) && (size_arr != 0))
	{
		glGenBuffers(
				1, 						//Количество объектов которое надо создать
				&VBO_NORMALS					//Ссылка на массив с объектами
				);						//vbos - идентификатор

		glBindBuffer(
				GL_ARRAY_BUFFER,		//Будет хранить массив вершин
				VBO_NORMALS						//Здеся
				);						//Привязываем указатель к названию цели
		glBufferData(
				GL_ARRAY_BUFFER, 		//Название цели
				size_arr*sizeof(Vector3f), 				//Размер данных в байтах
				arr_normals, 			//Адрес массива вершин
				GL_STATIC_DRAW			//Говорит о том, что мы не будем изменять значения буфера
				);						//Заполняем данными
	}
}

geometry::~geometry() {
	// TODO Auto-generated destructor stub
}

