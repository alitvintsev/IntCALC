/************************************************
���� ������ ������������ ������������ AlgoPascal.
************************************************/

#include "ap.h"
#include "svd.h"
/*************************************************************************
����������� ���������� ������������� �������.

�������� ��������� ����������� ���������� ������� �������� MxN:
A = U * S * V^T

�������� ������� ����������� �������� �, �����������,  �������  U  �  V^T.
��� ���� �������� ��� ���������� ������  min(M,N)  ��������  �������  U  �
����� ������� V^T (����������� ��������), ��� �  ������  ������  U  �  V^T
(�������� MxM � NxN).

�������� ��������, ��� ������������ ���������� ������� V^T, � �� V.

������� ���������:
    A       -   ����������� �������.
                ������ � ���������� ��������� [1..M, 1..N]
    M       -   ����� ����� � ������� A
    N       -   ����� �������� � ������� A
    UNeeded -   0, 1 ��� 2. ��������� ��. �������� ��������� U
    VTNeeded-   0, 1 ��� 2. ��������� ��. �������� ��������� V

�������� ���������:
    W       -   �������� ����������� ��������, ������������� �� ��������.
    U       -   ���� UNeeded=0, �� ����������. ����� �����������   �������
                �� �����������.
                ���� UNeeded=1, �������� ����� ����������� ������� (������
                min(M,N) �������� ������� U). ������ � ���������� ���������
                [1..M, 1..Min(M,N)].
                ���� UNeeded=2, �������� ������ ������� U. ������ � ������-
                ���� ��������� [1..M, 1..M].
    VT      -   ���� VTNeeded=0, �� ����������. ������ ����������� �������
                �� �����������.
                ���� VTNeeded=1,  ��������   ������   �����������  �������
                (������ min(M,N) ����� ������� V^T). ������  �  ����������
                ��������� [1..min(M,N), 1..N].
                ���� VTNeeded=2, �������� ������  �������  V^T.  ������  �
                ���������� ��������� [1..N, 1..N].

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
bool svddecomposition(ap::real_2d_array a,
     int m,
     int n,
     int uneeded,
     int vtneeded,
     ap::real_1d_array& w,
     ap::real_2d_array& u,
     ap::real_2d_array& vt)
{
    bool result;
    ap::real_1d_array tauq;
    ap::real_1d_array taup;
    ap::real_1d_array tau;
    ap::real_1d_array e;
    ap::real_2d_array t;
    bool isupper;
    int minmn;
    int ncu;
    int nrvt;
    int nru;
    int ncvt;
    int i;
    int j;
    int im1;
    double sm;

    result = true;
    if( m==0||n==0 )
    {
        return result;
    }
    minmn = ap::minint(m, n);
    w.setbounds(1, minmn);
    ncu = 0;
    nru = 0;
    if( uneeded==1 )
    {
        nru = m;
        ncu = minmn;
        u.setbounds(1, nru, 1, ncu);
    }
    if( uneeded==2 )
    {
        nru = m;
        ncu = m;
        u.setbounds(1, nru, 1, ncu);
    }
    nrvt = 0;
    ncvt = 0;
    if( vtneeded==1 )
    {
        nrvt = minmn;
        ncvt = n;
        vt.setbounds(1, nrvt, 1, ncvt);
    }
    if( vtneeded==2 )
    {
        nrvt = n;
        ncvt = n;
        vt.setbounds(1, nrvt, 1, ncvt);
    }
    if( m>1.6*n )
    {
        if( uneeded==0 )
        {
            qrdecomposition(a, m, n, tau);
            for(i = 2; i <= n; i++)
            {
                for(j = 1; j <= i-1; j++)
                {
                    a(i,j) = 0;
                }
            }
            tobidiagonal(a, n, n, tauq, taup);
            unpackptfrombidiagonal(a, n, n, taup, nrvt, vt);
            unpackdiagonalsfrombidiagonal(a, n, n, isupper, w, e);
            result = bidiagonalsvddecomposition(w, e, n, isupper, false, u, 0, vt, ncvt);
            return result;
        }
        else
        {
            qrdecomposition(a, m, n, tau);
            unpackqfromqr(a, m, n, tau, ncu, u);
            for(i = 2; i <= n; i++)
            {
                for(j = 1; j <= i-1; j++)
                {
                    a(i,j) = 0;
                }
            }
            tobidiagonal(a, n, n, tauq, taup);
            unpackqfrombidiagonal(a, n, n, tauq, n, t);
            unpackptfrombidiagonal(a, n, n, taup, nrvt, vt);
            unpackdiagonalsfrombidiagonal(a, n, n, isupper, w, e);
            result = bidiagonalsvddecomposition(w, e, n, isupper, false, t, n, vt, ncvt);
            if( !result )
            {
                return result;
            }
            for(i = 1; i <= m; i++)
            {
                for(j = 1; j <= n; j++)
                {
                    sm = ap::vdotproduct(u.getrow(i, 1, n), t.getcolumn(j, 1, n));
                    a(i,j) = sm;
                }
            }
            for(j = 1; j <= n; j++)
            {
                ap::vmove(u.getcolumn(j, 1, m), a.getcolumn(j, 1, m));
            }
            return result;
        }
    }
    if( n>1.6*m )
    {
        if( vtneeded==0 )
        {
            lqdecomposition(a, m, n, tau);
            for(i = 1; i <= m-1; i++)
            {
                for(j = i+1; j <= m; j++)
                {
                    a(i,j) = 0;
                }
            }
            tobidiagonal(a, m, m, tauq, taup);
            unpackqfrombidiagonal(a, m, m, tauq, ncu, u);
            unpackdiagonalsfrombidiagonal(a, m, m, isupper, w, e);
            result = bidiagonalsvddecomposition(w, e, m, isupper, false, u, nru, vt, 0);
            return result;
        }
        else
        {
            lqdecomposition(a, m, n, tau);
            unpackqfromlq(a, m, n, tau, nrvt, vt);
            for(i = 1; i <= m-1; i++)
            {
                for(j = i+1; j <= m; j++)
                {
                    a(i,j) = 0;
                }
            }
            tobidiagonal(a, m, m, tauq, taup);
            unpackqfrombidiagonal(a, m, m, tauq, ncu, u);
            unpackptfrombidiagonal(a, m, m, taup, m, t);
            unpackdiagonalsfrombidiagonal(a, m, m, isupper, w, e);
            result = bidiagonalsvddecomposition(w, e, m, isupper, false, u, nru, t, m);
            if( !result )
            {
                return result;
            }
            for(j = 1; j <= n; j++)
            {
                for(i = 1; i <= m; i++)
                {
                    sm = ap::vdotproduct(t.getrow(i, 1, m), vt.getcolumn(j, 1, m));
                    a(i,j) = sm;
                }
            }
            for(i = 1; i <= m; i++)
            {
                ap::vmove(vt.getrow(i, 1, n), a.getrow(i, 1, n));
            }
            return result;
        }
    }
    tobidiagonal(a, m, n, tauq, taup);
    unpackqfrombidiagonal(a, m, n, tauq, ncu, u);
    unpackptfrombidiagonal(a, m, n, taup, nrvt, vt);
    unpackdiagonalsfrombidiagonal(a, m, n, isupper, w, e);
    result = bidiagonalsvddecomposition(w, e, minmn, isupper, false, u, nru, vt, ncvt);
    return result;
}

/*************************************************************************
���������� ������������������ ������������ �������� � �������

��������  ��������  ������� �����   ��  ������������������  ��������������
��������, �������� ��������� C � S. � ����������� ��  ���������  IsForward
�������� ������������ ��������������� ���� 1 � 2, 3 � 4 � �.�. ������ (����
IsForward=True), ���� N � N-1-��, N-2 � N-3 � �.�. ������.

�������������� ������������ �� ��� ������� � ������ � ����� (������ �� M1
�� M2, ������� �� N1 �� N2). ��������, �� �������� � ��������� ����������,
�������� ��� ���������.

������� ���������:
    IsForward   -   ������������������ ���������� ��������
    M1,M2       -   �������������� ������������ ������ �� M1 �� M2
    N1,N2       -   �������������� ������������ ������� �� N1 �� N2
    C,S         -   ������������ ��������������.
                    ������ � ���������� ��������� [1..M2-M1]
    A           -   �������������� �������
    WORK        -   ������� ������ � ���������� ��������� [N1..N2]

�������� ���������
    A           -   ��������������� �������

��������� ������������
*************************************************************************/
void applyrotationsfromtheleft(bool isforward,
     int m1,
     int m2,
     int n1,
     int n2,
     const ap::real_1d_array& c,
     const ap::real_1d_array& s,
     ap::real_2d_array& a,
     ap::real_1d_array& work)
{
    int i;
    int j;
    int jp1;
    double ctemp;
    double stemp;

    if( m1>m2||n1>n2 )
    {
        return;
    }
    if( isforward )
    {
        for(j = m1; j <= m2-1; j++)
        {
            ctemp = c(j-m1+1);
            stemp = s(j-m1+1);
            if( ctemp!=1||stemp!=0 )
            {
                jp1 = j+1;
                ap::vmove(work.getvector(n1, n2), a.getrow(jp1, n1, n2), ctemp);
                ap::vsub(work.getvector(n1, n2), a.getrow(j, n1, n2), stemp);
                ap::vmul(a.getrow(j, n1, n2), ctemp);
                ap::vadd(a.getrow(j, n1, n2), a.getrow(jp1, n1, n2), stemp);
                ap::vmove(a.getrow(jp1, n1, n2), work.getvector(n1, n2));
            }
        }
    }
    else
    {
        for(j = m2-1; j >= m1; j--)
        {
            ctemp = c(j-m1+1);
            stemp = s(j-m1+1);
            if( ctemp!=1||stemp!=0 )
            {
                jp1 = j+1;
                ap::vmove(work.getvector(n1, n2), a.getrow(jp1, n1, n2), ctemp);
                ap::vsub(work.getvector(n1, n2), a.getrow(j, n1, n2), stemp);
                ap::vmul(a.getrow(j, n1, n2), ctemp);
                ap::vadd(a.getrow(j, n1, n2), a.getrow(jp1, n1, n2), stemp);
                ap::vmove(a.getrow(jp1, n1, n2), work.getvector(n1, n2));
            }
        }
    }
}


