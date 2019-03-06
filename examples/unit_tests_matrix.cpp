#include "../source/vs.hpp"


//
// Convert Joseph Redmon matrix source to our code
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

typedef struct matrix{
    int rows, cols;
    double **data;
} matrix;

typedef struct LUP{
    matrix *L;
    matrix *U;
    int *P;
    int n;
} LUP;

matrix make_matrix(int rows, int cols)
{
    matrix m;
    m.rows = rows;
    m.cols = cols;
    m.data = static_cast<double**>(calloc(m.rows, sizeof(double *)));
    int i;
    for(i = 0; i < m.rows; ++i) m.data[i] = static_cast<double*>(calloc(m.cols, sizeof(double)));
    return m;
}

matrix make_identity_homography()
{
    matrix H = make_matrix(3,3);
    H.data[0][0] = 1;
    H.data[1][1] = 1;
    H.data[2][2] = 1;
    return H;
}

matrix make_translation_homography(float dx, float dy)
{
    matrix H = make_identity_homography();
    H.data[0][2] = dx;
    H.data[1][2] = dy;
    return H;
}

void free_matrix(matrix m)
{
    int i;
    for(i = 0; i < m.rows; ++i) free(m.data[i]);
    free(m.data);
}

matrix copy_matrix(matrix m)
{
    int i,j;
    matrix c = make_matrix(m.rows, m.cols);
    for(i = 0; i < m.rows; ++i){
        for(j = 0; j < m.cols; ++j){
            c.data[i][j] = m.data[i][j];
        }
    }
    return c;
}

matrix augment_matrix(matrix m)
{
    int i,j;
    matrix c = make_matrix(m.rows, m.cols*2);
    for(i = 0; i < m.rows; ++i){
        for(j = 0; j < m.cols; ++j){
            c.data[i][j] = m.data[i][j];
        }
    }
    for(j = 0; j < m.rows; ++j){
        c.data[j][j+m.cols] = 1;
    }
    return c;
}

matrix make_identity(int rows, int cols)
{
    int i;
    matrix m = make_matrix(rows, cols);
    for(i = 0; i < rows && i < cols; ++i){
        m.data[i][i] = 1;
    }
    return m;
}

matrix matrix_mult_matrix(matrix a, matrix b)
{
    assert(a.cols == b.rows);

    int i, j, k;
    matrix p = make_matrix(a.rows, b.cols);
    for(i = 0; i < p.rows; ++i){
        for(j = 0; j < p.cols; ++j){
            for(k = 0; k < a.cols; ++k){
                p.data[i][j] += a.data[i][k]*b.data[k][j];
            }
        }
    }
    return p;
}

matrix matrix_sub_matrix(matrix a, matrix b)
{
    assert(a.cols == b.cols);
    assert(a.rows == b.rows);
    int i, j;
    matrix p = make_matrix(a.rows, a.cols);
    for(i = 0; i < p.rows; ++i){
        for(j = 0; j < p.cols; ++j){
            p.data[i][j] = a.data[i][j] - b.data[i][j];
        }
    }
    return p;
}

matrix transpose_matrix(matrix m)
{
    matrix t;
    t.rows = m.cols;
    t.cols = m.rows;
    t.data = static_cast<double**>(calloc(t.rows, sizeof(double *)));
    int i, j;
    for(i = 0; i < t.rows; ++i){
        t.data[i] = static_cast<double*>(calloc(t.cols, sizeof(double)));
        for(j = 0; j < t.cols; ++j){
            t.data[i][j] = m.data[j][i];
        }
    }
    return t;
}

void scale_matrix(matrix m, double s)
{
    int i, j;
    for(i = 0; i < m.rows; ++i){
        for(j =0 ; j < m.cols; ++j){
            m.data[i][j] *= s;
        }
    }
}

double *matrix_mult_vector(matrix m, double *v)
{
    double *p = static_cast<double*>(calloc(m.rows, sizeof(double)));
    int i, j;
    for(i = 0; i < m.rows; ++i){
        for(j = 0; j < m.cols; ++j){
            p[i] += m.data[i][j]*v[j];
        }
    }
    return p;
}

