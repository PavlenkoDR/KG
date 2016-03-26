#include "mat_vec.hpp"

t_matrix::t_error t_matrix::error = t_matrix::ERR_NORM;

t_matrix::t_matrix(unsigned int count_rows, unsigned int count_columns){

	if ((!count_columns)&&(!count_rows)){
		error = ERR_SIZE;
		ptr = NULL;
		return;
	}
	ptr = new MatrixBuff;
	ptr->ct_rws = count_rows;
	ptr->ct_cms = count_columns;
	ptr->ct_pts = 0;
	ptr->buff = new FL_POINT[count_rows * count_columns];
	for (unsigned int i = 0; i < count_rows * count_columns; ++i)
		ptr->buff[i] = (FL_POINT)0;
}
t_matrix::t_matrix(unsigned int num){
	if (!num){
		error = ERR_SIZE;
		ptr = NULL;
		return;
	}
	ptr = new MatrixBuff;
	ptr->ct_rws = num;
	ptr->ct_cms = num;
	ptr->ct_pts = 0;
	ptr->buff = new FL_POINT[num*num];
	for (unsigned int i = 0; i < num*num; ++i)
		ptr->buff[i] = (FL_POINT)0;
}

t_matrix::t_matrix(unsigned int count_rows, unsigned int count_columns, FL_POINT *src){
	if ((!count_columns)&&(!count_rows)){
		error = ERR_SIZE;
		ptr = NULL;
		return;
	}
	ptr = new MatrixBuff;
	ptr->ct_rws = count_rows;
	ptr->ct_cms = count_columns;
	ptr->ct_pts = 0;
	ptr->buff = new FL_POINT[count_rows * count_columns];
	if (src == NULL)
	for (unsigned int i = 0; i < count_rows * count_columns; ++i)
		ptr->buff[i] = (FL_POINT)0;
	else
		for (unsigned int i = 0; i < count_rows * count_columns; ++i)
			ptr->buff[i] = src[i];
}

t_matrix::t_matrix(unsigned int num, FL_POINT *src){
	if (!num){
		error = ERR_SIZE;
		ptr = NULL;
		return;
	}
	ptr = new MatrixBuff;
	ptr->ct_rws = num;
	ptr->ct_cms = num;
	ptr->ct_pts = 0;
	ptr->buff = new FL_POINT[num * num];
	if (src == NULL)
	for (unsigned int i = 0; i < num * num; ++i)
		ptr->buff[i] = (FL_POINT)0;
	else
		for (unsigned int i = 0; i < num * num; ++i)
			ptr->buff[i] = src[i];
}

t_matrix::t_matrix(const t_matrix &from){

	ptr = from.ptr;
	if (ptr != NULL)
		++ptr->ct_pts;
}

t_matrix::~t_matrix(){

	if (ptr == NULL)
		return;
	if (!ptr->ct_pts){
		delete[] ptr->buff;
		ptr->ct_rws = ptr->ct_cms = 0;
		delete ptr;
	}else
		--ptr->ct_pts;
	ptr = NULL;
}

t_matrix t_matrix::operator +(const t_matrix &right){	//Возвращается объект по значению, ибо сработает контруктор копирования
	if ((right.ptr == NULL)||(ptr == NULL)){
		error = ERR_NULL;
		return *this;
	}
	if ((ptr->ct_rws != right.ptr->ct_rws)||(ptr->ct_cms != right.ptr->ct_cms)){
		error = ERR_DIMENSION;
		return *this;
	}
	t_matrix ans(ptr->ct_rws, ptr->ct_cms); //Выделяется память под объект
	for (unsigned int i = 0; i < ptr->ct_rws; ++i)
		for (unsigned int j = 0; j < ptr->ct_cms; ++j)
			ans.ptr->buff[i*ptr->ct_cms + j] = ptr->buff[i*(ptr->ct_cms) + j] + right.ptr->buff[i*(ptr->ct_cms) + j];
	return ans;	//Вызывается контруктор копирования
}

t_matrix t_matrix::operator -(const t_matrix &right){
	if ((right.ptr == NULL)||(ptr == NULL)){
		error = ERR_NULL;
		return *this;
	}
	if ((ptr->ct_rws != right.ptr->ct_rws)||(ptr->ct_cms != right.ptr->ct_cms)){
		error = ERR_DIMENSION;
		return *this;
	}
	t_matrix ans(ptr->ct_rws, ptr->ct_cms);
	for (unsigned int i = 0; i < ptr->ct_rws; ++i)
		for (unsigned int j = 0; j < ptr->ct_cms; ++j)
			ans.ptr->buff[i*ptr->ct_cms + j] = ptr->buff[i*(ptr->ct_cms) + j] - right.ptr->buff[i*(ptr->ct_cms) + j];
	return ans;
}