/*************************************************************************
���������� ������������������ ������������ �������� � �������

��������  ��������  ������� ������   ��  ������������������  ��������������
��������, �������� ��������� C � S. � ����������� ��  ���������   IsForward
�������� ������������ ��������������� ���� 1 � 2,  3  �  4  � �.�.  �������
(���� IsForward=True), ���� N-�� � N-1-��, N-2-�� � N-3-�� � �.�. �������.

�������������� ������������ �� ��� ������� � ������ � ����� (������ �� M1
�� M2, ������� �� N1 �� N2). ��������, �� �������� � ��������� ����������,
�������� ��� ���������.

������� ���������:
    IsForward   -   ������������������ ���������� ��������
    M1,M2       -   �������������� ������������ ������ �� M1 �� M2
    N1,N2       -   �������������� ������������ ������� �� N1 �� N2
    C,S         -   ������������ ��������������.
                    ������ � ���������� ��������� [1..N2-N1]
    A           -   �������������� �������
    WORK        -   ������� ������ � ���������� ��������� [M1..M2]

�������� ���������
    A           -   ��������������� �������

��������� ������������
*************************************************************************/
void applyrotationsfromtheright(bool isforward,
     int m1,
     int m2,
     int n1,
     int n2,
     const ap::real_1d_array& c,
     const ap::real_1d_array& s,
     ap::real_2d_array& a,
     ap::real_1d_array& work)
{
    int i;
    int j;
    int jp1;
    double ctemp;
    double stemp;

    if( isforward )
    {
        for(j = n1; j <= n2-1; j++)
        {
            ctemp = c(j-n1+1);
            stemp = s(j-n1+1);
            if( ctemp!=1||stemp!=0 )
            {
                jp1 = j+1;
                ap::vmove(work.getvector(m1, m2), a.getcolumn(jp1, m1, m2), ctemp);
                ap::vsub(work.getvector(m1, m2), a.getcolumn(j, m1, m2), stemp);
                ap::vmul(a.getcolumn(j, m1, m2), ctemp);
                ap::vadd(a.getcolumn(j, m1, m2), a.getcolumn(jp1, m1, m2), stemp);
                ap::vmove(a.getcolumn(jp1, m1, m2), work.getvector(m1, m2));
            }
        }
    }
    else
    {
        for(j = n2-1; j >= n1; j--)
        {
            ctemp = c(j-n1+1);
            stemp = s(j-n1+1);
            if( ctemp!=1||stemp!=0 )
            {
                jp1 = j+1;
                ap::vmove(work.getvector(m1, m2), a.getcolumn(jp1, m1, m2), ctemp);
                ap::vsub(work.getvector(m1, m2), a.getcolumn(j, m1, m2), stemp);
                ap::vmul(a.getcolumn(j, m1, m2), ctemp);
                ap::vadd(a.getcolumn(j, m1, m2), a.getcolumn(jp1, m1, m2), stemp);
                ap::vmove(a.getcolumn(jp1, m1, m2), work.getvector(m1, m2));
            }
        }
    }
}


/*************************************************************************
������������ ���������� ������������ ��������, �����, ���

[  CS  SN  ]  .  [ F ]  =  [ R ]
[ -SN  CS  ]     [ G ]     [ 0 ]

CS**2 + SN**2 = 1
*************************************************************************/
void generaterotation(double f, double g, double& cs, double& sn, double& r)
{
    int cnt;
    int i;
    double eps;
    double f1;
    double g1;

    if( g==0 )
    {
        cs = 1;
        sn = 0;
        r = f;
    }
    else
    {
        if( f==0 )
        {
            cs = 0;
            sn = 1;
            r = g;
        }
        else
        {
            f1 = f;
            g1 = g;
            r = sqrt(ap::sqr(f1)+ap::sqr(g1));
            cs = f1/r;
            sn = g1/r;
            if( fabs(f)>fabs(g)&&cs<0 )
            {
                cs = -cs;
                sn = -sn;
                r = -r;
            }
        }
    }
}

/*************************************************************************
��������� ������������� �������������� ���������

������������ ���������� ������������ ��������� H ������� N, �����, ��� ���
��������� X ����������� ��������� ���������:

    ( X(1) )   ( Beta )
H * (  ..  ) = (  0   )
    ( X(n) )   (  0   )
    
���
              ( V(1) )
H = 1 - Tau * (  ..  ) * ( V(1), ..., V(n) )
              ( V(n) )

������ ������ ���������� ������� V ����� �������.
              
������� ���������:
    X       -   ������. ������ � ���������� ��������� [1..N]
    N       -   ������� ���������
    
�������� ���������:
    X       -   ���������� � 2 �� N ���������� �������� V. ������
                ���������� ���������� ���������� Beta.
    Tau     -   ��������� �������� Tau. ����� 0 (���� ������ X - �������),
                � ��������� ������ 1 <= Tau <= 2.

������ ������������ �������� ������������ ����������� DLARFG �� ����������
LAPACK. ���������������� �����������, �� �����������  ���������� ���������
�������, ����� ������������� ���������� ����������  �����������  ���������
�����.

  -- LAPACK auxiliary routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     September 30, 1994
*************************************************************************/
void generatereflection(ap::real_1d_array& x, int n, double& tau)
{
    int j;
    int knt;
    double alpha;
    double xnorm;
    double v;
    double safmin;
    double rsafmin;
    double beta;

    if( n<=1 )
    {
        tau = 0;
        return;
    }
    alpha = x(1);
    xnorm = ap::vdotproduct(x.getvector(2, n), x.getvector(2, n));
    xnorm = sqrt(xnorm);
    if( xnorm==0 )
    {
        tau = 0;
        return;
    }
    beta = -sqrt(ap::sqr(alpha)+ap::sqr(xnorm));
    if( alpha<0 )
    {
        beta = -beta;
    }
    tau = (beta-alpha)/beta;
    v = double(1)/double(alpha-beta);
    ap::vmul(x.getvector(2, n), v);
    x(1) = beta;
}


/*************************************************************************
���������� ������������� ��������� � ������������� ������� �������� MxN

�������� �������� ����� ������� �� ������������ ��������������  ���������,
��������    ��������   V   �   ���������   ���������   Tau  (��.  ��������
GenerateReflection). �������������� ������������ �� ��� �������, �  ������
� ����� (������ �� M1 �� M2, ������� �� N1 �� N2). ��������, ��  ��������
� ��������� ����������, �������� ��� ���������.

������� ���������:
    C   -   �������,  �  �������  �����������  ��������������.
    Tau -   ������, �������� ��������������.
    V   -   �������, �������� ��������������. ������ � ���������� ���������
            [1..M2-M1+1]
    M1,M2   -   �������� �����, ������������� ���������������
    N1,N2   -   �������� ��������, ������������� ���������������
    WORK    -   ������� ������ � ���������� ��������� �� N1 �� N2

�������� ���������:
    C   -   ��������� ��������� ������� ������� C �� ������� ��������������,
            ��������� Tau � V. ���� N1>N2 ��� M1>M2, �� C �� ��������.

  -- LAPACK auxiliary routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     September 30, 1994
*************************************************************************/
void applyreflectionfromtheleft(ap::real_2d_array& c,
     double tau,
     const ap::real_1d_array& v,
     int m1,
     int m2,
     int n1,
     int n2,
     ap::real_1d_array& work)
{
    double t;
    int i;
    int vm;

    if( tau==0||n1>n2||m1>m2 )
    {
        return;
    }
    vm = m2-m1+1;
    for(i = n1; i <= n2; i++)
    {
        work(i) = 0;
    }
    for(i = m1; i <= m2; i++)
    {
        t = v(i+1-m1);
        ap::vadd(work.getvector(n1, n2), c.getrow(i, n1, n2), t);
    }
    for(i = m1; i <= m2; i++)
    {
        t = v(i-m1+1)*tau;
        ap::vsub(c.getrow(i, n1, n2), work.getvector(n1, n2), t);
    }
}


