clear variables; close all; clc 

f  = 1000; 
dt = 1/f; 
t = 0:dt:15; 

std = linspace(0.5,30,10);      % vector with possible std's of velocity 
rv = std(10);                       % std of velocity 
rp = 0.5;                        % std of position 

% Initialize first time step 
x(1) = 1.6*sin(0.1*2*pi*t(1))+4*sin(0.3*2*pi*t(1))+0.8*sin(0.5*2*pi*t(1))+2.4*sin(0.8*2*pi*t(1));
y(1) = 0.8*sin(0.2*2*pi*t(1))+2.4*sin(0.3*2*pi*t(1))+4*sin(0.6*2*pi*t(1))+2.4*sin(0.8*2*pi*t(1)); 

vx(1) = 0; 
vy(1) = 0; 

npx(1,1:5) = x(1) + rp*randn(1,5); 
npy(1,1:5) = y(1) + rp*randn(1,5); 
nvx(1,1:5) = vx(1) + rv*randn(1,5); 
nvy(1,1:5) = vy(1) + rv*randn(1,5);

% Vector containing which particle should change 
r = zeros(1,length(t)); 
r([80:400:length(t)])  = 1; 
r([160:400:length(t)]) = 2; 
r([240:400:length(t)]) = 3; 
r([320:400:length(t)]) = 4; 
r([400:400:length(t)]) = 5; 


figure()
h = plot(npx,npy,'ro');
% hold on 
% g = plot(x(end,:),y(end,:),'bo');
hold off 
xlim([-20 20])
ylim([-20 20])
F(1) = getframe(gca); 

for i = 2: length(t)
    x(i) = 1.6*sin(0.1*2*pi*t(i))+4*sin(0.3*2*pi*t(i))+0.8*sin(0.5*2*pi*t(i))+2.4*sin(0.8*2*pi*t(i));
    y(i) = 0.8*sin(0.2*2*pi*t(i))+2.4*sin(0.3*2*pi*t(i))+4*sin(0.6*2*pi*t(i))+2.4*sin(0.8*2*pi*t(i)); 

    vx(i)  = (x(i) - x(i-1))/dt; 
    vy(i)  = (y(i) - y(i-1))/dt; 
    npx(i,1:5) = npx(i-1,1:5) + dt.*nvx(i-1,1:5);   
    npy(i,1:5) = npy(i-1,1:5) + dt.*nvy(i-1,1:5);
    nvx(i,1:5) = nvx(i-1,1:5); 
    nvy(i,1:5) = nvy(i-1,1:5); 

if mod(i,80) == 0
    npx(i,r(i)) = x(i) + rp*randn(1); 
    npy(i,r(i)) = y(i) + rp*randn(1); 
    nvx(i,r(i)) = vx(i) + rv*randn(1); 
    nvy(i,r(i)) = vy(i) + rv*randn(1);
    display(['i = ' num2str(i)]); 
end 


xn = npx(i,:); 
yn = npy(i,:); 
xr = x(i); 
yr = y(i);


set(h, 'xdata' , xn, 'ydata', yn)
% set(g, 'xdata' , xr, 'ydata', yr)

drawnow limitrate 

F(i) = getframe(gca); 

end 

%% Make a video of the trajectory 

v = VideoWriter('VN_v_0.05.avi','Motion JPEG AVI');
v.Quality = 100; 
v.FrameRate = 1000; 

open(v) 
writeVideo(v,F)
close(v)

%% Show trajectory 

npx_mean = (mean(npx,2));  
npy_mean = (mean(npy,2));

figure 
plot(x(1:find(t==10)),y(1:find(t==10)),'b')
hold on 
plot(npx_mean(1:find(t==10)),npy_mean(1:find(t==10)),'r') 
xlim([-20 20])
ylim([-20 20])





