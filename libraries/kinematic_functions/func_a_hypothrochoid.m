function a = func_a_hypothrochoid(t,T,n,hstart,hend,wstart,wend)
%FUNC_A_HYPOTHROCHOID
%    A = FUNC_A_HYPOTHROCHOID(T,T,N,HSTART,HEND,WSTART,WEND)

%    This function was generated by the Symbolic Math Toolbox version 7.1.
%    01-Feb-2017 15:33:03

t2 = n+1.0;
t3 = 1.0./t2;
t4 = wend.^2;
t5 = 1.0./hend;
t6 = n-1.0;
t8 = hend.*n.*t3.*2.0;
t9 = hend.*t3.*t6;
t7 = t8-t9;
t10 = t.*wend;
t11 = 1.0./t6;
t12 = t.*t2.*t5.*t7.*t11.*wend;
t13 = t7.^2;
t14 = 1.0./t6.^2;
t15 = t2.^2;
a = [-t4.*t7.*cos(t10)-t4.*t5.*t13.*t14.*t15.*cos(t12);-t4.*t7.*sin(t10)+t4.*t5.*t13.*t14.*t15.*sin(t12)];