/*************************************************************************
���������� ������������� ��������� � ������������� ������� �������� MxN

�������� �������� ������ ������� �� ������������ �������������� ���������,
��������    ��������   V   �   ���������   ���������   Tau  (��.  ��������
GenerateReflection). �������������� ������������ �� ��� �������, �  ������
� ����� (������ �� M1 �� M2, ������� �� N1 �� N2). ��������, ��  ��������
� ��������� ����������, �������� ��� ���������.

������� ���������:
    C   -   �������,  �  �������  �����������  ��������������.
    Tau -   ������, �������� ��������������.
    V   -   �������, �������� ��������������. ������ � ���������� ���������
            [1..N2-N1+1]
    M1,M2   -   �������� �����, ������������� ���������������
    N1,N2   -   �������� ��������, ������������� ���������������
    WORK    -   ������� ������ � ���������� ��������� �� M1 �� M2

�������� ���������:
    C   -   ��������� ��������� ������� ������� C �� ������� ��������������,
            ��������� Tau � V. ���� N1>N2 ��� M1>M2, �� C �� ��������.

  -- LAPACK auxiliary routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     September 30, 1994
*************************************************************************/
void applyreflectionfromtheright(ap::real_2d_array& c,
     double tau,
     const ap::real_1d_array& v,
     int m1,
     int m2,
     int n1,
     int n2,
     ap::real_1d_array& work)
{
    double t;
    int i;
    int vm;

    if( tau==0||n1>n2||m1>m2 )
    {
        return;
    }
    vm = n2-n1+1;
    for(i = m1; i <= m2; i++)
    {
        t = ap::vdotproduct(c.getrow(i, n1, n2), v.getvector(1, vm));
        work(i) = t;
    }
    for(i = m1; i <= m2; i++)
    {
        t = work(i)*tau;
        ap::vsub(c.getrow(i, n1, n2), v.getvector(1, vm), t);
    }
}



/*************************************************************************
QR-���������� ������������� ������� �������� M x N

������� ���������:
    A   -   ������� A. ��������� ���������: [1..M, 1..N]
    M   -   ����� ����� � ������� A
    N   -   ����� �������� � ������� A

�������� ���������:
    A   -   ������� Q � R � ���������� ����� (��. ����)
    Tau -   ������ ��������� ����������, ����������� � ������������
            ������� Q. ��������� ��������� [1.. Min(M,N)]
            
������� A ��������������, ��� A = QR, ��� Q ������������� ������� ��������
M x M, � R  ����������������� (��� ���������������������) ������� ��������
M x N.

����� ���������� ������ ������������ �� ������� ��������� ������� A � ����
������������� �������� ������� R. � ������� Tau � ���  �������  ����������
������� A ������������� ��������, ����������� ������� Q, ��������� ��������:

������� Q ��������������, ��� ������������ ������������ ���������

Q = H(1)*H(2)*...*H(k),

��� k = min(m,n), � ������ H(i) ����� ���

H(i) = 1 - tau * v * (v^T)

��� tau ��������� ��������, ���������� � Tau[I], � v - ������������ ������
� �������� v(1:i-1)=0, v(i)=1, v(i+1:m) �������� � ��������� A(i+1:m,i).

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     February 29, 1992
*************************************************************************/
void qrdecomposition(ap::real_2d_array& a,
     int m,
     int n,
     ap::real_1d_array& tau)
{
    ap::real_1d_array work;
    ap::real_1d_array t;
    int i;
    int k;
    int mmip1;
    double aii;
    int minmn;
    double tmp;

    minmn = ap::minint(m, n);
    work.setbounds(1, n);
    t.setbounds(1, m);
    tau.setbounds(1, minmn);
    k = ap::minint(m, n);
    for(i = 1; i <= k; i++)
    {
        mmip1 = m-i+1;
        ap::vmove(t.getvector(1, mmip1), a.getcolumn(i, i, m));
        generatereflection(t, mmip1, tmp);
        tau(i) = tmp;
        ap::vmove(a.getcolumn(i, i, m), t.getvector(1, mmip1));
        t(1) = 1;
        if( i<n )
        {
            applyreflectionfromtheleft(a, tau(i), t, i, m, i+1, n, work);
        }
    }
}