void print_matrix(matrix m)
{
    int i, j;
    printf(" __");
    for(j = 0; j < 16*m.cols-1; ++j) printf(" ");
    printf("__ \n");

    printf("|  ");
    for(j = 0; j < 16*m.cols-1; ++j) printf(" ");
    printf("  |\n");

    for(i = 0; i < m.rows; ++i){
        printf("|  ");
        for(j = 0; j < m.cols; ++j){
            printf("%15.7f ", m.data[i][j]);
        }
        printf(" |\n");
    }
    printf("|__");
    for(j = 0; j < 16*m.cols-1; ++j) printf(" ");
    printf("__|\n");
}

double *LUP_solve(matrix L, matrix U, int *p, double *b)
{
    int i, j;
    double *c = static_cast<double*>(calloc(L.rows, sizeof (double)));
    for(i = 0; i < L.rows; ++i){
        int pi = p[i];
        c[i] = b[pi];
        for(j = 0; j < i; ++ j){
            c[i] -= L.data[i][j]*c[j];
        }
    }
    for(i = U.rows-1; i >= 0; --i){
        for(j = i+1; j < U.cols; ++j){
            c[i] -= U.data[i][j]*c[j];
        }
        c[i] /= U.data[i][i];
    }
    return c;
}

matrix matrix_invert(matrix m)
{
    //print_matrix(m);
    matrix none = {0};
    if(m.rows != m.cols){
        fprintf(stderr, "Matrix not square\n");
        return none;
    }
    matrix c = augment_matrix(m);
    //print_matrix(c);

    int i, j, k;
    for(k = 0; k < c.rows; ++k){
        double p = 0.;
        int index = -1;
        for(i = k; i < c.rows; ++i){
            double val = fabs(c.data[i][k]);
            if(val > p){
                p = val;
                index = i;
            }
        }
        if(index == -1){
            fprintf(stderr, "Can't do it, sorry!\n");
            free_matrix(c);
            return none;
        }

        double *swap = c.data[index];
        c.data[index] = c.data[k];
        c.data[k] = swap;

        double val = c.data[k][k];
        c.data[k][k] = 1;
        for(j = k+1; j < c.cols; ++j){
            c.data[k][j] /= val;
        }
        for(i = k+1; i < c.rows; ++i){
            float s = -c.data[i][k];
            c.data[i][k] = 0;
            for(j = k+1; j < c.cols; ++j){
                c.data[i][j] +=  s*c.data[k][j];
            }
        }
    }
    for(k = c.rows-1; k > 0; --k){
        for(i = 0; i < k; ++i){
            double s = -c.data[i][k];
            c.data[i][k] = 0;
            for(j = k+1; j < c.cols; ++j){
                c.data[i][j] += s*c.data[k][j];
            }
        }
    }
    //print_matrix(c);
    matrix inv = make_matrix(m.rows, m.cols);
    for(i = 0; i < m.rows; ++i){
        for(j = 0; j < m.cols; ++j){
            inv.data[i][j] = c.data[i][j+m.cols];
        }
    }
    free_matrix(c);
    //print_matrix(inv);
    return inv;
}

int* in_place_LUP(matrix m)
{
    int *pivot = static_cast<int*>(calloc(m.rows, sizeof(int)));
    if(m.rows != m.cols){
        fprintf(stderr, "Matrix not square\n");
        return 0;
    }

    int i, j, k;
    for(k = 0; k < m.rows; ++k) pivot[k] = k;
    for(k = 0; k < m.rows; ++k){
        double p = 0.;
        int index = -1;
        for(i = k; i < m.rows; ++i){
            double val = fabs(m.data[i][k]);
            if(val > p){
                p = val;
                index = i;
            }
        }
        if(index == -1){
            fprintf(stderr, "Matrix is singular\n");
            return 0;
        }

        int swapi = pivot[k];
        pivot[k] = pivot[index];
        pivot[index] = swapi;

        double *swap = m.data[index];
        m.data[index] = m.data[k];
        m.data[k] = swap;

        for(i = k+1; i < m.rows; ++i){
            m.data[i][k] = m.data[i][k]/m.data[k][k];
            for(j = k+1; j < m.cols; ++j){
                m.data[i][j] -= m.data[i][k] * m.data[k][j];
            }
        }
    }
    return pivot;
}

matrix random_matrix(int rows, int cols)
{
    matrix m = make_matrix(rows, cols);
    int i, j;
    for(i = 0; i < rows; ++i){
        for(j = 0; j < cols; ++j){
            m.data[i][j] = rand()%100 - 50;
        }
    }
    return m;
}

