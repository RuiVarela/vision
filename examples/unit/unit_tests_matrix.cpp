#include "../../source/vs.hpp"

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

static matrix make_matrix(int rows, int cols)
{
    matrix m;
    m.rows = rows;
    m.cols = cols;
    m.data = static_cast<double**>(calloc(size_t(m.rows), sizeof(double *)));
    int i;
    for(i = 0; i < m.rows; ++i) m.data[i] = static_cast<double*>(calloc(size_t(m.cols), sizeof(double)));
    return m;
}

static matrix make_identity_homography()
{
    matrix H = make_matrix(3,3);
    H.data[0][0] = 1;
    H.data[1][1] = 1;
    H.data[2][2] = 1;
    return H;
}

static matrix make_translation_homography(float dx, float dy)
{
    matrix H = make_identity_homography();
    H.data[0][2] = double(dx);
    H.data[1][2] = double(dy);
    return H;
}

static void free_matrix(matrix m)
{
    int i;
    for(i = 0; i < m.rows; ++i) free(m.data[i]);
    free(m.data);
}

static matrix copy_matrix(matrix m)
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

static matrix augment_matrix(matrix m)
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

static matrix make_identity(int rows, int cols)
{
    int i;
    matrix m = make_matrix(rows, cols);
    for(i = 0; i < rows && i < cols; ++i){
        m.data[i][i] = 1;
    }
    return m;
}

static matrix matrix_mult_matrix(matrix a, matrix b)
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

static matrix matrix_sub_matrix(matrix a, matrix b)
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

static matrix transpose_matrix(matrix m)
{
    matrix t;
    t.rows = m.cols;
    t.cols = m.rows;
    t.data = static_cast<double**>(calloc(size_t(t.rows), sizeof(double *)));
    int i, j;
    for(i = 0; i < t.rows; ++i){
        t.data[i] = static_cast<double*>(calloc(size_t(t.cols), sizeof(double)));
        for(j = 0; j < t.cols; ++j){
            t.data[i][j] = m.data[j][i];
        }
    }
    return t;
}

static void scale_matrix(matrix m, double s)
{
    int i, j;
    for(i = 0; i < m.rows; ++i){
        for(j =0 ; j < m.cols; ++j){
            m.data[i][j] *= s;
        }
    }
}

static double *matrix_mult_vector(matrix m, double *v)
{
    double *p = static_cast<double*>(calloc(size_t(m.rows), sizeof(double)));
    int i, j;
    for(i = 0; i < m.rows; ++i){
        for(j = 0; j < m.cols; ++j){
            p[i] += m.data[i][j]*v[j];
        }
    }
    return p;
}

