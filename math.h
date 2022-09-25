#ifndef MATH_H
#define MATH_H
#include <math.h>

#define Pi 3.141592f

union v2
{
    struct
    {
        real32 X;
        real32 Y;
    };
    
    struct
    {
        real32 u;
        real32 v;
    };
};

union v3
{
    struct
    {
        real32 X;
        real32 Y;
        real32 Z;
    };
    
    v3 operator+(v3 array);
    v3 operator-(v3 array);
    v3 operator*(real32 scale);
    v3 operator*(v3 array);
    real32 operator[](int index)
    {
        return 0.0f;
    }
    void operator+=(v3 array);
    void operator-=(v3 array);
};

union v4
{
    struct
    {
        real32 X;
        real32 Y;
        real32 Z;
        real32 W;
    };
    
    struct
    {
        uint32 R;
        uint32 G;
        uint32 B;
        uint32 A;
    };
    
    real32 E[4];
    
    v4 operator+(v4 vector);
    v4 operator-(v4 vector);
    v4 operator*(real32 scale);
    v4 operator*(v4 vector);
    real32& operator[](uint32 index);
};

union Matrix
{
    struct
    {
        v4 a1;
        v4 a2;
        v4 a3;
        v4 a4;
    };
    
    v4 E[4];
    
    Matrix operator*(Matrix mat);
    
    v4& operator[](uint32 index);
};

inline v2
V2(real32 X, real32 Y)
{
    v2 Result;
    Result.X = X;
    Result.Y = Y;
    
    return Result;
}

inline v3
V3(real32 X, real32 Y, real32 Z)
{
    v3 Result;
    Result.X = X;
    Result.Y = Y;
    Result.Z = Z;
    
    return Result;
}

inline v3 
v3::operator+(v3 array) 
{
    v3 Result;
    Result.X = X + array.X;
    Result.Y = Y + array.Y;
    Result.Z = Z + array.Z;
    
    return Result;
}

inline void 
v3::operator+=(v3 array) 
{
    X = X + array.X;
    Y = Y + array.Y;
    Z = Z + array.Z;
}

inline v3 
v3::operator-(v3 vector)
{
    v3 Result;
    Result.X = X - vector.X;
    Result.Y = Y - vector.Y;
    Result.Z = Z - vector.Z;
    
    return Result;
}

inline void 
v3::operator-=(v3 array) 
{
    X = X - array.X;
    Y = Y - array.Y;
    Z = Z - array.Z;
}

inline v3 
v3::operator*(real32 scale)
{
    v3 Result;
    Result.X = scale*X;
    Result.Y = scale*Y;
    Result.Z = scale*Z;
    
    return Result;
}

inline v3
v3::operator*(v3 array)
{
    v3 Result;
    // NOTE: cross-product
    Result.X = Y*array.Z - Z*array.Y;
    Result.Y = Z*array.X - X*array.Z;
    Result.Z = X*array.Y - Y*array.X;
    
    return Result;
}

inline v3
Normalize(v3 *vector)
{
    v3 Result;
    real32 lengthSquare = vector->X*vector->X + 
        vector->Y*vector->Y + vector->Z*vector->Z;
    
    // NOTE: fast square root / intrinsics
    real32 length = sqrt(lengthSquare);
    Result.X      = vector->X/length;
    Result.Y      = vector->Y/length;
    Result.Z      = vector->Z/length;
    
    return Result;
}

inline v3
Normalize(real32 X, real32 Y, real32 Z)
{
    v3 Result;
    real32 lengthSquare = X*X + Y*Y + Z*Z;
    
    // NOTE: fast square root / intrinsics
    real32 length = sqrt(lengthSquare);
    Result.X      = X/length;
    Result.Y      = Y/length;
    Result.Z      = Z/length;
    
    return Result;
}

inline real32
Dot(v3 *vector1, v3 *vector2)
{
    real32 Result = vector1->X*vector2->X + 
        vector1->Y*vector2->Y + vector1->Z*vector2->Z;
    
    return Result;
}

inline bool32
CompareGT(v3 *A, v3 *B)
{
    // NOTE: idea for branchless, A - B then applies 3 masks to the sign bit
    return A->X > B->X && A->Y > B->Y && A->Z > B->Z;
}

inline bool32
CompareLT(v3 *A, v3 *B)
{
    return A->X < B->X && A->Y < B->Y && A->Z < B->Z;
}

inline v4
V4(real32 X, real32 Y, real32 Z, real32 W)
{
    v4 vector;
    vector.X = X;
    vector.Y = Y;
    vector.Z = Z;
    vector.W = W;
    
    return vector;
}