t_matrix t_matrix::operator *(const t_matrix &right){
	if ((ptr == NULL)||(right.ptr == NULL)){
		error = ERR_NULL;
		return *this;
	}
	if (ptr->ct_cms != right.ptr->ct_rws){
		error = ERR_DIMENSION;
		return *this;
	}
	t_matrix ans(ptr->ct_rws, right.ptr->ct_cms);
	for (unsigned int i = 0; i < ptr->ct_rws; ++i)
		for (unsigned int j = 0; j < right.ptr->ct_cms; ++j)
			for (unsigned int k = 0; k < ptr->ct_cms; ++k)
				ans.ptr->buff[i*right.ptr->ct_cms + j] += ptr->buff[i*ptr->ct_cms + k] * right.ptr->buff[k*right.ptr->ct_cms + j];
	return ans;
}

//С 16 числа прием доков на офицера запаса. Экипажная 18, паспорт, приписное.
t_matrix t_matrix::operator *(double addition){
	if (ptr == NULL){
		error = ERR_NULL;
		return *this;
	}
	t_matrix ans(ptr->ct_rws, ptr->ct_cms);
	for (unsigned int i = 0; i < ptr->ct_rws; ++i)
		for (unsigned int j = 0; j < ptr->ct_cms; ++j)
			ans.ptr->buff[i*ptr->ct_cms + j] = ptr->buff[i*ptr->ct_cms + j] * ((FL_POINT)addition);
	return ans;
}

t_matrix operator *(double addition, const t_matrix &right){
	if (right.ptr == NULL){
		t_matrix::error = t_matrix::ERR_NULL;
		return right;
	}
	t_matrix ans(right.ptr->ct_rws, right.ptr->ct_cms);
	for (unsigned int i = 0; i < right.ptr->ct_rws; ++i)
		for (unsigned int j = 0; j < right.ptr->ct_cms; ++j)
			ans.ptr->buff[i*right.ptr->ct_cms + j] = right.ptr->buff[i*right.ptr->ct_cms + j] * ((FL_POINT)addition);
	return ans;
}

t_matrix t_matrix::operator *(const float addition){
	if (ptr == NULL){
		error = ERR_NULL;
		return *this;
	}
	t_matrix ans(ptr->ct_rws, ptr->ct_cms);
	for (unsigned int i = 0; i < ptr->ct_rws; ++i)
		for (unsigned int j = 0; j < ptr->ct_cms; ++j)
			ans.ptr->buff[i*ptr->ct_cms + j] = ptr->buff[i*ptr->ct_cms + j] * ((FL_POINT)addition);
	return ans;
}

t_matrix operator *(float addition, const t_matrix &right){
	if (right.ptr == NULL){
		t_matrix::error = t_matrix::ERR_NULL;
		return right;
	}
	t_matrix ans(right.ptr->ct_rws, right.ptr->ct_cms);
	for (unsigned int i = 0; i < ans.ptr->ct_rws; ++i)
		for (unsigned int j = 0; j < ans.ptr->ct_cms; ++j)
			ans.ptr->buff[i*(ans.ptr->ct_cms) + j] = right.ptr->buff[i*(ans.ptr->ct_cms) + j] * ((FL_POINT)addition);
	return ans;
}

t_matrix t_matrix::operator /(const float addition){
	if (ptr == NULL){
		t_matrix ans;
		error = ERR_DIV0;
		return ans;
	}
	if (((((FL_POINT)addition) - MX_FL_EPS < 0)&&(((FL_POINT)addition) + MX_FL_EPS > 0))){
		t_matrix ans;
		error = ERR_NULL;
		return ans;
	}
	t_matrix ans(ptr->ct_rws, ptr->ct_cms);
	for (unsigned int i = 0; i < ptr->ct_rws; ++i)
		for (unsigned int j = 0; j < ptr->ct_cms; ++j)
			ans.ptr->buff[i*(ptr->ct_cms) + j] = ptr->buff[i*(ptr->ct_cms) + j] / ((FL_POINT)addition);
	return ans;
}

