#include "texture.h"

texture::texture(){
	width = height = 0;
	image = NULL;
}

texture::texture(GLenum TextureTarget, const std::string &src){
	image = SOIL_load_image(src.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
	if (image == 0)
		std::cout << "Image " << src << " not loaded" << std::endl;
    m_textureTarget = TextureTarget;
}

texture::~texture(){
	width = height = 0;
	if (image != NULL)
		SOIL_free_image_data(image);
	image = NULL;
}

bool texture::Load()
{
    glGenTextures(1, &m_textureObj);
    /*
     * Генерирует указанное число объектов текстур и помещает их в указатель на массив GLuint (второй параметр)
     */
    glBindTexture(m_textureTarget, m_textureObj);
    /*
     * 1. С какой текстурой работаем? 2D, 3D?
     * 2. К какому объекту текстуры привяжем
     */
    glTexImage2D(m_textureTarget,		//С какой текстурой работаем? 2D, 3D?
    			0,						//Качество. 0 - самый качественный
    			GL_RGBA,				//Какие каналы будут храниться
    			_height(),
    			_width(),
    			0.0f,
    			GL_RGBA,				//Входные каналы
    			GL_UNSIGNED_BYTE,
    			_image()
    			);
    if (m_textureTarget < 1)
    	return false;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//

    /*
     * glTexParameteri - устанавливает параметры текстуры. Первый параметр - тип, который может быть GL_TEXTURE_1D или GL_TEXTURE_2D.
     * 		Второй параметр - символическое имя параметры, который нужно установить.
     * 			Мы можем изменять: GL_TEXTURE_MIN_FILTER, GL_TEXTURE_MAG_FILTER, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T и GL_TEXTURE_BORDER_COLOR .
     * 		Третий параметр - значение, которое мы хотим установить, указанному в предыдущем параметре.

		GL_TEXTURE_MIN_FILTER - устанавливает тип функции, которая будет использоваться при уменьшении текстуры.
				Если ты решил изменить этот параметр, то ты должен указать GL_TEXTURE_MIN_FILTER вторым в функции glTexParameteri.

				В этом случае, третий параметр сможет принимать значения:
			GL_NEAREST - возвращает значение элемента текстуры, который является самым близким.
			GL_LINEAR - возвращает среднее значение из четырёх ближайших к центру тестируемого пиксела.

		GL_TEXTURE_MAG_FILTER - устанавливает функцию используемую при увеличении текстуры.
				Если ты решил её изменить, то ты должен установить GL_TEXTURE_MAG_FILTER вторым параметром в glTexParameteri, а третий сможет принимать значения:
			GL_NEAREST
			GL_LINEAR

			GL_CLAMP - запрещает повторение текстуры. Одна её копия будет натянута на весь объект.
			GL_REPEAT - разрешает повторение текстуры.

			GL_TEXTURE_BORDER_COLOR - устанавливает цвет обрамления текстуры. При этом параметре, третий сможет принимать значение массива из четырёх значений, которые интерпретируются как цвет (RGBA). Пор умолчанию (0,0,0,0).
     */

    return true;
}

void texture::Bind(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(m_textureTarget, m_textureObj);
}
