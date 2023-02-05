/*
Copyright (c) 2023 William M.H.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef MATRIX_H
#define MATRIX_H

#include <initializer_list>
#include <iostream>
#include <vector>

namespace matrix {
#ifndef MATRIX_NOTHROW
    enum __error_code {
        ERR_ROW_RANGE,
        ERR_COL_RANGE,
        ERR_INVALID_SIZE,
        ERR_INCOMPATIBLE,
        ERR_NOT_SQUARE
    };

    const char __error_messages[][53] = {
        [ERR_ROW_RANGE] = "row index out of range [0, rows - 1].",
        [ERR_COL_RANGE] = "column index out of range [0, columns - 1].",
        [ERR_INVALID_SIZE] = "invalid matrix dimensions [rows < 1 OR columns < 1].",
        [ERR_INCOMPATIBLE] = "incompatible matrix dimensions.",
        [ERR_NOT_SQUARE] = "matrix must be square [rows = columns]."
    };
#endif

    template<typename T> 
    class matrix {
    private:
        size_t m_rows;
        size_t m_columns;
        std::vector<T> m_data;

    public:
        matrix(size_t rows, size_t columns, T fill = T()) {
#ifndef MATRIX_NOTHROW
            if ((rows < 1) || (columns < 1)) {
                throw std::runtime_error(__error_messages[ERR_INVALID_SIZE]);
            }
#endif
            m_rows = rows;
            m_columns = columns;
            m_data.resize(rows * columns);

            for (size_t index = 0; index < rows * columns; index++) {
                m_data.data()[index] = fill;
            }
        }

        ~matrix() {

        }

        size_t rows() {
            size_t result = m_rows;
            return result;
        }

        size_t columns() {
            size_t result = m_columns;
            return result;
        }

        matrix<T> row_vector(size_t row) {
#ifndef MATRIX_NOTHROW
            if ((row < 0) || (row >= m_rows)) {
                throw std::runtime_error(__error_messages[ERR_ROW_RANGE]);
            }
#endif
            matrix<T> result(1, m_columns);

            for (size_t column = 0; column < m_columns; column++) {
                result(0, column) = (*this)(row, column);
            }

            return result;
        }

        matrix<T> column_vector(size_t column) {
#ifndef MATRIX_NOTHROW
            if ((column < 0) || (column >= m_columns)) {
                throw std::runtime_error(__error_messages[ERR_COL_RANGE]);
            }
#endif
            matrix<T> result(m_columns, 1);

            for (size_t row = 0; row < m_rows; row++) {
                result(row, 0) = (*this)(row, column);
            }

            return result;
        }

        template <typename L>
        matrix<T>& transform(L&& lambda) {
            matrix<T>& result = (*this);

            for (size_t row = 0; row < m_rows; row++) {
                for (size_t column = 0; column < m_columns; column++) {
                    lambda(row, column, result(row, column));
                }
            }

            return result;
        }

        T& operator()(size_t row, size_t column) {
#ifndef MATRIX_NOTHROW
            if ((row < 0) || (row >= m_rows)) {
                throw std::runtime_error(__error_messages[ERR_ROW_RANGE]);
            }

            if ((column < 0) || (column >= m_columns)) {
                throw std::runtime_error(__error_messages[ERR_COL_RANGE]);
            }
#endif
            T& result = m_data[row * m_columns + column];
            return result;
        }

        matrix<T>& operator=(std::initializer_list<T> operand) {
#ifndef MATRIX_NOTHROW
            if (m_rows * m_columns != operand.size()) {
                throw std::runtime_error(__error_messages[ERR_INCOMPATIBLE]);
            }
#endif
            matrix<T>& result = (*this);
            result.m_data = operand;
            return result;
        }

        matrix<T> operator+(matrix<T>& operand) {
#ifndef MATRIX_NOTHROW            
            if ((m_rows != operand.m_rows) || (m_columns != operand.m_columns)) {
                throw std::runtime_error(__error_messages[ERR_INCOMPATIBLE]);
            }
#endif
            matrix<T> result(m_rows, m_columns);

            for (size_t row = 0; row < m_rows; row++) {
                for (size_t column = 0; column < m_columns; column++) {
                    result(row, column) = (*this)(row, column) + operand(row, column);
                }
            }

            return result;
        }

        matrix<T> operator-(matrix<T>& operand) {
#ifndef MATRIX_NOTHROW
            if ((m_rows != operand.m_rows) || (m_columns != operand.m_columns)) {
                throw std::runtime_error(__error_messages[ERR_INCOMPATIBLE]);
            }
#endif
            matrix<T> result(m_rows, m_columns);

            for (size_t row = 0; row < m_rows; row++) {
                for (size_t column = 0; column < m_columns; column++) {
                    result(row, column) = (*this)(row, column) - operand(row, column);
                }
            }

            return result;
        }

        matrix<T> operator-() {
            matrix<T> result(m_rows, m_columns);

            for (size_t row = 0; row < m_rows; row++) {
                for (size_t column = 0; column < m_columns; column++) {
                    result(row, column) = -result(row, column);
                }
            }

            return result;
        }

        matrix<T> operator*(T operand) {
            matrix<T> result(m_rows, m_columns);

            for (size_t row = 0; row < m_rows; row++) {
                for (size_t column = 0; column < m_columns; column++) {
                    result(row, column) = result(row, column) + operand;
                }
            }

            return result;
        }

        matrix<T> operator*(matrix<T> operand) {
#ifndef MATRIX_NOTHROW
            if (m_columns != operand.m_rows) {
                throw std::runtime_error(__error_messages[ERR_INCOMPATIBLE]);
            }
#endif
            matrix<T> result(m_rows, operand.m_columns);

            for (size_t row = 0; row < m_rows; row++) {
                for (size_t column = 0; column < operand.m_columns; column++) {
                    for (size_t element = 0; element < m_columns; element++) {
                        result(row, column) = result(row, column) + (*this)(row, element) * operand(element, column);
                    }
                }
            }

            return result;
        }

        matrix<T> transpose() {
            matrix<T> result(m_columns, m_rows);

            for (size_t row = 0; row < m_rows; row++) {
                for (size_t column = 0; column < m_columns; column++) {
                    result(column, row) = (*this)(row, column);
                }
            }

            return result;
        }

        matrix<T> minor(size_t at_row, size_t at_column) {
            matrix<T> result(m_rows - 1, m_columns - 1);
            size_t index = 0;

            for (size_t row = 0; row < m_rows; row++) {
                for (size_t column = 0; column < m_columns; column++) {
                    if (row == at_row) { continue; }
                    if (column == at_column) { continue; }
                    result.m_data[index++] = (*this)(row, column);
                }
            }

            return result;
        }

        T determinant() {
#ifndef MATRIX_NOTHROW
            if (m_rows != m_columns) {
                throw std::runtime_error(__error_messages[ERR_NOT_SQUARE]);
            }
#endif
            T result = T();

            if (m_rows == 1) {
                result = (*this)(0, 0);
            } else if (m_rows == 2) {
                result = (*this)(0, 0) * (*this)(1, 1) - (*this)(1, 0) * (*this)(0, 1);
            } else {
                for (size_t row = 0; row < m_rows; row++) {
                    matrix<T> sub_matrix = (*this).minor(row, 0);
                    T sub_result = (*this)(row, 0) * sub_matrix.determinant();

                    if (row % 2 == 0) {
                        result = result + sub_result;
                    } else {
                        result = result - sub_result;
                    }
                }
            }

            return result;
        }
    };
}

#endif