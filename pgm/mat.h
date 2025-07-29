#ifndef MAT_H
#define MAT_H
#include <vector>
#include <cassert>

template <typename T>
class mat {
    int rows_, cols_;
    std::vector<T> data_;

public:
    mat(int rows=0, int cols=0) :
        rows_(rows), cols_(cols), data_(rows*cols) {}

    int rows() const {return rows_;}
    int cols() const {return cols_;}
    int size() const {return rows_*cols_;}
    int rawsize() const {return rows_*cols_*sizeof(T);}

    T& operator()(int r, int c) {
        assert(r>=0 && c>=0 && r<rows_ && c<cols_);
        return data_[r*cols_+c];
    }
    const T& operator()(int r, int c) const {
        assert(r>=0 && c>=0 && r<rows_ && c<cols_);
        return data_[r*cols_+c];
    }
    T& operator[](int pos) {
        assert(pos < size());
        return data_[pos];
    }
    const T& operator[](int pos) const {
        assert(pos < size());
        return data_[pos];
    }

    void resize(int r, int c) {
        assert(r>=0 && c>=0);
        rows_ = r;
        cols_ = c;
        data_.resize(rows_*cols_);
    }

    char* rawdata() {return reinterpret_cast<char*>(data_.data());}
    const char* rawdata() const {return reinterpret_cast<const char*>(data_.data());}

    auto begin() {return data_.begin();}
    auto end() {return data_.end();}
    auto begin() const {return data_.begin();}
    auto end() const {return data_.end();}i
};

#endif //MAT_H