inline v4
V4(v3 *array, real32 W)
{
    v4 vector;
    vector.X = array->X;
    vector.Y = array->Y;
    vector.Z = array->Z;
    vector.W = W;
    
    return vector;
}

inline v4 
v4::operator*(real32 scale)
{
    v4 Result;
    
    Result.X = X*scale;
    Result.Y = Y*scale;
    Result.Z = Z*scale;
    Result.W = W*scale;
    
    return Result;
}

inline v4
v4::operator*(v4 vector)
{
    v4 Result;
    // NOTE: NOT cross nor dot product, component x component
    Result.X = X*vector.X;
    Result.Y = Y*vector.Y;
    Result.Z = Z*vector.Z;
    Result.W = W*vector.W;
    
    return Result;
}

inline v4 
v4::operator+(v4 vector)
{
    v4 Result;
    
    Result.X = X + vector.X;
    Result.Y = Y + vector.Y;
    Result.Z = Z + vector.Z;
    Result.W = W + vector.W;
    
    return Result;
}

inline v4 
v4::operator-(v4 vector)
{
    v4 Result;
    
    Result.X = X - vector.X;
    Result.Y = Y - vector.Y;
    Result.Z = Z - vector.Z;
    Result.W = W - vector.W;
    
    return Result;
}

inline real32&
v4::operator[](uint32 Index)
{
    return E[Index];
}

inline v4
Normalize(v4 *vector)
{
    v4 Result;
    real32 lengthSquare = vector->X*vector->X + vector->Y*vector->Y + vector->Z*vector->Z + vector->W*vector->W;
    
    real32 length = sqrt(lengthSquare);
    Result.X = vector->X/length;
    Result.Y = vector->Y/length;
    Result.Z = vector->Z/length;
    Result.W = vector->W/length;
    
    return Result;
}

inline v4&
Matrix::operator[](uint32 Index)
{
    return E[Index];
}

inline Matrix
MatrixZero()
{
    Matrix Result = {};
    return Result;
}

inline Matrix
MatrixIdentity()
{
    Matrix Result = {};
    
    Result[0][0] = 1.0f;
    Result[1][1] = 1.0f;
    Result[2][2] = 1.0f;
    Result[3][3] = 1.0f;
    
    return Result;
}

inline Matrix
Transpose(Matrix *mat)
{
    Matrix Result;
    
    Result[0] = V4(mat->E[0][0], mat->E[1][0], mat->E[2][0], mat->E[3][0]);
    Result[1] = V4(mat->E[0][1], mat->E[1][1], mat->E[2][1], mat->E[3][1]);
    Result[2] = V4(mat->E[0][2], mat->E[1][2], mat->E[2][2], mat->E[3][2]);
    Result[3] = V4(mat->E[0][3], mat->E[1][3], mat->E[2][3], mat->E[3][3]);
    
    return Result;
}

inline Matrix 
Matrix::operator*(Matrix mat)
{
    // TODO: Change to pointer instead
    Matrix Result;
    
    real32 X11 = E[0][0]*mat[0][0] + E[0][1]*mat[1][0] + E[0][2]*mat[2][0] + E[0][3]*mat[3][0];
    real32 X12 = E[0][0]*mat[0][1] + E[0][1]*mat[1][1] + E[0][2]*mat[2][1] + E[0][3]*mat[3][1];
    real32 X13 = E[0][0]*mat[0][2] + E[0][1]*mat[1][2] + E[0][2]*mat[2][2] + E[0][3]*mat[3][2];
    real32 X14 = E[0][0]*mat[0][3] + E[0][1]*mat[1][3] + E[0][2]*mat[2][3] + E[0][3]*mat[3][3];
    real32 X21 = E[1][0]*mat[0][0] + E[1][1]*mat[1][0] + E[1][2]*mat[2][0] + E[1][3]*mat[3][0];
    real32 X22 = E[1][0]*mat[0][1] + E[1][1]*mat[1][1] + E[1][2]*mat[2][1] + E[1][3]*mat[3][1];
    real32 X23 = E[1][0]*mat[0][2] + E[1][1]*mat[1][2] + E[1][2]*mat[2][2] + E[1][3]*mat[3][2];
    real32 X24 = E[1][0]*mat[0][3] + E[1][1]*mat[1][3] + E[1][2]*mat[2][3] + E[1][3]*mat[3][3];
    real32 X31 = E[2][0]*mat[0][0] + E[2][1]*mat[1][0] + E[2][2]*mat[2][0] + E[2][3]*mat[3][0];
    real32 X32 = E[2][0]*mat[0][1] + E[2][1]*mat[1][1] + E[2][2]*mat[2][1] + E[2][3]*mat[3][1];
    real32 X33 = E[2][0]*mat[0][2] + E[2][1]*mat[1][2] + E[2][2]*mat[2][2] + E[2][3]*mat[3][2];
    real32 X34 = E[2][0]*mat[0][3] + E[2][1]*mat[1][3] + E[2][2]*mat[2][3] + E[2][3]*mat[3][3];
    real32 X41 = E[3][0]*mat[0][0] + E[3][1]*mat[1][0] + E[3][2]*mat[2][0] + E[3][3]*mat[3][0];
    real32 X42 = E[3][0]*mat[0][1] + E[3][1]*mat[1][1] + E[3][2]*mat[2][1] + E[3][3]*mat[3][1];
    real32 X43 = E[3][0]*mat[0][2] + E[3][1]*mat[1][2] + E[3][2]*mat[2][2] + E[3][3]*mat[3][2];
    real32 X44 = E[3][0]*mat[0][3] + E[3][1]*mat[1][3] + E[3][2]*mat[2][3] + E[3][3]*mat[3][3];
    
    Result[0] = V4( X11, X12, X13, X14 );
    Result[1] = V4( X21, X22, X23, X24 );
    Result[2] = V4( X31, X32, X33, X34 );
    Result[3] = V4( X41, X42, X43, X44 );
    
    return Result;
}

