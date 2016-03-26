#ifndef __INCLUDE_MAT_VEC_

#define __INCLUDE_MAT_VEC_
#define __INCLUDE_VECTOR_

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>

#define VEC_EPS (1e-5)
#define MX_FL_EPS (1e-5)

typedef float FL_POINT;

class t_matrix;

class t_vector{
public:
	enum t_error {
		ERR_NORM,
		ERR_NULL,
		ERR_ACCESS,
		ERR_CROSS,
		ERR_SIZE,
		ERR_ZERO_LONG
	};
private:
	typedef struct{
		FL_POINT *buff;			//Массив значений
		unsigned int ct_lms;	//Количество аргументов
		unsigned int ct_pts;	//Количество указателей на объект
	}t_vec;
	static t_error error;
	t_vec *ptr;
public:
	/*Errors*/
	inline static t_error get_error(){ return error;};
	inline static void off_error(){ error = ERR_NORM;};
	/*Init/Destruct/Copy/...*/
	t_vector(unsigned int count, FL_POINT elem);
	t_vector(const t_vector &);
	t_vector(): ptr(NULL){};
	~t_vector();
	inline unsigned int size() const{
		if (ptr == NULL)
			return 0;
		return ptr->ct_lms;
	}
	/*for t_vector class*/
	float Length() const;
	t_vector Normalize() const;
	void NormalizeInPlace();
	t_vector operator +(const t_vector &);
	t_vector operator -(const t_vector &);
	t_vector operator ^(const t_vector &);			/*Покомпонентное произведение*/
	FL_POINT operator *(const t_vector &);			/*Скалярное произведение*/
	t_vector operator ()(const t_vector &);		/*Векторное произведение*/
	t_vector &operator =(const t_vector &);
	inline bool operator ==(const t_vector &src){
		if (ptr == NULL){
			if (src.ptr == NULL) return true;
			else return false;
		}else if (src.ptr == NULL) return false;

		for (unsigned int i = 0; i < ptr->ct_lms; ++i)
			if ((src.ptr->buff[i] - ptr->buff[i]) > VEC_EPS)
				return false;

		return true;
	}
	/*for scalar*/
	t_vector operator *(FL_POINT sclr);
	friend t_vector operator *(FL_POINT sclr, const t_vector &src);
	friend std::ostream &operator <<(std::ostream &, t_vector &);
	friend std::ofstream &operator <<(std::ofstream &, t_vector &);
public:
	class t_iter {
	private:
		t_vector *vtr;
		unsigned int ind;
	protected:
		t_iter(t_vector *_vtr, unsigned int _ind): vtr(_vtr), ind(_ind){};
		t_iter(): vtr(NULL), ind(0){};
		friend class t_vector;
	public:
		FL_POINT operator =(FL_POINT elem);
		FL_POINT operator +=(FL_POINT elem);
		FL_POINT operator -=(FL_POINT elem);
		FL_POINT operator /=(FL_POINT elem);
		FL_POINT operator *=(FL_POINT elem);
		operator FL_POINT() const;
		bool operator ==(FL_POINT elem){
			if (vtr == NULL)
				return 0;
			return (vtr->ptr->buff[ind] == elem) ? 1 : 0;
		}
		bool operator >(FL_POINT elem){
			if (vtr == NULL)
				return 0;
			return (vtr->ptr->buff[ind] > elem) ? 1 : 0;
		}
		bool operator >=(FL_POINT elem){
			if (vtr == NULL)
				return 0;
			return (vtr->ptr->buff[ind] >= elem) ? 1 : 0;
		}
		bool operator <(FL_POINT elem){
			if (vtr == NULL)
				return 0;
			return (vtr->ptr->buff[ind] < elem) ? 1 : 0;
		}
		bool operator <=(FL_POINT elem){
			if (vtr == NULL)
				return 0;
			return (vtr->ptr->buff[ind] <= elem) ? 1 : 0;
		}
		bool operator !=(FL_POINT elem){
			if (vtr == NULL)
				return 0;
			return (vtr->ptr->buff[ind] != elem) ? 1 : 0;
		}
	};
protected:
	friend class t_iter;
	friend class t_matrix;
public:
	t_iter operator [](unsigned int num);
};
/*-------------------------------------*/