t_matrix t_matrix::operator /(const double addition){
	if (ptr == NULL){
		error = ERR_NULL;
		t_matrix ans;
		return ans;
	}
	if (((((FL_POINT)addition) - MX_FL_EPS < 0)&&(((FL_POINT)addition) + MX_FL_EPS > 0))){
		t_matrix ans;
		error = ERR_DIV0;
		return ans;
	}
	t_matrix ans(ptr->ct_rws, ptr->ct_cms);
	for (unsigned int i = 0; i < ptr->ct_rws; ++i)
		for (unsigned int j = 0; j < ptr->ct_cms; ++j)
			ans.ptr->buff[i*(ptr->ct_cms) + j] = ptr->buff[i*(ptr->ct_cms) + j] / ((FL_POINT)addition);
	return ans;
}

t_matrix t_matrix::operator =(const t_matrix &from){
	if (ptr == from.ptr)
		return *this;
	if (ptr != NULL){
		if (!(ptr->ct_pts)){
			delete[] ptr->buff;
			delete ptr;
		}else
			--ptr->ct_pts;
	}
	if (from.ptr != NULL)
		++from.ptr->ct_pts;
	ptr = from.ptr;
	return *this;
}

std::ostream &operator <<(std::ostream &_out, const t_matrix &_mtr){
	if (_mtr.ptr != NULL)
		for (unsigned int i = 0; i < _mtr.ptr->ct_rws; ++i){
			if (i)
				_out << '\n';
			for (unsigned int j = 0; j < _mtr.ptr->ct_cms; ++j){
				_out << std::fixed << std::setw(10) << std::setprecision(5)
          << std::setfill(' ') << _mtr.ptr->buff[i*(_mtr.ptr->ct_cms) + j];
				if (j != _mtr.ptr->ct_cms - 1)
					_out<< '\t';
			}
		}
	return _out;
}
/*
std::ofstream &operator <<(std::ofstream &_out, const t_matrix &_mtr){
	if (_mtr.ptr != NULL)
		for (unsigned int i = 0; i < _mtr.ptr->ct_rws; ++i){
			if (i)
				_out << std::endl;
			for (unsigned int j = 0; j < _mtr.ptr->ct_cms; ++j){
				_out << _mtr.ptr->buff[i*(_mtr.ptr->ct_cms) + j];
				if (j != _mtr.ptr->ct_cms - 1)
					_out<< '\t';
			}
		}
	return _out;
}
*/
t_matrix::t_iter t_matrix::operator [](unsigned int _num){
	if (ptr == NULL)
		error = ERR_NULL;
	else
		error = ERR_NORM;
	return t_iter(&ptr, _num);
}

t_matrix::t_iter::tt_iter t_matrix::t_iter::operator [](unsigned int _ind){
	if (error == ERR_NULL)
		return tt_iter();
	if (((*mtr)->ct_rws < ind)||((*mtr)->ct_cms < _ind)){
		error = ERR_DIMENSION;
		return tt_iter();
	}
	return tt_iter(mtr, ind, _ind);
}

FL_POINT t_matrix::t_iter::tt_iter::operator =(FL_POINT from){
	if (error != ERR_NORM)
		return from;
	if ((*mtr)->ct_pts){
		MatrixBuff *plur = new MatrixBuff;
		--(*mtr)->ct_pts;
		plur->ct_rws = (*mtr)->ct_rws;
		plur->ct_cms = (*mtr)->ct_cms;
		plur->ct_pts = 0;
		plur->buff = new FL_POINT[plur->ct_rws * plur->ct_cms];
		for (unsigned long int count = 0; count < plur->ct_rws * plur->ct_cms; ++count)
			plur->buff[count] = (*mtr)->buff[count];
		*mtr = plur;
	}
	(*mtr)->buff[ind1*((*mtr)->ct_cms) + ind2] = from;
	return *((*mtr)->buff + ind1*((*mtr)->ct_cms) + ind2);
}