/*************************************************************************
��������� "����������" ������� Q �� QR-���������� ������� A.

������� ���������:
    QR      -   ������� Q � R � ����������� �����.
                ��������� ������ QRDecomposition.
    M       -   ����� ����� � ������������ ������� A. M>=0
    N       -   ����� �������� � ������������ ������� A. N>=0
    Tau     -   ��������� ���������, ����������� Q.
                ��������� ������ QRDecomposition.
    QColumns-   ��������� ����� �������� ������� Q. M>=QColumns>=0.

�������� ���������:
    Q       -   ������ QColumns �������� ������� Q. ������ � ����������
                ��������� [1..M, 1..QColumns]. ���� QColumns=0, �� ������
                �� ����������.

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
void unpackqfromqr(const ap::real_2d_array& qr,
     int m,
     int n,
     const ap::real_1d_array& tau,
     int qcolumns,
     ap::real_2d_array& q)
{
    int i;
    int j;
    int k;
    int minmn;
    ap::real_1d_array v;
    ap::real_1d_array work;
    int vm;

    ap::ap_error::make_assertion(qcolumns<=m);
    if( m==0||n==0||qcolumns==0 )
    {
        return;
    }
    minmn = ap::minint(m, n);
    k = ap::minint(minmn, qcolumns);
    q.setbounds(1, m, 1, qcolumns);
    v.setbounds(1, m);
    work.setbounds(1, qcolumns);
    for(i = 1; i <= m; i++)
    {
        for(j = 1; j <= qcolumns; j++)
        {
            if( i==j )
            {
                q(i,j) = 1;
            }
            else
            {
                q(i,j) = 0;
            }
        }
    }
    for(i = k; i >= 1; i--)
    {
        vm = m-i+1;
        ap::vmove(v.getvector(1, vm), qr.getcolumn(i, i, m));
        v(1) = 1;
        applyreflectionfromtheleft(q, tau(i), v, i, m, 1, qcolumns, work);
    }
}


/*************************************************************************
QR-���������� ������������� ������� ������� M x N

����������  QRDecomposition.   ��  ����������������  ����������  ���,  ���
�������  �������  Q  �  R �� � ���������� �����, � � ���� ��������� ������
������ ����. ��������������, �������  R  "���������������",  ����������  �
������ ������ �������� ����������, � ������� Q ��������  ���  ������������
������������ � ������������ ���������.

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
void qrdecompositionunpacked(ap::real_2d_array a,
     int m,
     int n,
     ap::real_2d_array& q,
     ap::real_2d_array& r)
{
    int i;
    int j;
    int k;
    int l;
    int vm;
    ap::real_1d_array tau;
    ap::real_1d_array work;
    ap::real_1d_array v;
    double tmp;

    k = ap::minint(m, n);
    if( n<=0 )
    {
        return;
    }
    work.setbounds(1, m);
    v.setbounds(1, m);
    q.setbounds(1, m, 1, m);
    r.setbounds(1, m, 1, n);
    qrdecomposition(a, m, n, tau);
    for(i = 1; i <= n; i++)
    {
        r(1,i) = 0;
    }
    for(i = 2; i <= m; i++)
    {
        ap::vmove(r.getrow(i, 1, n), r.getrow(1, 1, n));
    }
    for(i = 1; i <= k; i++)
    {
        ap::vmove(r.getrow(i, i, n), a.getrow(i, i, n));
    }
    unpackqfromqr(a, m, n, tau, m, q);
}


/*************************************************************************
LQ-���������� ������������� ������� �������� M x N

������� ���������:
    A   -   ������� A. ��������� ���������: [1..M, 1..N]
    M   -   ����� ����� � ������� A
    N   -   ����� �������� � ������� A

�������� ���������:
    A   -   ������� L � Q � ���������� ����� (��. ����)
    Tau -   ������ ��������� ����������, ����������� � ������������
            ������� Q. ��������� ��������� [1.. Min(M,N)]
            
������� A ��������������, ��� A = LQ, ��� Q ������������� ������� ��������
M x M, � R  ����������������  (���  ��������������������) ������� ��������
M x N.

����� ���������� ������ ������������ �� ������� ��������� ������� A � ����
������������� �������� ������� L. � ������� Tau � ���  �������  ����������
������� A ������������� ��������, ����������� ������� Q, ��������� ��������:

������� Q ��������������, ��� ������������ ������������ ���������

Q = H(k)*H(k-1)*...*H(2)*H(1),

��� k = min(m,n), � ������ H(i) ����� ���

H(i) = 1 - tau * v * (v^T)

��� tau ��������� ��������, ���������� � Tau[I], � v - ������������ ������
� �������� v(1:i-1)=0, v(i)=1, v(i+1:n) �������� � ��������� A(i,i+1:n).

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
void lqdecomposition(ap::real_2d_array& a,
     int m,
     int n,
     ap::real_1d_array& tau)
{
    ap::real_1d_array work;
    ap::real_1d_array t;
    int i;
    int k;
    int nmip1;
    double aii;
    int minmn;
    int maxmn;
    double tmp;

    minmn = ap::minint(m, n);
    maxmn = ap::maxint(m, n);
    work.setbounds(1, m);
    t.setbounds(1, n);
    tau.setbounds(1, minmn);
    k = ap::minint(m, n);
    for(i = 1; i <= k; i++)
    {
        nmip1 = n-i+1;
        ap::vmove(t.getvector(1, nmip1), a.getrow(i, i, n));
        generatereflection(t, nmip1, tmp);
        tau(i) = tmp;
        ap::vmove(a.getrow(i, i, n), t.getvector(1, nmip1));
        t(1) = 1;
        if( i<n )
        {
            applyreflectionfromtheright(a, tau(i), t, i+1, m, i, n, work);
        }
    }
}


/*************************************************************************
��������� "����������" ������� Q �� LQ-���������� ������� A.

������� ���������:
    LQ      -   ������� L � Q � ����������� �����.
                ��������� ������ LQDecomposition.
    M       -   ����� ����� � ������������ ������� A. M>=0
    N       -   ����� �������� � ������������ ������� A. N>=0
    Tau     -   ��������� ���������, ����������� Q.
                ��������� ������ LQDecomposition.
    QRows   -   ��������� ����� ����� ������� Q. N>=QRows>=0.

�������� ���������:
    Q       -   ������ QRows ����� ������� Q. ������ � ����������
                ��������� [1..QRows, 1..N]. ���� QRows=0, �� ������
                �� ����������.

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
void unpackqfromlq(const ap::real_2d_array& lq,
     int m,
     int n,
     const ap::real_1d_array& tau,
     int qrows,
     ap::real_2d_array& q)
{
    int i;
    int j;
    int k;
    int minmn;
    ap::real_1d_array v;
    ap::real_1d_array work;
    int vm;

    ap::ap_error::make_assertion(qrows<=n);
    if( m==0||n==0||qrows==0 )
    {
        return;
    }
    minmn = ap::minint(m, n);
    k = ap::minint(minmn, qrows);
    q.setbounds(1, qrows, 1, n);
    v.setbounds(1, n);
    work.setbounds(1, qrows);
    for(i = 1; i <= qrows; i++)
    {
        for(j = 1; j <= n; j++)
        {
            if( i==j )
            {
                q(i,j) = 1;
            }
            else
            {
                q(i,j) = 0;
            }
        }
    }
    for(i = k; i >= 1; i--)
    {
        vm = n-i+1;
        ap::vmove(v.getvector(1, vm), lq.getrow(i, i, n));
        v(1) = 1;
        applyreflectionfromtheright(q, tau(i), v, 1, qrows, i, n, work);
    }
}


/*************************************************************************
LQ-���������� ������������� ������� ������� M x N

����������  LQDecomposition.   ��  ����������������  ����������  ���,  ���
�������  �������  L  �  Q �� � ���������� �����, � � ���� ��������� ������
������ ����. ��������������, �������  L  "���������������",  ����������  �
������ ������ �������� ����������, � ������� Q ��������  ���  ������������
������������ � ������������ ���������.

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
void lqdecompositionunpacked(ap::real_2d_array a,
     int m,
     int n,
     ap::real_2d_array& l,
     ap::real_2d_array& q)
{
    int i;
    int j;
    ap::real_1d_array tau;
    double tmp;

    if( n<=0 )
    {
        return;
    }
    q.setbounds(1, n, 1, n);
    l.setbounds(1, m, 1, n);
    lqdecomposition(a, m, n, tau);
    for(i = 1; i <= m; i++)
    {
        for(j = 1; j <= n; j++)
        {
            if( j>i )
            {
                l(i,j) = 0;
            }
            else
            {
                l(i,j) = a(i,j);
            }
        }
    }
    unpackqfromlq(a, m, n, tau, n, q);
}
/*************************************************************************
����������� ���������� ���������������� �������

��������  ������������  �����������  ���������� ���������������� ������� B
(������� ��� ������), ����������� � � ���� B = Q * S * P^T, ��� Q �  P  -
������������� �������, S - ������������ � ����������������  ����������  ��
������� ���������, �������������� � ������� ��������.

�������� ������� ����������� ��������. ������������� �������� ����� ���������
������� Q � P (������, �� ���� �������, � �� ������������ �� �������� �������
U � VT - U*Q � (P^T)*VT)). ����������, ������� U � VT  �����  ����  ������
������, � ��� ����� � ����������.

������������ ���������  ��������   �����������  ��������  ���  �����������
��������,  �  ���  �����  �  �����  ������ ������� � ����, � �������������
���������, ������� � ��������. ���� ��������  IsFractionalAccuracyRequired
����� True, �� ��� ����������� �������� ��������� � �������  �������������
���������, ������� � ��������. ���� �������� ����� False, �� �  ���������,
������� � ��������, ��������� ���� ����� ������� ����������� ��������,   �
���������� ����������� ���������  �����������  ��������  �����  ����������
����������� ������ �������� ������������ ��������.

������� ���������:
    D   -       ������� ��������� ������� B.
                ������ � ���������� ��������� [1..N]
    E   -       ������������ (��� ������������) ������� B.
                ������ � ���������� ��������� [1..N-1]
    N   -       ������ ������� B
    IsUpper-    True, ���� ������� ������� ����������������.
    IsFractionalAccuracyRequired-
                � ����� ��������� ���� ������ ����������� ��������
    U   -       �������, ������� �����  �������� �� �������  Q.  ������  �
                ���������� ��������� [1..NRU, 1..N].  �������  �����  ����
                ������, � ����� ������  ����  ���������  ����������  �����
                �������� �� ������� Q.
    NRU -       ����� ����� � ������� U
    VT  -       �������, ������� �����  �������� �� �������  P^T. ������ �
                ���������� ��������� [1..N, 1..NCVT]. �������  �����  ����
                ������, � ����� ������  ����  ���������  ����������  �����
                �������� �� ������� P^T.
    NCVT-       ����� �������� � ������� VT.

�������� ���������:
    D   -       ����������� �������� ������� B, ������������� �� ��������.
    U   -       ���� NRU>0, �������� ������� U*Q
    VT  -       ���� NCVT>0, �������� ������� (P^T)*VT

�������������� ����������:
    ��� ���������� �������������� ���������� ���������� TOL. ���� ��������
    ������   ����,  ��  �����������  ��������  ���������  �  �������������
    ������������ TOL. ���� TOL<0, ����������� �������� ��������� �
    ���������� ������������ ABS(TOL)*norm(B).
    �� ��������� |TOL| ��������� � ��������� �� 10*Epsilon �� 100*Epsilon,
    ��� Epsilon - �������� ��������. �� ������������� ������  TOL  ������,
    ��� 10*Epsilon, ��������� ��� �����������  ��������  ��������,  ��  ��
    ����������� �������� � ����������� ���������� �����������.

  -- LAPACK routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     October 31, 1999
*************************************************************************/
bool bidiagonalsvddecomposition(ap::real_1d_array& d,
     ap::real_1d_array e,
     int n,
     bool isupper,
     bool isfractionalaccuracyrequired,
     ap::real_2d_array& u,
     int nru,
     ap::real_2d_array& vt,
     int ncvt)
{
    bool result;
    int i;
    int idir;
    int isub;
    int iter;
    int j;
    int ll;
    int lll;
    int m;
    int maxit;
    int nm1;
    int nm12;
    int nm13;
    int oldll;
    int oldm;
    double abse;
    double abss;
    double cosl;
    double cosr;
    double cs;
    double eps;
    double f;
    double g;
    double h;
    double mu;
    double oldcs;
    double oldsn;
    double r;
    double shift;
    double sigmn;
    double sigmx;
    double sinl;
    double sinr;
    double sll;
    double smax;
    double smin;
    double sminl;
    double sminlo;
    double sminoa;
    double sn;
    double thresh;
    double tol;
    double tolmul;
    double unfl;
    ap::real_1d_array work0;
    ap::real_1d_array work1;
    ap::real_1d_array work2;
    ap::real_1d_array work3;
    int maxitr;
    bool matrixsplitflag;
    bool iterflag;
    ap::real_1d_array utemp;
    ap::real_1d_array vttemp;
    ap::real_1d_array etemp;
    bool rightside;
    bool fwddir;
    double tmp;
    int mm1;

    result = true;
    if( n==0 )
    {
        return result;
    }
    if( n==1 )
    {
        if( d(1)<0 )
        {
            d(1) = -d(1);
            if( ncvt>0 )
            {
                ap::vmul(vt.getrow(1, 1, ncvt), -1);
            }
        }
        return result;
    }
    work0.setbounds(1, n-1);
    work1.setbounds(1, n-1);
    work2.setbounds(1, n-1);
    work3.setbounds(1, n-1);
    utemp.setbounds(1, ap::maxint(nru, 1));
    vttemp.setbounds(1, ap::maxint(ncvt, 1));
    maxitr = 6;
    rightside = true;
    fwddir = true;
    etemp.setbounds(1, n);
    for(i = 1; i <= n-1; i++)
    {
        etemp(i) = e(i);
    }
    e.setbounds(1, n);
    for(i = 1; i <= n-1; i++)
    {
        e(i) = etemp(i);
    }
    e(n) = 0;
    nm1 = n-1;
    nm12 = nm1+nm1;
    nm13 = nm12+nm1;
    idir = 0;
    eps = ap::machineepsilon;
    unfl = ap::minrealnumber;
    if( !isupper )
    {
        for(i = 1; i <= n-1; i++)
        {
            generaterotation(d(i), e(i), cs, sn, r);
            d(i) = r;
            e(i) = sn*d(i+1);
            d(i+1) = cs*d(i+1);
            work0(i) = cs;
            work1(i) = sn;
        }
        if( nru>0 )
        {
            applyrotationsfromtheright(fwddir, 1, nru, 1, n, work0, work1, u, utemp);
        }
    }
    tolmul = ap::maxreal(10, ap::minreal(100, pow(eps, -0.125)));
    tol = tolmul*eps;
    if( !isfractionalaccuracyrequired )
    {
        tol = -tol;
    }
    smax = 0;
    for(i = 1; i <= n; i++)
    {
        smax = ap::maxreal(smax, fabs(d(i)));
    }
    for(i = 1; i <= n-1; i++)
    {
        smax = ap::maxreal(smax, fabs(e(i)));
    }
    sminl = 0;
    if( tol>=0 )
    {
        sminoa = fabs(d(1));
        if( sminoa!=0 )
        {
            mu = sminoa;
            for(i = 2; i <= n; i++)
            {
                mu = fabs(d(i))*(mu/(mu+fabs(e(i-1))));
                sminoa = ap::minreal(sminoa, mu);
                if( sminoa==0 )
                {
                    break;
                }
            }
        }
        sminoa = sminoa/sqrt(n);
        thresh = ap::maxreal(tol*sminoa, maxitr*n*n*unfl);
    }
    else
    {
        thresh = ap::maxreal(fabs(tol)*smax, maxitr*n*n*unfl);
    }
    maxit = maxitr*n*n;
    iter = 0;
    oldll = -1;
    oldm = -1;
    m = n;
    while(true)
    {
        if( m<=1 )
        {
            break;
        }
        if( iter>maxit )
        {
            result = false;
            return result;
        }
        if( tol<0&&fabs(d(m))<=thresh )
        {
            d(m) = 0;
        }
        smax = fabs(d(m));
        smin = smax;
        matrixsplitflag = false;
        for(lll = 1; lll <= m-1; lll++)
        {
            ll = m-lll;
            abss = fabs(d(ll));
            abse = fabs(e(ll));
            if( tol<0&&abss<=thresh )
            {
                d(ll) = 0;
            }
            if( abse<=thresh )
            {
                matrixsplitflag = true;
                break;
            }
            smin = ap::minreal(smin, abss);
            smax = ap::maxreal(smax, ap::maxreal(abss, abse));
        }
        if( !matrixsplitflag )
        {
            ll = 0;
        }
        else
        {
            e(ll) = 0;
            if( ll==m-1 )
            {
                m = m-1;
                continue;
            }
        }
        ll = ll+1;
        if( ll==m-1 )
        {
            svdv2x2(d(m-1), e(m-1), d(m), sigmn, sigmx, sinr, cosr, sinl, cosl);
            d(m-1) = sigmx;
            e(m-1) = 0;
            d(m) = sigmn;
            if( ncvt>0 )
            {
                mm1 = m-1;
                ap::vmove(vttemp.getvector(1, ncvt), vt.getrow(mm1, 1, ncvt), cosr);
                ap::vadd(vttemp.getvector(1, ncvt), vt.getrow(m, 1, ncvt), sinr);
                ap::vmul(vt.getrow(m, 1, ncvt), cosr);
                ap::vsub(vt.getrow(m, 1, ncvt), vt.getrow(mm1, 1, ncvt), sinr);
                ap::vmove(vt.getrow(mm1, 1, ncvt), vttemp.getvector(1, ncvt));
            }
            if( nru>0 )
            {
                mm1 = m-1;
                ap::vmove(utemp.getvector(1, nru), u.getcolumn(mm1, 1, nru), cosl);
                ap::vadd(utemp.getvector(1, nru), u.getcolumn(m, 1, nru), sinl);
                ap::vmul(u.getcolumn(m, 1, nru), cosl);
                ap::vsub(u.getcolumn(m, 1, nru), u.getcolumn(mm1, 1, nru), sinl);
                ap::vmove(u.getcolumn(mm1, 1, nru), utemp.getvector(1, nru));
            }
            m = m-2;
            continue;
        }
        if( ll>oldm||m<oldll )
        {
            if( fabs(d(ll))>=fabs(d(m)) )
            {
                idir = 1;
            }
            else
            {
                idir = 2;
            }
        }
        if( idir==1 )
        {
            if( fabs(e(m-1))<=fabs(tol)*fabs(d(m))||tol<0&&fabs(e(m-1))<=thresh )
            {
                e(m-1) = 0;
                continue;
            }
            if( tol>=0 )
            {
                mu = fabs(d(ll));
                sminl = mu;
                iterflag = false;
                for(lll = ll; lll <= m-1; lll++)
                {
                    if( fabs(e(lll))<=tol*mu )
                    {
                        e(lll) = 0;
                        iterflag = true;
                        break;
                    }
                    sminlo = sminl;
                    mu = fabs(d(lll+1))*(mu/(mu+fabs(e(lll))));
                    sminl = ap::minreal(sminl, mu);
                }
                if( iterflag )
                {
                    continue;
                }
            }
        }
        else
        {
            if( fabs(e(ll))<=fabs(tol)*fabs(d(ll))||tol<0&&fabs(e(ll))<=thresh )
            {
                e(ll) = 0;
                continue;
            }
            if( tol>=0 )
            {
                mu = fabs(d(m));
                sminl = mu;
                iterflag = false;
                for(lll = m-1; lll >= ll; lll--)
                {
                    if( fabs(e(lll))<=tol*mu )
                    {
                        e(lll) = 0;
                        iterflag = true;
                        break;
                    }
                    sminlo = sminl;
                    mu = fabs(d(lll))*(mu/(mu+fabs(e(lll))));
                    sminl = ap::minreal(sminl, mu);
                }
                if( iterflag )
                {
                    continue;
                }
            }
        }
        oldll = ll;
        oldm = m;
        if( tol>=0&&n*tol*(sminl/smax)<=ap::maxreal(eps, 0.01*tol) )
        {
            shift = 0;
        }
        else
        {
            if( idir==1 )
            {
                sll = fabs(d(ll));
                svd2x2(d(m-1), e(m-1), d(m), shift, r);
            }
            else
            {
                sll = fabs(d(m));
                svd2x2(d(ll), e(ll), d(ll+1), shift, r);
            }
            if( sll>0 )
            {
                if( ap::sqr(shift/sll)<eps )
                {
                    shift = 0;
                }
            }
        }
        iter = iter+m-ll;
        if( shift==0 )
        {
            if( idir==1 )
            {
                cs = 1;
                oldcs = 1;
                for(i = ll; i <= m-1; i++)
                {
                    generaterotation(d(i)*cs, e(i), cs, sn, r);
                    if( i>ll )
                    {
                        e(i-1) = oldsn*r;
                    }
                    generaterotation(oldcs*r, d(i+1)*sn, oldcs, oldsn, tmp);
                    d(i) = tmp;
                    work0(i-ll+1) = cs;
                    work1(i-ll+1) = sn;
                    work2(i-ll+1) = oldcs;
                    work3(i-ll+1) = oldsn;
                }
                h = d(m)*cs;
                d(m) = h*oldcs;
                e(m-1) = h*oldsn;
                if( ncvt>0 )
                {
                    applyrotationsfromtheleft(fwddir, ll, m, 1, ncvt, work0, work1, vt, vttemp);
                }
                if( nru>0 )
                {
                    applyrotationsfromtheright(fwddir, 1, nru, ll, m, work2, work3, u, utemp);
                }
                if( fabs(e(m-1))<=thresh )
                {
                    e(m-1) = 0;
                }
            }
            else
            {
                cs = 1;
                oldcs = 1;
                for(i = m; i >= ll+1; i--)
                {
                    generaterotation(d(i)*cs, e(i-1), cs, sn, r);
                    if( i<m )
                    {
                        e(i) = oldsn*r;
                    }
                    generaterotation(oldcs*r, d(i-1)*sn, oldcs, oldsn, tmp);
                    d(i) = tmp;
                    work0(i-ll) = cs;
                    work1(i-ll) = -sn;
                    work2(i-ll) = oldcs;
                    work3(i-ll) = -oldsn;
                }
                h = d(ll)*cs;
                d(ll) = h*oldcs;
                e(ll) = h*oldsn;
                if( ncvt>0 )
                {
                    applyrotationsfromtheleft(!fwddir, ll, m, 1, ncvt, work2, work3, vt, vttemp);
                }
                if( nru>0 )
                {
                    applyrotationsfromtheright(!fwddir, 1, nru, ll, m, work0, work1, u, utemp);
                }
                if( fabs(e(ll))<=thresh )
                {
                    e(ll) = 0;
                }
            }
        }
        else
        {
            if( idir==1 )
            {
                f = (fabs(d(ll))-shift)*(extsignbdsqr(1, d(ll))+shift/d(ll));
                g = e(ll);
                for(i = ll; i <= m-1; i++)
                {
                    generaterotation(f, g, cosr, sinr, r);
                    if( i>ll )
                    {
                        e(i-1) = r;
                    }
                    f = cosr*d(i)+sinr*e(i);
                    e(i) = cosr*e(i)-sinr*d(i);
                    g = sinr*d(i+1);
                    d(i+1) = cosr*d(i+1);
                    generaterotation(f, g, cosl, sinl, r);
                    d(i) = r;
                    f = cosl*e(i)+sinl*d(i+1);
                    d(i+1) = cosl*d(i+1)-sinl*e(i);
                    if( i<m-1 )
                    {
                        g = sinl*e(i+1);
                        e(i+1) = cosl*e(i+1);
                    }
                    work0(i-ll+1) = cosr;
                    work1(i-ll+1) = sinr;
                    work2(i-ll+1) = cosl;
                    work3(i-ll+1) = sinl;
                }
                e(m-1) = f;
                if( ncvt>0 )
                {
                    applyrotationsfromtheleft(fwddir, ll, m, 1, ncvt, work0, work1, vt, vttemp);
                }
                if( nru>0 )
                {
                    applyrotationsfromtheright(fwddir, 1, nru, ll, m, work2, work3, u, utemp);
                }
                if( fabs(e(m-1))<=thresh )
                {
                    e(m-1) = 0;
                }
            }
            else
            {
                f = (fabs(d(m))-shift)*(extsignbdsqr(1, d(m))+shift/d(m));
                g = e(m-1);
                for(i = m; i >= ll+1; i--)
                {
                    generaterotation(f, g, cosr, sinr, r);
                    if( i<m )
                    {
                        e(i) = r;
                    }
                    f = cosr*d(i)+sinr*e(i-1);
                    e(i-1) = cosr*e(i-1)-sinr*d(i);
                    g = sinr*d(i-1);
                    d(i-1) = cosr*d(i-1);
                    generaterotation(f, g, cosl, sinl, r);
                    d(i) = r;
                    f = cosl*e(i-1)+sinl*d(i-1);
                    d(i-1) = cosl*d(i-1)-sinl*e(i-1);
                    if( i>ll+1 )
                    {
                        g = sinl*e(i-2);
                        e(i-2) = cosl*e(i-2);
                    }
                    work0(i-ll) = cosr;
                    work1(i-ll) = -sinr;
                    work2(i-ll) = cosl;
                    work3(i-ll) = -sinl;
                }
                e(ll) = f;
                if( fabs(e(ll))<=thresh )
                {
                    e(ll) = 0;
                }
                if( ncvt>0 )
                {
                    applyrotationsfromtheleft(!fwddir, ll, m, 1, ncvt, work2, work3, vt, vttemp);
                }
                if( nru>0 )
                {
                    applyrotationsfromtheright(!fwddir, 1, nru, ll, m, work0, work1, u, utemp);
                }
            }
        }
        continue;
    }
    for(i = 1; i <= n; i++)
    {
        if( d(i)<0 )
        {
            d(i) = -d(i);
            if( ncvt>0 )
            {
                ap::vmul(vt.getrow(i, 1, ncvt), -1);
            }
        }
    }
    for(i = 1; i <= n-1; i++)
    {
        isub = 1;
        smin = d(1);
        for(j = 2; j <= n+1-i; j++)
        {
            if( d(j)<=smin )
            {
                isub = j;
                smin = d(j);
            }
        }
        if( isub!=n+1-i )
        {
            d(isub) = d(n+1-i);
            d(n+1-i) = smin;
            if( ncvt>0 )
            {
                j = n+1-i;
                ap::vmove(vttemp.getvector(1, ncvt), vt.getrow(isub, 1, ncvt));
                ap::vmove(vt.getrow(isub, 1, ncvt), vt.getrow(j, 1, ncvt));
                ap::vmove(vt.getrow(j, 1, ncvt), vttemp.getvector(1, ncvt));
            }
            if( nru>0 )
            {
                j = n+1-i;
                ap::vmove(utemp.getvector(1, nru), u.getcolumn(isub, 1, nru));
                ap::vmove(u.getcolumn(isub, 1, nru), u.getcolumn(j, 1, nru));
                ap::vmove(u.getcolumn(j, 1, nru), utemp.getvector(1, nru));
            }
        }
    }
    return result;
}


