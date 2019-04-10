clear variables; close all; clc 

f  = 1000; 
dt = 1/f; 
t = 10:dt:15; 

std = linspace(0.5,30,10);      % vector with possible std's of velocity 
rv = std(1);                    % std of velocity 
rp = 0.5;                       % std of position 

% Initialize first time step 
x(1) = 1.6*sin(0.1*t(1))+4*sin(0.3*t(1))+0.8*sin(0.5*t(1))+2.4*sin(0.8*t(1));
y(1) = 0.8*sin(0.2*t(1))+2.4*sin(0.3*t(1))+4*sin(0.6*t(1))+2.4*sin(0.8*t(1)); 

vx(1) = 0; 
vy(1) = 0; 

npx(1,1:5) = x(1) + 0.5*randn(1,5); 
npy(1,1:5) = y(1) + 0.5*randn(1,5); 
nvx(1,1:5) = vx(1) + rv*randn(1,5); 
nvy(1,1:5) = vy(1) + rv*randn(1,5);

% Vector containing which particle should change 
r = zeros(1,length(t)); 
r([400:2000:length(t)])  = 1; 
r([800:2000:length(t)])  = 2; 
r([1200:2000:length(t)]) = 3; 
r([1600:2000:length(t)]) = 4; 
r([2000:2000:length(t)]) = 5; 


figure()
for i = 2: length(t)

    x(i) = 1.6*sin(0.1*t(i))+4*sin(0.3*t(i))+0.8*sin(0.5*t(i))+2.4*sin(0.8*t(i));
    y(i) = 0.8*sin(0.2*t(i))+2.4*sin(0.3*t(i))+4*sin(0.6*t(i))+2.4*sin(0.8*t(i)); 

    vx(i)  = (x(i) - x(i-1))/dt; 
    vy(i)  = (y(i) - y(i-1))/dt; 
    npx(i,1:5) = npx(i-1,1:5) + dt.*nvx(i-1,1:5);   
    npy(i,1:5) = npy(i-1,1:5) + dt.*nvy(i-1,1:5);
    nvx(i,1:5) = nvx(i-1,1:5); 
    nvy(i,1:5) = nvy(i-1,1:5); 

if mod(i,400) == 0
    npx(i,r(i)) = x(i) + rp*randn(1); 
    npy(i,r(i)) = y(i) + rp*randn(1); 
    nvx(i,r(i)) = vx(i) + rv*randn(1); 
    nvy(i,r(i)) = vy(i) + rv*randn(1);
else 
end 


plot(npx(i,:),npy(i,:),'ro')
hold on 
plot(x(i),y(i),'bo')
hold off 
xlim([-15 15])
ylim([-15 15])

pause(0.01)

end 