#include "mat_vec.hpp"

t_vector::t_error t_vector::error = t_vector::ERR_NORM;

t_vector::t_vector(unsigned int count, FL_POINT elem){
	if (!count){
		ptr = NULL;
		return;
	}
	ptr = new t_vec;
	ptr->buff = new FL_POINT[count];
	for (unsigned int i = 0; i < count; ++i)
		ptr->buff[i] = elem;
	ptr->ct_lms = count;
	ptr->ct_pts = 0;
}

t_vector::t_vector(const t_vector &src){
	if (src.ptr != NULL)
		++src.ptr->ct_pts;
	ptr = src.ptr;
}

t_vector::~t_vector(){
	if (ptr != NULL){
		if (!ptr->ct_pts){
			delete[] ptr->buff;
			ptr->ct_pts = ptr->ct_lms = 0;
			delete ptr;
		}else
			--ptr->ct_pts;
	}
	ptr = NULL;
}

float t_vector::Length() const{
	float sum, c, y, t;
	sum = c = y = t = 0;
	if (ptr == NULL){
		error = ERR_NULL;
		return -1.0f;
	}
	for (unsigned int i = 0; i < ptr->ct_lms; ++i){
		y = ptr->buff[i] * ptr->buff[i] - c;
		t = sum + y;
		c = (t - sum) - y;
		sum = t;
	}
	return (float)std::sqrt(sum);
}

t_vector t_vector::Normalize() const{
	if (ptr == NULL){
		error = ERR_NULL;
		t_vector vec;
		return vec;
	}
	float len = this->Length();
	if ((len < 0)||(std::fabs(len) < VEC_EPS)){
		error = ERR_ZERO_LONG;
		return *this;
	}
	t_vector result;
	result.ptr = new t_vec;
	result.ptr->buff = new FL_POINT[ptr->ct_lms];
	result.ptr->ct_pts = 0;
	result.ptr->ct_lms = ptr->ct_lms;
	for (unsigned int i = 0; i < ptr->ct_lms; ++i)
		result.ptr->buff[i] = ptr->buff[i] / len;
	return result;
}

void t_vector::NormalizeInPlace(){
	if (ptr == NULL){
		error = ERR_NULL;
		return;
	}
	float len = this->Length();
	if ((len < 0)||(std::fabs(len) < VEC_EPS)){
		error = ERR_ZERO_LONG;
		return;
	}
	for (unsigned int i = 0; i < ptr->ct_lms; ++i)
		ptr->buff[i] /= len;
}

t_vector t_vector::operator +(const t_vector &src){
	if ((ptr == NULL)||(src.ptr == NULL)){
		error = ERR_NULL;
		t_vector vec;
		return vec;
	}
	if (ptr->ct_lms != src.ptr->ct_lms){
		error = ERR_SIZE;
		t_vector vec;
		return vec;
	}
	t_vector result(ptr->ct_lms, 0);
	for (unsigned int i = 0; i < ptr->ct_lms; ++i)
		result.ptr->buff[i] = ptr->buff[i] + src.ptr->buff[i];
	return result;
}

t_vector t_vector::operator -(const t_vector &src){
	if ((ptr == NULL)||(src.ptr == NULL)){
		error = ERR_NULL;
		t_vector vec;
		return vec;
	}
	if (ptr->ct_lms != src.ptr->ct_lms){
		error = ERR_SIZE;
		t_vector vec;
		return vec;
	}
	t_vector result(ptr->ct_lms, 0);
	for (unsigned int i = 0; i < ptr->ct_lms; ++i)
		result.ptr->buff[i] = ptr->buff[i] - src.ptr->buff[i];
	return result;
}

t_vector t_vector::operator ^(const t_vector &src){
	if ((ptr == NULL)||(src.ptr == NULL)){
		error = ERR_NULL;
		t_vector vec;
		return vec;
	}
	if (ptr->ct_lms != src.ptr->ct_lms){
		error = ERR_SIZE;
		t_vector vec;
		return vec;
	}
	t_vector result(ptr->ct_lms, 0);
	for (unsigned int i = 0; i < ptr->ct_lms; ++i)
		result.ptr->buff[i] = ptr->buff[i] * src.ptr->buff[i];
	return result;
}