/*************************************************************************
�������� �����. ��������� ������������.
*************************************************************************/
double extsignbdsqr(double a, double b)
{
    double result;

    if( b>=0 )
    {
        result = fabs(a);
    }
    else
    {
        result = -fabs(a);
    }
    return result;
}


/*************************************************************************
Svd2X2  computes the singular values of the 2-by-2 matrix
   [  F   G  ]
   [  0   H  ].
On return, SSMIN is the smaller singular value and SSMAX is the
larger singular value.

  -- LAPACK auxiliary routine (version 3.0) --
     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
     Courant Institute, Argonne National Lab, and Rice University
     September 30, 1994
*************************************************************************/
void svd2x2(double f, double g, double h, double& ssmin, double& ssmax)
{
    double aas;
    double at;
    double au;
    double c;
    double fa;
    double fhmn;
    double fhmx;
    double ga;
    double ha;

    fa = fabs(f);
    ga = fabs(g);
    ha = fabs(h);
    fhmn = ap::minreal(fa, ha);
    fhmx = ap::maxreal(fa, ha);
    if( fhmn==0 )
    {
        ssmin = 0;
        if( fhmx==0 )
        {
            ssmax = ga;
        }
        else
        {
            ssmax = ap::maxreal(fhmx, ga)*sqrt(1+ap::sqr(ap::minreal(fhmx, ga)/ap::maxreal(fhmx, ga)));
        }
    }
    else
    {
        if( ga<fhmx )
        {
            aas = 1+fhmn/fhmx;
            at = (fhmx-fhmn)/fhmx;
            au = ap::sqr(ga/fhmx);
            c = double(2)/double(sqrt(aas*aas+au)+sqrt(at*at+au));
            ssmin = fhmn*c;
            ssmax = fhmx/c;
        }
        else
        {
            au = fhmx/ga;
            if( au==0 )
            {
                ssmin = fhmn*fhmx/ga;
                ssmax = ga;
            }
            else
            {
                aas = 1+fhmn/fhmx;
                at = (fhmx-fhmn)/fhmx;
                c = double(1)/double(sqrt(1+ap::sqr(aas*au))+sqrt(1+ap::sqr(at*au)));
                ssmin = fhmn*c*au;
                ssmin = ssmin+ssmin;
                ssmax = ga/(c+c);
            }
        }
    }
}


