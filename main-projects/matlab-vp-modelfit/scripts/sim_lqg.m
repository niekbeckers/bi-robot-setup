function [xe, L, stable] = sim_lqg(params,target,dt,doFF,checkStability)
assert(isa(checkStability,'double'));

% model parameters
m = diag([0.3 0.3]); 
tu = 0.04;
td = 0.100;
delay = round(td/dt);
D = doFF*[0 15;-15 0];
gamma = 0.8;
noise = 0;

N = size(target,2); % number of samples

% dynamics matrices
[Ae,B,H] = dynamics(dt,m,tu,td,D);
Aim = dynamics(dt,m,tu,td,gamma*D);

% cost matrices
wp = params(1);
wv = params(2);
wf = params(3);
r = 1e-4; %1e-8

Q = zeros(size(Ae));
R = zeros(size(B,2),size(B,2));

%minimise distance between p and t
Q(1:10,1:10) = diag([wp wp wv wv wf wf wp wp 0 0]);
Q(7,1) = -wp;
Q(8,2) = -wp;
Q(1,7) = -wp;
Q(2,8) = -wp;
Q = Q; %*dt;

R(1,1,:) = r;
R(2,2,:) = r;
R = R; %*dt;

% initial state and state uncertainty
x0 = zeros(size(Ae,1),1);
x0(1:2) = target(1:2,1);
x0(7:8) = target(1:2,1);

sigma = 0.5136;
sigma_sens = 0.0001;

% process noise
sigmaP_Ow = 0;
sigmaV_Ow = 0;
sigmaF_Ow = sigma*sqrt(dt);
sigmaPt_Ow = 0;
sigmaVt_Ow = 0;

Ow = diag([sigmaP_Ow^2 sigmaP_Ow^2 sigmaV_Ow^2 sigmaV_Ow^2 ...
    sigmaF_Ow^2 sigmaF_Ow^2 sigmaPt_Ow^2 sigmaPt_Ow^2 sigmaVt_Ow^2 sigmaVt_Ow^2]);

% sensory noise
sigmaP_Ov = sigma_sens*sqrt(0.01)/sqrt(dt);
sigmaV_Ov = sigma_sens*sqrt(0.01)/sqrt(dt);

Ov = diag([sigmaP_Ov^2 sigmaP_Ov^2 sigmaV_Ov^2 sigmaV_Ov^2]);

%% generate noise (not used during fitting, can't be mex-compiled)
% rfactor = round(dt/0.001);
% whitenoise = randn([N*rfactor,1])*20; 
% 
% fs = 1000;
% fc1 = 1.7; %Hz
% [b1,a1] = butter(3,fc1/(fs/2));
% noisef1 = filter(b1,a1,whitenoise);
% 
% fc2 = 0.2;
% [b2,a2] = butter(1,fc2/(fs/2),'high');
% noisef2 = filter(b2,a2,noisef1);
% 
% % resample
% noisef2 = noisef2(rfactor:rfactor:N*rfactor);

noisef2 = zeros(length(target));
%% run LQG
[xe, L] = lqg_target(Ae,Aim,B,H,Q,R,Ow,Ov,noisef2,x0,N,target,noise,delay,dt); 

% check stability
stable = 1;
if checkStability
    for k = 1:size(L,3)-1
        if any(abs(eig(Ae - B*L(:,:,k))) > 1)
            stable = 0;
        end
    end 
end
end