%% hypothrochoid_trajectory
clear all; close all; clc;

% create symbolic functions
syms t x y xdot xdd ydd h hend hstart n a b w phx phy wstart wend T A real

% h = hstart + (hend-hstart)/T * t;

h = hstart+(hend-hstart)/2*sin(2*pi*0.1*t);

% h(t) = (hend-hstart)/2*sin(2*pi*0.1*t);
% h = hend;
a(t) = 2*n*h/(n+1);
b(t) = (n-1)*h/(n+1);

% w(t) = wstart + (wend-wstart)/T * t;
% w(t) = wend*(1/(1+exp(-0.2*(t-10)))+0.5);
% w(t)  = (wend-wstart)/2*sin(2*pi*0.1*t);
% w(t) = 1;

w = wend;

x(t) = [((a-b)*cos(w*t)+h*cos((a-b)/b*w*t));
        ((a-b)*sin(w*t)-h*sin((a-b)/b*w*t))];
    
sc = (1./(1+exp(-(t-5))));

x = x*sc;

% differentiate
xd(t) = diff(x,t);
xdd(t) = diff(xd,t);

% create functions from symbolic functions
matlabFunction(x,'File','func_p_hypothrochoid','Outputs',{'p'}, 'Vars',{t,T,n,hstart,hend,wstart,wend});
matlabFunction(xd,'File','func_v_hypothrochoid','Outputs',{'v'}, 'Vars',{t,T,n,hstart,hend,wstart,wend});
matlabFunction(xdd,'File','func_a_hypothrochoid','Outputs',{'a'}, 'Vars',{t,T,n,hstart,hend,wstart,wend});

% simulate signals
T = 80;
dt = 0.001;
t = (0:dt:T)';
hstart = 0.045;
hend = 0.045;
n = 6;
wstart = 0.5;%0.2*2*pi;
wend = 0.5;%0.2*2*pi;

x_func = func_p_hypothrochoid(t',T,n,hstart,hend,wstart,wend);
xdot_func = func_v_hypothrochoid(t',T,n,hstart,hend,wstart,wend);
xddot_func = func_a_hypothrochoid(t',T,n,hstart,hend,wstart,wend);
 
figure('color','w')
subplot(2,3,[1 2 4 5])
plot(x_func(1,:),x_func(2,:)); hold on;
title('position'); xlabel('time, s');
% legend('x - numerical','y - numerical','x - analytical','y - numerical')

subplot(2,3,3)
plot(t,xdot_func(1,:),'--',t,xdot_func(2,:),'--');
title('velocity'); xlabel('time, s');

subplot(2,3,6)
plot(t,xddot_func(1,:),'--',t,xddot_func(2,:),'--');
title('acceleration'); xlabel('time, s')
 


