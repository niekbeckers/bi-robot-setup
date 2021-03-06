function pos = func_eq_FK(Q1,Q5,Lf,Lu,Lb)
%FUNC_EQ_FK
%    POS = FUNC_EQ_FK(Q1,Q5,LF,LU,LB)

%    This function was generated by the Symbolic Math Toolbox version 7.1.
%    08-Dec-2016 13:36:14

t2 = sin(Q1);
t3 = Lu.*t2;
t4 = sin(Q5);
t9 = Lu.*t4;
t5 = t3-t9;
t6 = cos(Q1);
t7 = cos(Q5);
t11 = Lu.*t6;
t12 = Lu.*t7;
t8 = Lb+t11-t12;
t10 = t5.^2;
t13 = t8.^2;
t14 = t10+t13;
t15 = 1.0./sqrt(t14);
t16 = Lf.^2;
t17 = t10.*(-1.0./4.0)-t13.*(1.0./4.0)+t16;
t18 = sqrt(t17);
pos = [Lu.*t6.*(1.0./2.0)+Lu.*t7.*(1.0./2.0)-t5.*t15.*t18;Lu.*t2.*(1.0./2.0)+Lu.*t4.*(1.0./2.0)+t8.*t15.*t18];