FL_POINT t_matrix::t_iter::tt_iter::operator =(t_matrix::t_iter::tt_iter from){
	if (error != ERR_NORM)
		return from;
	if ((*mtr)->ct_pts){
		MatrixBuff *plur = new MatrixBuff;
		--(*mtr)->ct_pts;
		plur->ct_rws = (*mtr)->ct_rws;
		plur->ct_cms = (*mtr)->ct_cms;
		plur->ct_pts = 0;
		plur->buff = new FL_POINT[plur->ct_rws * plur->ct_cms];
		for (unsigned long int count = 0; count < plur->ct_rws * plur->ct_cms; ++count)
			plur->buff[count] = (*mtr)->buff[count];
		*mtr = plur;
	}
	if (from.mtr == NULL){
		error = ERR_NULL;
		return *this;
	}
	(*mtr)->buff[ind1*((*mtr)->ct_cms) + ind2] = (*(from.mtr))->buff[from.ind1*((*(from.mtr))->ct_cms) + from.ind2];
	return *((*mtr)->buff + ind1*((*mtr)->ct_cms) + ind2);
}

t_matrix::t_iter::tt_iter::operator FL_POINT(void) const{
	if (error != ERR_NORM){
		return 0;
	}
	return *((*mtr)->buff + ind1*((*mtr)->ct_cms) + ind2);
}

/*---*/
FL_POINT t_matrix::t_iter::tt_iter::operator +=(FL_POINT from){
	if (error != ERR_NORM)
		return from;
	if ((*mtr)->ct_pts){
		MatrixBuff *plur = new MatrixBuff;
		--(*mtr)->ct_pts;
		plur->ct_rws = (*mtr)->ct_rws;
		plur->ct_cms = (*mtr)->ct_cms;
		plur->ct_pts = 0;
		plur->buff = new FL_POINT[plur->ct_rws * plur->ct_cms];
		for (unsigned long int count = 0; count < plur->ct_rws * plur->ct_cms; ++count)
			plur->buff[count] = (*mtr)->buff[count];
		*mtr = plur;
	}
	(*mtr)->buff[ind1*((*mtr)->ct_cms) + ind2] += from;
	return *((*mtr)->buff + ind1*((*mtr)->ct_cms) + ind2);
}

FL_POINT t_matrix::t_iter::tt_iter::operator -=(FL_POINT from){
	if (error != ERR_NORM){
		return from;
	}
	if ((*mtr)->ct_pts){
		MatrixBuff *plur = new MatrixBuff;
		--(*mtr)->ct_pts;
		plur->ct_rws = (*mtr)->ct_rws;
		plur->ct_cms = (*mtr)->ct_cms;
		plur->ct_pts = 0;
		plur->buff = new FL_POINT[plur->ct_rws * plur->ct_cms];
		for (unsigned long int count = 0; count < plur->ct_rws * plur->ct_cms; ++count)
			plur->buff[count] = (*mtr)->buff[count];
		*mtr = plur;
	}
	(*mtr)->buff[ind1*((*mtr)->ct_cms) + ind2] -= from;
	return *((*mtr)->buff + ind1*((*mtr)->ct_cms) + ind2);
}

FL_POINT t_matrix::t_iter::tt_iter::operator /=(FL_POINT from){
	if (error != ERR_NORM)
		return from;
	if ((*mtr)->ct_pts){
		MatrixBuff *plur = new MatrixBuff;
		--(*mtr)->ct_pts;
		plur->ct_rws = (*mtr)->ct_rws;
		plur->ct_cms = (*mtr)->ct_cms;
		plur->ct_pts = 0;
		plur->buff = new FL_POINT[plur->ct_rws * plur->ct_cms];
		for (unsigned long int count = 0; count < plur->ct_rws * plur->ct_cms; ++count)
			plur->buff[count] = (*mtr)->buff[count];
		*mtr = plur;
	}
	if (std::fabs(from) < MX_FL_EPS){
		error = ERR_DIV0;
		return (1/0.0);
	}
	(*mtr)->buff[ind1*((*mtr)->ct_cms) + ind2] /= from;
	return *((*mtr)->buff + ind1*((*mtr)->ct_cms) + ind2);
}

FL_POINT t_matrix::t_iter::tt_iter::operator *=(FL_POINT from){
	if (error != ERR_NORM)
		return from;
	if ((*mtr)->ct_pts){
		MatrixBuff *plur = new MatrixBuff;
		--(*mtr)->ct_pts;
		plur->ct_rws = (*mtr)->ct_rws;
		plur->ct_cms = (*mtr)->ct_cms;
		plur->ct_pts = 0;
		plur->buff = new FL_POINT[plur->ct_rws * plur->ct_cms];
		for (unsigned long int count = 0; count < plur->ct_rws * plur->ct_cms; ++count)
			plur->buff[count] = (*mtr)->buff[count];
		*mtr = plur;
	}
	(*mtr)->buff[ind1*((*mtr)->ct_cms) + ind2] *= from;
	return *((*mtr)->buff + ind1*((*mtr)->ct_cms) + ind2);
}