inline void
Scaling(Matrix *mat, real32 ScaleX, real32 ScaleY, real32 ScaleZ)
{
    Matrix ScaleMatrix;
    
    ScaleMatrix[0] = V4( ScaleX,   0.0f,   0.0f, 0.0f );
    ScaleMatrix[1] = V4(   0.0f, ScaleY,   0.0f, 0.0f );
    ScaleMatrix[2] = V4(   0.0f,   0.0f, ScaleZ, 0.0f );
    ScaleMatrix[3] = V4(   0.0f,   0.0f,   0.0f, 1.0f );
    
    *mat = ScaleMatrix * *mat;
}

inline void
RotationX(Matrix *mat, real32 AngleRadian)
{
    Matrix RotationMatrix;
    
    RotationMatrix[0] = V4( 1.0f,             0.0f,              0.0f, 0.0f );
    RotationMatrix[1] = V4( 0.0f,  cos(AngleRadian), sin(AngleRadian), 0.0f );
    RotationMatrix[2] = V4( 0.0f, -sin(AngleRadian), cos(AngleRadian), 0.0f );
    RotationMatrix[3] = V4( 0.0f,              0.0f,             0.0f, 1.0f );
    
    *mat = *mat * RotationMatrix;
}

inline void
RotationY(Matrix *mat, real32 AngleRadian)
{
    Matrix RotationMatrix;
    
    RotationMatrix[0] = V4( cos(AngleRadian), 0.0f, -sin(AngleRadian), 0.0f );
    RotationMatrix[1] = V4(             0.0f, 1.0f,              0.0f, 0.0f );
    RotationMatrix[2] = V4( sin(AngleRadian), 0.0f,  cos(AngleRadian), 0.0f );
    RotationMatrix[3] = V4(             0.0f, 0.0f,              0.0f, 1.0f );
    
    *mat = *mat * RotationMatrix;
}

inline void
RotationZ(Matrix *mat, real32 AngleRadian)
{
    Matrix RotationMatrix;
    
    RotationMatrix[0] = V4(  cos(AngleRadian), sin(AngleRadian), 0.0f, 0.0f );
    RotationMatrix[1] = V4( -sin(AngleRadian), cos(AngleRadian), 0.0f, 0.0f );
    RotationMatrix[2] = V4(              0.0f,             0.0f, 1.0f, 0.0f );
    RotationMatrix[3] = V4(              0.0f,             0.0f, 0.0f, 1.0f );
    
    *mat = *mat * RotationMatrix;
}

inline void
RotationAxisMat(Matrix *mat, v3 Axis, real32 AngleRadian)
{
    Matrix RotationMatrix;
}

inline void
Translation(Matrix *mat, real32 Tx, real32 Ty, real32 Tz)
{
    Matrix Translation;
    
    Translation[0] = V4( 1.0f, 0.0f, 0.0f, Tx );
    Translation[1] = V4( 0.0f, 1.0f, 0.0f, Ty );
    Translation[2] = V4( 0.0f, 0.0f, 1.0f, Tz );
    Translation[3] = V4( 1.0f, 1.0f, 1.0f, 1.0f );
    
    *mat = Translation * *mat;
}