/*************************************************************************
SvdV2X2 computes the singular value decomposition of a 2-by-2
triangular matrix
   [  F   G  ]
   [  0   H  ].
On return, abs(SSMAX) is the larger singular value, abs(SSMIN) is the
smaller singular value, and (CSL,SNL) and (CSR,SNR) are the left and
right singular vectors for abs(SSMAX), giving the decomposition

   [ CSL  SNL ] [  F   G  ] [ CSR -SNR ]  =  [ SSMAX   0   ]
   [-SNL  CSL ] [  0   H  ] [ SNR  CSR ]     [  0    SSMIN ].
*************************************************************************/
void svdv2x2(double f,
     double g,
     double h,
     double& ssmin,
     double& ssmax,
     double& snr,
     double& csr,
     double& snl,
     double& csl)
{
    bool gasmal;
    bool swp;
    int pmax;
    double a;
    double clt;
    double crt;
    double d;
    double fa;
    double ft;
    double ga;
    double gt;
    double ha;
    double ht;
    double l;
    double m;
    double mm;
    double r;
    double s;
    double slt;
    double srt;
    double t;
    double temp;
    double tsign;
    double tt;
    double v;

    ft = f;
    fa = fabs(ft);
    ht = h;
    ha = fabs(h);
    pmax = 1;
    swp = ha>fa;
    if( swp )
    {
        pmax = 3;
        temp = ft;
        ft = ht;
        ht = temp;
        temp = fa;
        fa = ha;
        ha = temp;
    }
    gt = g;
    ga = fabs(gt);
    if( ga==0 )
    {
        ssmin = ha;
        ssmax = fa;
        clt = 1;
        crt = 1;
        slt = 0;
        srt = 0;
    }
    else
    {
        gasmal = true;
        if( ga>fa )
        {
            pmax = 2;
            if( fa/ga<ap::machineepsilon )
            {
                gasmal = false;
                ssmax = ga;
                if( ha>1 )
                {
                    v = ga/ha;
                    ssmin = fa/v;
                }
                else
                {
                    v = fa/ga;
                    ssmin = v*ha;
                }
                clt = 1;
                slt = ht/gt;
                srt = 1;
                crt = ft/gt;
            }
        }
        if( gasmal )
        {
            d = fa-ha;
            if( d==fa )
            {
                l = 1;
            }
            else
            {
                l = d/fa;
            }
            m = gt/ft;
            t = 2-l;
            mm = m*m;
            tt = t*t;
            s = sqrt(tt+mm);
            if( l==0 )
            {
                r = fabs(m);
            }
            else
            {
                r = sqrt(l*l+mm);
            }
            a = 0.5*(s+r);
            ssmin = ha/a;
            ssmax = fa*a;
            if( mm==0 )
            {
                if( l==0 )
                {
                    t = extsignbdsqr(2, ft)*extsignbdsqr(1, gt);
                }
                else
                {
                    t = gt/extsignbdsqr(d, ft)+m/t;
                }
            }
            else
            {
                t = (m/(s+t)+m/(r+l))*(1+a);
            }
            l = sqrt(t*t+4);
            crt = double(2)/double(l);
            srt = t/l;
            clt = (crt+srt*m)/a;
            v = ht/ft;
            slt = v*srt/a;
        }
    }
    if( swp )
    {
        csl = srt;
        snl = crt;
        csr = slt;
        snr = clt;
    }
    else
    {
        csl = clt;
        snl = slt;
        csr = crt;
        snr = srt;
    }
    if( pmax==1 )
    {
        tsign = extsignbdsqr(1, csr)*extsignbdsqr(1, csl)*extsignbdsqr(1, f);
    }
    if( pmax==2 )
    {
        tsign = extsignbdsqr(1, snr)*extsignbdsqr(1, csl)*extsignbdsqr(1, g);
    }
    if( pmax==3 )
    {
        tsign = extsignbdsqr(1, snr)*extsignbdsqr(1, snl)*extsignbdsqr(1, h);
    }
    ssmax = extsignbdsqr(ssmax, tsign);
    ssmin = extsignbdsqr(ssmin, tsign*extsignbdsqr(1, f)*extsignbdsqr(1, h));
}