bool t_matrix::t_iter::tt_iter::operator ==(FL_POINT from){
	if (error != ERR_NORM)
		return 0;
	return (*((*mtr)->buff + ind1*((*mtr)->ct_cms) + ind2) == from ? 1 : 0);
}

bool t_matrix::t_iter::tt_iter::operator >(FL_POINT from){
	if (error != ERR_NORM)
		return 0;
	return (*((*mtr)->buff + ind1*((*mtr)->ct_cms) + ind2) > from ? 1 : 0);
}

bool t_matrix::t_iter::tt_iter::operator >=(FL_POINT from){
	if (error != ERR_NORM)
		return 0;
	return (*((*mtr)->buff + ind1*((*mtr)->ct_cms) + ind2) >= from ? 1 : 0);
}

bool t_matrix::t_iter::tt_iter::operator <=(FL_POINT from){
	if (error != ERR_NORM)
		return 0;
	return (*((*mtr)->buff + ind1*((*mtr)->ct_cms) + ind2) <= from ? 1 : 0);
}

bool t_matrix::t_iter::tt_iter::operator <(FL_POINT from){
	if (error != ERR_NORM)
		return 0;
	return (*((*mtr)->buff + ind1*((*mtr)->ct_cms) + ind2) < from ? 1 : 0);
}

/*---*/
t_matrix t_matrix::T(){
	if (ptr == NULL){
		error = ERR_NULL;
		t_matrix res;
		return res;
	}
	t_matrix result(ptr->ct_cms, ptr->ct_rws);
	for (unsigned int i = 0; i < ptr->ct_rws; ++i)
		for (unsigned int j = 0; j < ptr->ct_cms; ++ j)
			result[j][i] = ptr->buff[i*ptr->ct_cms + j];
	return result;
}

t_matrix operator -(const t_matrix &src){
	if (src.ptr == NULL){
		t_matrix::error = t_matrix::ERR_NULL;
		t_matrix mat;
		return mat;
	}
	t_matrix result(src);
	for (unsigned int i = 0; i < src.ptr->ct_rws; ++i)
		for (unsigned int j = 0; j < src.ptr->ct_cms; ++j)
			result[i][j] = -src.ptr->buff[i*src.ptr->ct_cms + j];
	return result;
}

FL_POINT gauss_det(const t_matrix &O){
	t_matrix Q(O);
	unsigned int	column[Q.ptr->ct_cms],
						row[Q.ptr->ct_rws];
	unsigned int count = 0;
	for (unsigned int i = 0; i < Q.ptr->ct_cms; ++i)
		column[i] = i;
	for (unsigned int i = 0; i < Q.ptr->ct_rws; ++i)
		row[i] = i;
	for (unsigned int k = 0; k < Q.ptr->ct_rws - 1; ++k){
		FL_POINT max = Q[row[k]][column[k]];
		unsigned int inx_row = row[k], inx_column = column[k];
		for (unsigned int i = k; i < Q.ptr->ct_rws; ++i)
			for (unsigned int j = k; j < Q.ptr->ct_cms; ++j)
				if (std::fabs(Q[row[i]][column[j]]) > std::fabs(max)){
					max = Q[row[i]][column[j]];
					inx_row = i; inx_column = j;
				}
		if (std::fabs(max) < MX_FL_EPS){
			return 0;
		}
		if (column[k] != column[inx_column])
			++count;
		unsigned int tmp = column[k];
		column[k] = column[inx_column];
		column[inx_column] = tmp;
		if (row[k] != row[inx_row])
			++count;
		tmp = row[k];
		row[k] = row[inx_row];
		row[inx_row] = tmp;
		for (unsigned int i = k + 1; i < Q.ptr->ct_rws; ++ i)
			for (unsigned int j = k + 1; j < Q.ptr->ct_cms; ++j){
				Q[row[i]][column[j]] -= (Q[row[i]][column[k]] * Q[row[k]][column[j]]) / Q[row[k]][column[k]];
			}
		for (unsigned int i = k + 1; i < Q.ptr->ct_rws; ++i)
			Q[row[i]][column[k]] = 0;
	}
	FL_POINT result = 1;
	for (unsigned int i = 0; i < Q.ptr->ct_rws; ++i)
		result *= Q[row[i]][column[i]];
	if (count % 2)
		return -result;
	return result;
}