FL_POINT t_vector::operator *(const t_vector &src){
	if ((ptr == NULL)||(src.ptr == NULL)){
		error = ERR_NULL;
		return -1;
	}
	if (ptr->ct_lms != src.ptr->ct_lms){
		error = ERR_SIZE;
		return -1;
	}
	FL_POINT sum, c, y, t;
	sum = c = y = t = 0;
	for (unsigned int i = 0; i < ptr->ct_lms; ++i){
		y = (ptr->buff[i] * src.ptr->buff[i]) - c;
		t = sum + y;
		c = (t - sum) - y;
		sum = t;
	}
	return sum;
}

/*Реализовано для векторов размерности не больше 4*/
t_vector t_vector::operator ()(const t_vector &src){
	if (ptr->ct_lms < 3){
		error = ERR_CROSS;
		t_vector vec;
		return vec;
	}
	if (ptr->ct_lms != src.ptr->ct_lms){
		error = ERR_SIZE;
		t_vector vec;
		return vec;
	}
	t_vector result(ptr->ct_lms, 0);
	result[0] = ptr->buff[1] * src.ptr->buff[2] - src.ptr->buff[1] * ptr->buff[2];
	result[1] = ptr->buff[0] * src.ptr->buff[2] - src.ptr->buff[0] * ptr->buff[2];
	result[2] = ptr->buff[0] * src.ptr->buff[1] - src.ptr->buff[0] * ptr->buff[1];
	result[3] = 1;
	return result;
}

t_vector &t_vector::operator =(const t_vector &src){
	if (ptr != NULL){
		if (!ptr->ct_pts){
			delete[] ptr->buff;
			ptr->ct_lms = 0;
			delete ptr;
		}else
			--ptr->ct_pts;
	}
	if (src.ptr != NULL)
		++src.ptr->ct_pts;
	ptr = src.ptr;
	return *this;
}

t_vector t_vector::operator *(FL_POINT sclr){
	if (ptr == NULL){
		error = ERR_NULL;
		t_vector vec;
		return vec;
	}
	t_vector result(ptr->ct_lms, 1);
	for (unsigned int i = 0; i < ptr->ct_lms; ++i)
		result.ptr->buff[i] = ptr->buff[i] * sclr;
	return result;
}

t_vector operator *(FL_POINT sclr, const t_vector &src){
	if (src.ptr == NULL){
		t_vector::error = t_vector::ERR_NULL;
		t_vector vec;
		return vec;
	}
	t_vector result(src.ptr->ct_lms, 1);
	for (unsigned int i = 0; i < src.ptr->ct_lms; ++i)
		result.ptr->buff[i] = src.ptr->buff[i] * sclr;
	return result;
}

std::ostream &operator <<(std::ostream &out, t_vector &src){
	if (src.ptr != NULL){
		out << '(';
		out << src.ptr->buff[0];
		for (unsigned int i = 1; i < src.ptr->ct_lms; ++i)
			out << ',' << src.ptr->buff[i];
		out << ')';
	}
	return out;
}

std::ofstream &operator <<(std::ofstream &out, t_vector &src){
	if (src.ptr != NULL){
		out << '(';
		out << src.ptr->buff[0];
		for (unsigned int i = 1; i < src.ptr->ct_lms; ++i)
			out << ',' << src.ptr->buff[i];
		out << ')';
	}
	return out;
}

t_vector::t_iter t_vector::operator [](unsigned int num){
	if (ptr == NULL){
		error = ERR_ACCESS;
		return t_iter();
	}
	return t_iter(this, num);
}

FL_POINT t_vector::t_iter::operator =(FL_POINT elem){
	if (vtr == NULL)
		return 0;
	vtr->ptr->buff[ind] = elem;
	return elem;
}

FL_POINT t_vector::t_iter::operator +=(FL_POINT elem){
	if (vtr == NULL)
		return 0;
	vtr->ptr->buff[ind] += elem;
	return elem;
}

FL_POINT t_vector::t_iter::operator -=(FL_POINT elem){
	if (vtr == NULL)
		return 0;
	vtr->ptr->buff[ind] -= elem;
	return elem;
}

FL_POINT t_vector::t_iter::operator /=(FL_POINT elem){
	if ((std::fabs(elem) < VEC_EPS)||(vtr == NULL))
		return 0;
	vtr->ptr->buff[ind] /= elem;
	return elem;
}

FL_POINT t_vector::t_iter::operator *=(FL_POINT elem){
	if (vtr == NULL)
		return 0;
	vtr->ptr->buff[ind] *= elem;
	return elem;
}

t_vector::t_iter::operator FL_POINT() const{
	if (vtr == NULL)
		return 0;
	return vtr->ptr->buff[ind];
}