/*************************************************************************
���������� ������������� ������� � ����������������� ����

��������  ��������   �������������  �������  A  �  �����������������  ����
�������������� ���������������� P � Q, ������, ��� A = Q*B*P'

������� ���������:
    A   -   �������� ������� A. ������ � ���������� ��������� [1..M, 1..N]
    M   -   ����� ����� � �������
    N   -   ����� �������� � �������
    
�������� ���������:
    A   -   ������� Q, B, P � ����������� ����� (��. ����).
    TauQ-   ��������� ���������, ����������� � ������������ ������� Q
    TauP-   ��������� ���������, ����������� � ������������ ������� Q

� ���������� ������ ��������� ������� ��������� ������� A � ���� �� ��������
���������� �������������� �������� B, �  �  ���������  ���������  ��������
������������  ���������,  �����������  �������  Q  �  P �������� MxM � NxN
��������������.

���� M>=N, �� ������� B - ������� ���������������� �������� MxN � ��������
� ��������������� ��������� ������� A. �������  Q  ��������������  �  ����
������������ ������������  ���������  Q = H(1)*H(2)*...*H(n),  ���  H(i) =
= 1 - tau*v*v'.  �����  tau  -  ���������  ��������, �������� � TauQ[i], �
������ v ����� ��������� ���������: v(1:i-1)=0, v(i)=1, v(i+1:m)  ��������
� ��������� A(i+1:m,i). ������� P ����� ��� P = G(1)*G(2)*...*G(n-1),  ���
G(i) = 1 - tau*u*u'. Tau �������� � TauP[i], u(1:i)=0, u(i+1)=1,  u(i+2:n)
�������� � ��������� A(i,i+2:n).

���� M<N, �� ������� B - ������ ���������������� �������� MxN, �  ��������
�  ���������������  ���������  �������  A.  Q = H(1)*H(2)*...*H(m-1),  ���
H(i) = 1 - tau*v*v',  tau  ��������  �  TauQ, v(1:i)=0, v(i+1)=1, v(i+2:m)
�������� � A(i+2:m,i).  P = G(1)*G(2)*...*G(m),  G(i) = 1 - tau*u*u',  tau
�������� � TauP,  u(1:i-1)=0, u(i)=1, u(i+1:n) �������� � A(i,i+1:n).

������:

m=6, n=5 (m > n):               m=5, n=6 (m < n):

(  d   e   u1  u1  u1 )         (  d   u1  u1  u1  u1  u1 )
(  v1  d   e   u2  u2 )         (  e   d   u2  u2  u2  u2 )
(  v1  v2  d   e   u3 )         (  v1  e   d   u3  u3  u3 )
(  v1  v2  v3  d   e  )         (  v1  v2  e   d   u4  u4 )
(  v1  v2  v3  v4  d  )         (  v1  v2  v3  e   d   u5 )
(  v1  v2  v3  v4  v5 )

����� vi � ui ���������� �������, ����������� H(i) � G(i), � d � e -
������������ � ��������������� �������� ������� B.
*************************************************************************/
void tobidiagonal(ap::real_2d_array& a,
     int m,
     int n,
     ap::real_1d_array& tauq,
     ap::real_1d_array& taup)
{
    ap::real_1d_array work;
    ap::real_1d_array t;
    int minmn;
    int maxmn;
    int i;
    double ltau;
    int mmip1;
    int nmi;
    int ip1;
    int nmip1;
    int mmi;

    minmn = ap::minint(m, n);
    maxmn = ap::maxint(m, n);
    work.setbounds(1, maxmn);
    t.setbounds(1, maxmn);
    taup.setbounds(1, minmn);
    tauq.setbounds(1, minmn);
    if( m>=n )
    {
        for(i = 1; i <= n; i++)
        {
            mmip1 = m-i+1;
            ap::vmove(t.getvector(1, mmip1), a.getcolumn(i, i, m));
            generatereflection(t, mmip1, ltau);
            tauq(i) = ltau;
            ap::vmove(a.getcolumn(i, i, m), t.getvector(1, mmip1));
            t(1) = 1;
            applyreflectionfromtheleft(a, ltau, t, i, m, i+1, n, work);
            if( i<n )
            {
                nmi = n-i;
                ip1 = i+1;
                ap::vmove(t.getvector(1, nmi), a.getrow(i, ip1, n));
                generatereflection(t, nmi, ltau);
                taup(i) = ltau;
                ap::vmove(a.getrow(i, ip1, n), t.getvector(1, nmi));
                t(1) = 1;
                applyreflectionfromtheright(a, ltau, t, i+1, m, i+1, n, work);
            }
            else
            {
                taup(i) = 0;
            }
        }
    }
    else
    {
        for(i = 1; i <= m; i++)
        {
            nmip1 = n-i+1;
            ap::vmove(t.getvector(1, nmip1), a.getrow(i, i, n));
            generatereflection(t, nmip1, ltau);
            taup(i) = ltau;
            ap::vmove(a.getrow(i, i, n), t.getvector(1, nmip1));
            t(1) = 1;
            applyreflectionfromtheright(a, ltau, t, i+1, m, i, n, work);
            if( i<m )
            {
                mmi = m-i;
                ip1 = i+1;
                ap::vmove(t.getvector(1, mmi), a.getcolumn(i, ip1, m));
                generatereflection(t, mmi, ltau);
                tauq(i) = ltau;
                ap::vmove(a.getcolumn(i, ip1, m), t.getvector(1, mmi));
                t(1) = 1;
                applyreflectionfromtheleft(a, ltau, t, i+1, m, i+1, n, work);
            }
            else
            {
                tauq(i) = 0;
            }
        }
    }
}