FL_POINT t_matrix::Det(){
	if (ptr == NULL){
		error = ERR_NULL;
		return 0;
	}
	if (ptr->ct_rws != ptr->ct_cms){
		error = ERR_DIMENSION;
		return 0;
	}
	if (ptr->ct_rws == 1)
		return ptr->buff[0];
	if (ptr->ct_rws == 2)
		return (ptr->buff[0] * ptr->buff[3] - ptr->buff[1] * ptr->buff[2]);
	if (ptr->ct_rws == 3){
		FL_POINT *a = ptr->buff;
		return (a[0]*(a[4] * a[8] - a[5] * a[7])
				- a[1]*(a[3] * a[8] - a[5] * a[6])
				+ a[2]*(a[3] * a[7] - a[4] * a[6]));
	}
	if (ptr->ct_rws == 4){
		FL_POINT *a = ptr->buff;
		return (
			a[0]*(a[5]*(a[10] * a[15] - a[11] * a[14]) - a[6]*(a[9] * a[15] - a[11] * a[13]) + a[7]*(a[9] * a[14] - a[10] * a[13]))
			-
			a[1]*(a[4]*(a[10] * a[15] - a[11] * a[14]) - a[6]*(a[8]*a[15] - a[11] * a[12]) + a[7]*(a[8] * a[14] - a[10] * a[12]))
			+
			a[2]*(a[4]*(a[9] * a[15] - a[11] * a[13]) - a[5]*(a[8] * a[15] - a[11] * a[12]) + a[7]*(a[8] * a[13] - a[9] * a[12]))
			-
			a[3]*(a[3]*(a[9] * a[14] - a[10] * a[13]) - a[5]*(a[8] * a[14] - a[10] * a[12]) + a[6]*(a[8] * a[13] - a[9] * a[12]))
		);
	}
	return gauss_det(*this);
}

t_matrix t_matrix::Inv(){
	t_matrix I(ptr->ct_rws);
	for (unsigned int i = 0; i < ptr->ct_rws; ++i)
		I[i][i] = 1;
	t_matrix MAIN(*this);
	unsigned int pos;
	FL_POINT max;
	for(unsigned int k = 0; k < ptr->ct_rws; ++k){
		max = MAIN[k][k];
		pos = k;
		for (unsigned int j = k; j < ptr->ct_rws; ++j)
			if (std::fabs(MAIN[j][k]) > std::fabs(max)){
			    max = MAIN[j][k];
			    pos = j;
			}
		if (std::fabs(max) < MX_FL_EPS)
			return t_matrix();
		for (unsigned int j = 0; j < ptr->ct_rws; ++j){
			float temp = MAIN[k][j];
			MAIN[k][j] = MAIN[pos][j];
			MAIN[pos][j] = temp;
			temp = I[k][j];
			I[k][j] = I[pos][j];
			I[pos][j] = temp;
		}

		for (unsigned int j = 0; j < ptr->ct_rws; ++j){
			if (j == k)
				continue;
			FL_POINT temp = MAIN[j][k] / MAIN[k][k];
			for (unsigned int i = 0; i < ptr->ct_rws; ++i){
				MAIN[j][i] -= MAIN[k][i]*temp;
				I[j][i] -= I[k][i]*temp;
			}
		}
	}

	for (unsigned int i = 0; i < ptr->ct_rws; ++i){
		FL_POINT t = MAIN[i][i];
		MAIN[i][i] /= t;
		for (unsigned int k = 0; k < ptr->ct_rws; ++k)
			I[i][k] /= t;
	}
	return I;
}
/*-------*/
t_vector t_matrix::operator *(const t_vector &src){
	if ((ptr == NULL)||(src.ptr == NULL)){
		error = ERR_NULL;
		t_vector vec;
		return vec;
	}
	if (ptr->ct_cms != src.ptr->ct_lms){
		error = ERR_SIZE;
		t_vector vec;
		return vec;
	}
	t_vector result(ptr->ct_rws, 0);
	for (unsigned int i = 0; i < ptr->ct_rws; ++i)
		for (unsigned int j = 0; j < ptr->ct_cms; ++j)
			result[i] += ptr->buff[i*ptr->ct_rws + j] * src.ptr->buff[j];
	return result;
}