inline v4
ProductMatrixByV4(Matrix *mat, v4 *vector)
{
    v4 Result;
    Result.X = mat->E[0][0]*vector->X + mat->E[0][1]*vector->Y + mat->E[0][3]*vector->Z + mat->E[0][3]*vector->W;
    Result.Y = mat->E[1][0]*vector->X + mat->E[1][1]*vector->Y + mat->E[1][2]*vector->Z + mat->E[1][3]*vector->W;
    Result.Z = mat->E[2][0]*vector->X + mat->E[2][1]*vector->Y + mat->E[2][2]*vector->Z + mat->E[2][3]*vector->W;
    Result.W = mat->E[3][0]*vector->X + mat->E[3][1]*vector->Y + mat->E[3][2]*vector->Z + mat->E[3][3]*vector->W;
    
    return Result;
}

inline v3
TransformCoordV3(Matrix *mat, v3 vector)
{
    v4 Result;
    v4 tempVector = V4( vector.X, vector.Y, vector.Z, 1.0);
    Result = ProductMatrixByV4(mat, &tempVector);
    
    v3 ReturnedResult = { Result.X, Result.Y, Result.Z };
    return ReturnedResult;
}

inline real32
GetDistanceSquareV3(v3 *vector)
{
    real32 Result = vector->X*vector->X + vector->Y*vector->Y + vector->Z*vector->Z;
    return Result;
}

inline real32
Determinant3X3(v3 mat[3])
{
    real32 A = mat[0].X;
    real32 B = mat[0].Y;
    real32 C = mat[0].Z;
    real32 D = mat[1].X;
    real32 E = mat[1].Y;
    real32 F = mat[1].Z;
    real32 G = mat[2].X;
    real32 H = mat[2].Y;
    real32 I = mat[2].Z;
    return A*E*I + B*F*G + C*D*H - C*E*G - B*D*I - A*F*H;
}

inline Matrix
InvMatrix(real32 *Determinant, Matrix *mat)
{
    v3 TempMat[3] = 
    {
        V3(mat->E[1][1], mat->E[1][2], mat->E[1][3]),
        V3(mat->E[2][1], mat->E[2][2], mat->E[2][3]),
        V3(mat->E[3][1], mat->E[3][2], mat->E[3][3])
    };
    real32 A = Determinant3X3(TempMat);
    TempMat[0] = V3(mat->E[1][0], mat->E[1][1], mat->E[1][2]);
    TempMat[1] = V3(mat->E[2][0], mat->E[2][1], mat->E[2][2]);
    TempMat[2] = V3(mat->E[3][0], mat->E[3][1], mat->E[3][2]);
    real32 B = Determinant3X3(TempMat);
    TempMat[0] = V3(mat->E[1][0], mat->E[1][1], mat->E[1][3]);
    TempMat[1] = V3(mat->E[2][0], mat->E[2][1], mat->E[2][3]);
    TempMat[2] = V3(mat->E[3][0], mat->E[3][1], mat->E[3][3]);
    real32 C = Determinant3X3(TempMat);
    TempMat[0] = V3(mat->E[1][0], mat->E[1][1], mat->E[1][2]);
    TempMat[1] = V3(mat->E[2][0], mat->E[2][1], mat->E[2][2]);
    TempMat[2] = V3(mat->E[3][0], mat->E[3][1], mat->E[3][2]);
    real32 D = Determinant3X3(TempMat);
    
    // NOTE: Case det = 0
    
    *Determinant = mat->E[0][0]*A - mat->E[0][1]*B + mat->E[0][3]*C - mat->E[0][4]*D;
    
    Matrix Result;
    if(*Determinant == 0)
    {
        return Result;
    }
    
    real32 DetInv = 1.0f / *Determinant;
    
    Result = Transpose(mat);
    Result[0] = Result[0] * DetInv;
    Result[1] = Result[1] * DetInv;
    Result[2] = Result[2] * DetInv;
    Result[3] = Result[3] * DetInv;
    
    return Result;
}

inline v4
v4Reciprocal(v4 *vect)
{
    v4 Result;
    
    Result.X = 1.0f / vect->X;
    Result.Y = 1.0f / vect->Y;
    Result.Z = 1.0f / vect->Z;
    Result.W = 1.0f / vect->W;
    
    return Result;
}

// MAX and MIN for real32 to be calls in intrinsics 
// for now just plain if statements
inline real32
Min(real32 A, real32 B)
{
    if(A < B) return A;
    
    return B;
}

inline real32
Max(real32 A, real32 B)
{
    if(A < B) return B;
    
    return A;
}

#endif //MATH_H