static double *LUP_solve(matrix L, matrix U, int *p, double *b)
{
    int i, j;
    double *c = static_cast<double*>(calloc(size_t(L.rows), sizeof (double)));
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

static matrix matrix_invert(matrix m)
{
    //print_matrix(m);
    matrix none = {0,0,nullptr};
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
            double s = -c.data[i][k];
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

static int* in_place_LUP(matrix m)
{
    int *pivot = static_cast<int*>(calloc(size_t(m.rows), sizeof(int)));
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


static double *sle_solve(matrix A, double *b)
{
    int *p = in_place_LUP(A);
    return LUP_solve(A, A, p, b);
}

static matrix solve_system(matrix M, matrix b)
{
    matrix none = {0,0,nullptr};
    matrix Mt = transpose_matrix(M);
    matrix MtM = matrix_mult_matrix(Mt, M);
    matrix MtMinv = matrix_invert(MtM);
    if(!MtMinv.data) return none;
    matrix Mdag = matrix_mult_matrix(MtMinv, Mt);
    matrix a = matrix_mult_matrix(Mdag, b);
    free_matrix(Mt); free_matrix(MtM); free_matrix(MtMinv); free_matrix(Mdag);
    return a;
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

template<typename T>
static bool same(const vs::MatT<T> &a, const matrix &b)
{
    double const epsilon = 0.005;

    if (a.w != b.cols || a.h != b.rows || a.c != 1)
    {
        printf("Expected %d x %d x %d image, got %d x %d x %d\n", b.cols, b.rows, 1, a.w, a.h, a.c);
        return false;
    }

    for (int y = 0; y < a.h; ++y)
        for (int x = 0; x < a.w; ++x)
            if (!vs::equivalent( double(a(y, x)), b.data[y][x], epsilon))
            {
                printf("Mismatch (%d %d) %f %f\n", x, y, double(a(y, x)), b.data[y][x]);
                return false;
            }


    return true;
}

static void randomize(vs::Mat& a, matrix& b, int rows, int cols) {
    a = vs::Mat(cols, rows);
    b = make_matrix(rows, cols);

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < cols; ++j){
            float value = float(rand() % 100 - 50);
            b.data[i][j] = double(value);
            a(i,j) = value;
        }
    }
}


static void test_basics() {

    matrix t = make_identity(4, 3);
    UTEST(same(vs::Mat::makeIdentity(4, 3), t));
    free_matrix(t);

    t = make_identity(2, 3);
    UTEST(same(vs::Mat::makeIdentity(2, 3), t));
    free_matrix(t);

    t = make_identity(1, 3);
    UTEST(same(vs::Mat::makeIdentity(1, 3), t));
    free_matrix(t);

    t = make_identity(3, 3);
    UTEST(same(vs::Mat::makeIdentity(3, 3), t));
    free_matrix(t);

    t = make_identity(2, 2);
    UTEST(same(vs::Mat::makeIdentity(2, 2), t));
    free_matrix(t);

    t = make_identity_homography();
    UTEST(same(vs::Mat::makeIdentity3x3(), t));
    free_matrix(t);

    t = make_translation_homography(30.0f, 20.0f);
    UTEST(same(vs::Mat::makeTranslation3x3(30.0f, 20.0f), t));
    free_matrix(t);

    t =  make_translation_homography(120.0f, 40.0f);
    UTEST(same(vs::Mat::makeTranslation3x3(120.0f, 40.0f), t));
    free_matrix(t);


    for (int i = 0; i != 10; ++i) {
        vs::Mat a;
        matrix b;
        randomize(a, b, 10, 10);

        matrix m = augment_matrix(b);
        UTEST(same(a.augment(), m));
        free_matrix(m);
    }


    for (int i = 0; i != 10; ++i) {
        vs::Mat a;
        matrix a1;
        randomize(a, a1, 10, 10);

        vs::Mat b;
        matrix b1;
        randomize(b, b1, 10, 10);

        matrix m = matrix_mult_matrix(a1, b1);
        UTEST(same(vs::Mat::mmult(a, b), m));

        free_matrix(m);
        free_matrix(a1);
        free_matrix(b1);
    }


    for (int i = 0; i != 10; ++i) {
        vs::Mat a;
        matrix a1;
        randomize(a, a1, 10, 10);

        vs::Mat b;
        matrix b1;
        randomize(b, b1, 10, 10);

        matrix m = matrix_sub_matrix(a1, b1);
        UTEST(same(vs::Mat::sub(a, b), matrix_sub_matrix(a1, b1)));

        free_matrix(m);
        free_matrix(a1);
        free_matrix(b1);
    }


    for (int i = 0; i != 10; ++i) {
        vs::Mat a;
        matrix a1;
        randomize(a, a1, 10, 10);

        vs::Mat b;
        matrix b1;
        randomize(b, b1, 10, 10);

        matrix m = matrix_mult_matrix(a1, b1);
        UTEST(same(vs::Mat::mmult(a, b), matrix_mult_matrix(a1, b1)));

        free_matrix(m);
        free_matrix(a1);
        free_matrix(b1);
    }


    for (int i = 0; i != 10; ++i) {
        vs::Mat a;
        matrix a1;
        randomize(a, a1, 3, 3);

        matrix m = transpose_matrix(a1);
        UTEST(same(a.transpose(), m));
        free_matrix(m);
        free_matrix(a1);

        randomize(a, a1, 3, 2);
        m = transpose_matrix(a1);
        UTEST(same(a.transpose(), m));
        free_matrix(m);
        free_matrix(a1);

        randomize(a, a1, 2, 3);
        m = transpose_matrix(a1);
        UTEST(same(a.transpose(), m));
        free_matrix(m);
        free_matrix(a1);
    }


    for (int i = 0; i != 10; ++i) {
        vs::Mat a;
        matrix a1;
        randomize(a, a1, 3, 3);

        vs::Mat v;
        matrix v1;
        randomize(v, v1, 3, 1);

        double *pi = static_cast<double*>(calloc(size_t(v1.rows), sizeof(double)));
        for(int i = 0; i != v1.rows; ++i) {
            pi[i] = v1.data[i][0];
        }
        double *po = matrix_mult_vector(a1, pi);
        for(int i = 0; i != v1.rows; ++i) {
            v1.data[i][0] = po[i];
        }
        free(pi);
        free(po);

        UTEST(same(vs::Mat::vmult(a, v), v1));

        free_matrix(v1);
        free_matrix(a1);
    }
}

static void test_invert() {
    for(int i = 0; i < 100; ++i){
        int s = rand() % 4 + 3;

        vs::Mat m;
        matrix m1;
        randomize(m, m1, s, s);


        vs::Mat inv = m.invert();
        matrix inv1 = matrix_invert(m1);
        UTEST(same(inv, inv1));

        vs::Mat res = vs::Mat::mmult(m, inv);
        matrix res1 = matrix_mult_matrix(m1, inv1);
        UTEST(same(res, res1));

        free_matrix(m1);
        free_matrix(inv1);
        free_matrix(res1);
    }
}

static void test_proj_mult()
{
    for (int i = 0; i < 100; ++i)
    {
        vs::Mat m;
        matrix m1;
        randomize(m, m1, 3, 3);

        vs::Mat p;
        matrix p1;
        randomize(p, p1, 3, 1);

        vs::Mat res = vs::Mat::mmult(m, p);
        matrix res1 = matrix_mult_matrix(m1, p1);
        UTEST(same(res, res1));

        free_matrix(m1);
        free_matrix(res1);
    }
}

static void test_matrix_homography() {
    {
        // test project a very simple point translation
        vs::Matches matches;
        vs::Match match;

        match.p.x = 1.0f;
        match.p.y = 1.0f;
        matches.push_back(match);

        match.p.x = 100.0f;
        match.p.y = 1.0f;
        matches.push_back(match);

        match.p.x = 100.0f;
        match.p.y = 20.0f;
        matches.push_back(match);

        match.p.x = 1.0f;
        match.p.y = 20.0f;
        matches.push_back(match);
        for(vs::Match& current : matches) {
            current.q = current.p;
            current.q.x += 100.0f;
            current.q.y += 100.0f;
        }

        vs::Matd H = vs::Matd::makeTranslation3x3(100, 100);
        int inliers = vs::modelInliers(H, matches, 2.0f);
        UTEST(inliers == 4);



        //
        // system solve
        //
        size_t n = matches.size();
        vs::Matd M(8, int(n * 2));
        vs::Matd b(1, int(n * 2));

        // fill in the matrices M and b.
        for (size_t i = 0; i < n; ++i)
        {
            double x = double(matches[i].p.x);
            double xp = double(matches[i].q.x);

            double y = double(matches[i].p.y);
            double yp = double(matches[i].q.y);

            int r = int(i * 2);
            M(r, 0) = x;
            M(r, 1) = y;
            M(r, 2) = 1;
            M(r, 3) = 0;
            M(r, 4) = 0;
            M(r, 5) = 0;
            M(r, 6) = -x * xp;
            M(r, 7) = -y * xp;
            b(r, 0) = xp;

            r++;

            M(r, 0) = 0;
            M(r, 1) = 0;
            M(r, 2) = 0;
            M(r, 3) = x;
            M(r, 4) = y;
            M(r, 5) = 1;
            M(r, 6) = -x * yp;
            M(r, 7) = -y * yp;
            b(r, 0) = yp;
        }

        matrix m1 = make_matrix(M.h, M.w);
        for (int y = 0; y != M.h; ++y)
            for (int x = 0; x != M.w; ++x)
                m1.data[y][x] = M(y,x);

        matrix b1 = make_matrix(b.h, b.w);
        for (int y = 0; y != b.h; ++y)
            for (int x = 0; x != b.w; ++x)
                b1.data[y][x] = b(y,x);

        UTEST(same(M, m1));
        UTEST(same(b, b1));

        matrix a1 = {0,0,nullptr};

        matrix Mt1 = transpose_matrix(m1);
        vs::Matd Mt = M.transpose();
        UTEST(same(M, m1));
        UTEST(same(Mt, Mt1));

        matrix MtM1 = matrix_mult_matrix(Mt1, m1);
        vs::Matd MtM = vs::Matd::mmult(Mt, M);
        UTEST(same(MtM, MtM1));

        matrix MtMinv1 = matrix_invert(MtM1);
        vs::Matd MtMinv = MtM.invert();
        UTEST(same(MtMinv, MtMinv1));

        matrix Mdag1 = {0,0,nullptr};
        if(MtMinv1.data) {
            Mdag1 = matrix_mult_matrix(MtMinv1, Mt1);
            vs::Matd Mdag = vs::Matd::mmult(MtMinv, Mt);
            UTEST(same(Mdag, Mdag1));

            a1 = matrix_mult_matrix(Mdag1, b1);
            vs::Matd a = vs::Matd::mmult(Mdag, b);
            UTEST(same(a, a1));


            vs::Matd H(3, 3);

            H(0,0) = a(0,0);
            H(0,1) = a(1,0);
            H(0,2) = a(2,0);

            H(1,0) = a(3,0);
            H(1,1) = a(4,0);
            H(1,2) = a(5,0);

            H(2,0) = a(6,0);
            H(2,1) = a(7,0);
            H(2,2) = 1.0;

            int inliers = vs::modelInliers(H, matches, 2.0f);
            UTEST(inliers == 4);
        }
        free_matrix(Mt1); free_matrix(MtM1); free_matrix(MtMinv1); free_matrix(Mdag1);

        {
            matrix a1 = solve_system(m1, b1);
            vs::Matd a = vs::Matd::llsSolve(M, b);
            UTEST(same(a, a1));
        }

        free_matrix(m1);
        free_matrix(b1);
        free_matrix(a1);
    }
}

int unit_tests_matrix(int argc, char **argv)
{
    test_basics();
    test_invert();
    test_proj_mult();
    test_matrix_homography();
    return 0;
}