/*************************************************************************
��������� "����������" ������� Q, ���������� �������  A � ����������������
�����.

������� ���������:
    QP      -   ������� Q � P � ����������� �����.
                ��������� ������ ������������ ToBidiagonal.
    M       -   ����� ����� � ������� A
    N       -   ����� �������� � ������� A
    TAUQ    -   ��������� ���������, ����������� ������� Q.
                ��������� ������ ������������ ToBidiagonal.
    QColumns-   ��������� ����� �������� ������� Q. M >= QColumns >= 0

�������� ���������:
    Q       -   QColumns ������ �������� ������� Q.
                ������ � ���������� ��������� [1..M, 1..QColumns]
                ���� QColumns=0, �� ������ �� ����������.

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
void unpackqfrombidiagonal(const ap::real_2d_array& qp,
     int m,
     int n,
     const ap::real_1d_array& tauq,
     int qcolumns,
     ap::real_2d_array& q)
{
    int i;
    int j;
    int ip1;
    ap::real_1d_array v;
    ap::real_1d_array work;
    int vm;

    ap::ap_error::make_assertion(qcolumns<=m);
    if( m==0||n==0||qcolumns==0 )
    {
        return;
    }
    q.setbounds(1, m, 1, qcolumns);
    v.setbounds(1, m);
    work.setbounds(1, qcolumns);
    for(i = 1; i <= m; i++)
    {
        for(j = 1; j <= qcolumns; j++)
        {
            if( i==j )
            {
                q(i,j) = 1;
            }
            else
            {
                q(i,j) = 0;
            }
        }
    }
    if( m>=n )
    {
        for(i = ap::minint(n, qcolumns); i >= 1; i--)
        {
            vm = m-i+1;
            ap::vmove(v.getvector(1, vm), qp.getcolumn(i, i, m));
            v(1) = 1;
            applyreflectionfromtheleft(q, tauq(i), v, i, m, 1, qcolumns, work);
        }
    }
    else
    {
        for(i = ap::minint(m-1, qcolumns-1); i >= 1; i--)
        {
            vm = m-i;
            ip1 = i+1;
            ap::vmove(v.getvector(1, vm), qp.getcolumn(i, ip1, m));
            v(1) = 1;
            applyreflectionfromtheleft(q, tauq(i), v, i+1, m, 1, qcolumns, work);
        }
    }
}


/*************************************************************************
��������� "����������" ������� P, ���������� �������  A � ����������������
�����. ������������ ������� ����������������� ������� P.

������� ���������:
    QP      -   ������� Q � P � ����������� �����.
                ��������� ������ ������������ ToBidiagonal.
    M       -   ����� ����� � ������� A
    N       -   ����� �������� � ������� A
    TAUP    -   ��������� ���������, ����������� ������� P.
                ��������� ������ ������������ ToBidiagonal.
    PTRows  -   ��������� ����� ����� ������� P^T. N >= PTRows >= 0

�������� ���������:
    PT      -   PTRows ������ �������� ������� PT.
                ������ � ���������� ��������� [1..PTRows, 1..N]
                ���� PTRows=0, �� ������ �� ����������.


  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
void unpackptfrombidiagonal(const ap::real_2d_array& qp,
     int m,
     int n,
     const ap::real_1d_array& taup,
     int ptrows,
     ap::real_2d_array& pt)
{
    int i;
    int j;
    int ip1;
    ap::real_1d_array v;
    ap::real_1d_array work;
    int vm;

    ap::ap_error::make_assertion(ptrows<=n);
    if( m==0||n==0||ptrows==0 )
    {
        return;
    }
    pt.setbounds(1, ptrows, 1, n);
    v.setbounds(1, n);
    work.setbounds(1, ptrows);
    for(i = 1; i <= ptrows; i++)
    {
        for(j = 1; j <= n; j++)
        {
            if( i==j )
            {
                pt(i,j) = 1;
            }
            else
            {
                pt(i,j) = 0;
            }
        }
    }
    if( m>=n )
    {
        for(i = ap::minint(n-1, ptrows-1); i >= 1; i--)
        {
            vm = n-i;
            ip1 = i+1;
            ap::vmove(v.getvector(1, vm), qp.getrow(i, ip1, n));
            v(1) = 1;
            applyreflectionfromtheright(pt, taup(i), v, 1, ptrows, i+1, n, work);
        }
    }
    else
    {
        for(i = ap::minint(m, ptrows); i >= 1; i--)
        {
            vm = n-i+1;
            ap::vmove(v.getvector(1, vm), qp.getrow(i, i, n));
            v(1) = 1;
            applyreflectionfromtheright(pt, taup(i), v, 1, ptrows, i, n, work);
        }
    }
}


/*************************************************************************
"����������" ������� � �������� ���������� ����������������� ����������
������� A.

������� ���������:
    B       -   ��������� ������ ������������ ToBidiagonal.
    M       -   ����� ����� � ������� B
    N       -   ����� �������� � ������� B

�������� ���������:
    IsUpper -   ���� ������� ������� ����������������, �� True,
                ����� - False.
    D       -   ������� ���������.
                ������ � ���������� ��������� [1..Min(M,N)]
    E       -   �������� ��������� (������� ��� ������, � ����������� ��
                ��������� IsUpper).
                ������ � ���������� ��������� [1..Min(M,N)],   ���������
                ������� �� ������������.

  -- ALGLIB --
     Copyright 2005 by Bochkanov Sergey
*************************************************************************/
void unpackdiagonalsfrombidiagonal(const ap::real_2d_array& b,
     int m,
     int n,
     bool& isupper,
     ap::real_1d_array& d,
     ap::real_1d_array& e)
{
    int i;

    isupper = m>=n;
    if( m==0||n==0 )
    {
        return;
    }
    if( isupper )
    {
        d.setbounds(1, n);
        e.setbounds(1, n);
        for(i = 1; i <= n-1; i++)
        {
            d(i) = b(i,i);
            e(i) = b(i,i+1);
        }
        d(n) = b(n,n);
    }
    else
    {
        d.setbounds(1, m);
        e.setbounds(1, m);
        for(i = 1; i <= m-1; i++)
        {
            d(i) = b(i,i);
            e(i) = b(i+1,i);
        }
        d(m) = b(m,m);
    }
}
/*************************************************************************
������������ �������.

����������� ������� � ������� �� �� ����������� �������� (�� ������
����������� �������), � ������� �����.

������� ������:
    a - ������� � ���������� [1..n, 1..n]
    n - ������ �������

�������� ������:
    a - ����������������� �������
    d - ������������ ������� �������������� �������, ���������������
        ��� ������������ (A' = D^(-1)*A*D).
        ��������� ���������: [1..n]
*************************************************************************/
void balancematrix(ap::real_2d_array& a, int n, ap::real_1d_array& d)
{
    int last;
    int j;
    int i;
    double s;
    double r;
    double g;
    double f;
    double c;
    double sqrdx;

    d.setbounds(1, n);
    for(i = 1; i <= n; i++)
    {
        d(i) = 1;
    }
    sqrdx = ap::sqr(radix);
    do
    {
        last = 1;
        for(i = 1; i <= n; i++)
        {
            c = 0.0;
            r = 0.0;
            for(j = 1; j <= n; j++)
            {
                if( j!=i )
                {
                    c = c+fabs(a(j,i));
                    r = r+fabs(a(i,j));
                }
            }
            if( c!=0&&r!=0 )
            {
                g = double(r)/double(radix);
                f = 1.0;
                s = c+r;
                while(c<g)
                {
                    f = f*radix;
                    c = c*sqrdx;
                }
                g = r*radix;
                while(c>g)
                {
                    f = double(f)/double(radix);
                    c = c/sqrdx;
                }
                if( (c+r)/f<0.95*s )
                {
                    last = 0;
                    g = double(1)/double(f);
                    for(j = 1; j <= n; j++)
                    {
                        a(i,j) = a(i,j)*g;
                    }
                    for(j = 1; j <= n; j++)
                    {
                        a(j,i) = a(j,i)*f;
                    }
                    d(i) = d(i)*f;
                }
            }
        }
    }
    while(last==0);
}