double *sle_solve(matrix A, double *b)
{
    int *p = in_place_LUP(A);
    return LUP_solve(A, A, p, b);
}

matrix solve_system(matrix M, matrix b)
{
    matrix none = {0};
    matrix Mt = transpose_matrix(M);
    matrix MtM = matrix_mult_matrix(Mt, M);
    matrix MtMinv = matrix_invert(MtM);
    if(!MtMinv.data) return none;
    matrix Mdag = matrix_mult_matrix(MtMinv, Mt);
    matrix a = matrix_mult_matrix(Mdag, b);
    free_matrix(Mt); free_matrix(MtM); free_matrix(MtMinv); free_matrix(Mdag);
    return a;
}

void test_matrix()
{
    int i;
    for(i = 0; i < 100; ++i){
        int s = rand()%4 + 3;
        matrix m = random_matrix(s, s);
        matrix inv = matrix_invert(m);
        matrix res = matrix_mult_matrix(m, inv);
        print_matrix(res);
    }
}

//
// Test Start
//


#define UTEST(EX) \
{\
    if(!(EX)) {\
        fprintf(stderr, "failed: [%s] testing [%s] in %s, line %d\n", __FUNCTION__, #EX, __FILE__, __LINE__);\
    }\
}\

bool same(const vs::Mat &a, const matrix &b)
{
    float const epsilon = 0.005f;

    if (a.cols != a.w)
    {
        printf("(a.cols != a.w) %d x %d\n", a.cols, a.w);
        return false;
    }

    if (a.rows != a.h)
    {
        printf("(a.rows != a.h) %d x %d\n", a.rows, a.h);
        return false;
    }

    if (a.w != b.cols || a.h != b.rows || a.c != 1)
    {
        printf("Expected %d x %d x %d image, got %d x %d x %d\n", b.cols, b.rows, 1, a.w, a.h, a.c);
        return false;
    }

    for (int y = 0; y < a.h; ++y)
        for (int x = 0; x < a.w; ++x)
            if (!vs::equivalent(a.m(y, x), float(b.data[y][x]), epsilon))
            {
                printf("Mismatch (%d %d) %f %f\n", x, y, a.m(y, x), float(b.data[y][x]));
                return false;
            }


    return true;
}

void randomize(vs::Mat& a, matrix& b, int rows, int cols) {
    a = vs::Mat::make(rows, cols);
    b = make_matrix(rows, cols);

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < cols; ++j){
            float value = rand() % 100 - 50;
            b.data[i][j] = double(value);
            a.m(i,j) = value;
        }
    }
}


void test_basics() {
    UTEST(same(vs::Mat::makeIdentity(4, 3), make_identity(4, 3)));
    UTEST(same(vs::Mat::makeIdentity(2, 3), make_identity(2, 3)));
    UTEST(same(vs::Mat::makeIdentity(1, 3), make_identity(1, 3)));
    UTEST(same(vs::Mat::makeIdentity(3, 3), make_identity(3, 3)));
    UTEST(same(vs::Mat::makeIdentity(2, 2), make_identity(2, 2)));
    UTEST(same(vs::Mat::makeIdentity3x3(), make_identity_homography()));
    UTEST(same(vs::Mat::makeTranslation3x3(30.0f, 20.0f), make_translation_homography(30.0f, 20.0f)));
    UTEST(same(vs::Mat::makeTranslation3x3(120.0f, 40.0f), make_translation_homography(120.0f, 40.0f)));

    for (int i = 0; i != 10; ++i) {
        vs::Mat a;
        matrix b;
        randomize(a, b, 10, 10);
        UTEST(same(vs::Mat::augment(a), augment_matrix(b)));
    }


    for (int i = 0; i != 10; ++i) {
        vs::Mat a;
        matrix a1;
        randomize(a, a1, 10, 10);

        vs::Mat b;
        matrix b1;
        randomize(b, b1, 10, 10);
        UTEST(same(vs::Mat::mmult(a, b), matrix_mult_matrix(a1, b1)));
    }

    for (int i = 0; i != 10; ++i) {
        vs::Mat a;
        matrix a1;
        randomize(a, a1, 10, 10);

        vs::Mat b;
        matrix b1;
        randomize(b, b1, 10, 10);
        UTEST(same(vs::Mat::sub(a, b), matrix_sub_matrix(a1, b1)));
    }
}


int unit_tests_matrix(int argc, char **argv)
{
    test_basics();

    return 0;
}