class t_matrix{
public:
	enum t_error {
		ERR_NORM,
		ERR_NULL,
		ERR_SIZE,
		ERR_DIMENSION,
		ERR_DIV0
	};
protected:
	typedef struct {
		FL_POINT *buff;//Можно сделать одномерный массив
		unsigned int ct_rws, ct_cms;
		unsigned int ct_pts;
	} MatrixBuff;
	static t_error error;
	MatrixBuff *ptr;

	friend class t_vector;
	friend FL_POINT gauss_det(const t_matrix &);
	friend t_matrix bord(t_matrix &, unsigned int, unsigned int);
public:
	t_matrix(unsigned int count_rows, unsigned int count_columns);
	t_matrix(unsigned int num);
	t_matrix(unsigned int count_rows, unsigned int count_columns, FL_POINT *src);
	t_matrix(unsigned int num, FL_POINT *src);
	t_matrix(const t_matrix &from);
	t_matrix(): ptr(NULL){};
	~t_matrix();
	inline unsigned int rows() const{
		if (ptr == NULL)
			return 0;
		return ptr->ct_rws;
	}
	inline unsigned int columns() const{
		if (ptr == NULL)
			return 0;
		return ptr->ct_cms;
	}
	inline unsigned int size() const{
		if (ptr == NULL)
			return 0;
		if (ptr->ct_rws != ptr->ct_cms){
			error = ERR_SIZE;
			return 0;
		}
		return ptr->ct_rws;
	}
	t_matrix operator +(const t_matrix &);
	t_matrix operator -(const t_matrix &);
	t_matrix operator *(const t_matrix &);
	t_matrix operator *(double);
	friend t_matrix operator *(double, const t_matrix &);
	t_matrix operator *(float);
	friend t_matrix operator *(float, const t_matrix &);
	t_matrix operator /(double);
	t_matrix operator /(float);
	t_matrix operator =(const t_matrix &);
	friend std::ostream &operator <<(std::ostream &, const t_matrix &);
	// friend std::ofstream &operator <<(std::ofstream &, const t_matrix &);
	inline static t_error get_error(){return error;}
	inline static void off_error(){error = ERR_NORM;}
	/**/
	t_matrix T();
	FL_POINT Det();
	t_matrix Inv();
	t_vector operator *(const t_vector &);
	friend t_matrix operator -(const t_matrix &src);
/**/
	friend class t_iter;
	class t_iter{
	private:
		MatrixBuff **mtr;
		unsigned int ind;
	public:
		t_iter(MatrixBuff **_mtr, unsigned int _ind): mtr(_mtr), ind(_ind){};
		friend class tt_iter;
		class tt_iter{
		private:
			MatrixBuff **mtr;
			unsigned int ind1, ind2;
		public:
			tt_iter(MatrixBuff **from, unsigned int _ind1, unsigned int _ind2) : mtr(from), ind1(_ind1), ind2(_ind2){};
			tt_iter(void): mtr(NULL), ind1(0), ind2(0){};
			FL_POINT operator =(FL_POINT from);
			FL_POINT operator =(tt_iter from);
			FL_POINT operator +=(FL_POINT from);
			FL_POINT operator -=(FL_POINT from);
			FL_POINT operator /=(FL_POINT from);
			FL_POINT operator *=(FL_POINT from);
			bool operator ==(FL_POINT from);
			bool operator >(FL_POINT from);
			bool operator >=(FL_POINT from);
			bool operator <=(FL_POINT from);
			bool operator <(FL_POINT from);
			operator FL_POINT(void) const;
		};
	public:
		tt_iter operator [](unsigned int _ind);
	};
	public:
	t_iter operator [](unsigned int _num);
/**/
};

#endif
