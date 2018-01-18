function [xe, L, stable] = sim_lqg(params,target,dt,doFF,checkStability)
assert(isa(checkStability,'double'));

%% model parameters
m = diag([1 1]); 
tu = 0.04;
td = 0.15;
D = doFF*[0 15;-15 0];
gamma = 0.8;
tp = 0;
N = size(target,2); % number of samples

%% dynamics matrices
[Ae,B,H] = dynamics(dt,m,tu,td,0,D);
Aim      = dynamics(dt,m,tu,td,0,gamma*D);

%% cost matrices
wp = params(1);
wv = params(2);
% wf = 0; % params(3);
r = 1; % 1e-8;

Q = zeros(size(Ae));
R = zeros(size(B,2),size(B,2));

Qp = zeros(8,8);
Qp(1,1,:) = wp;
Qp(2,2,:) = wp;
Qp(3,3,:) = wv;
Qp(4,4,:) = wv;
Qp(5,5,:) = wp;
Qp(6,6,:) = wp;
Qp(7,7,:) = wv;
Qp(8,8,:) = wv;
Qp(1,5,:) = -wp;
Qp(2,6,:) = -wp;
Qp(5,1,:) = -wp;
Qp(6,2,:) = -wp;
Qp(3,7,:) = -wv;
Qp(4,8,:) = -wv;
Qp(7,3,:) = -wv;
Qp(8,4,:) = -wv;
    
% prediction matrix
Mp = zeros(8,size(Ae,1));  

% prediction matrix (based on k)
Mp(1,1) = 1;
Mp(1,3) = tp;
Mp(2,2) = 1;
Mp(2,4) = tp;
Mp(3,3) = 1;
Mp(4,4) = 1;
Mp(5,7) = 1;
Mp(5,9) = tp;
Mp(6,8) = 1;
Mp(6,10) = tp;
Mp(7,9) = 1;
Mp(8,10) = 1;

Q = Mp'*Qp*Mp;

R(1,1,:) = r;
R(2,2,:) = r;

%% define process and observation noise
% process noise
sigmaU_Ov = 0.002/sqrt(0.01)*sqrt(dt);
sigmaT_Ov = 0.002/sqrt(0.01)*sqrt(dt);
sigmaTV_Ov = 0.0005/sqrt(0.01)*sqrt(dt);
Ow = zeros(size(Ae));
Ow(5,5) = sigmaU_Ov^2;
Ow(6,6) = sigmaU_Ov^2;
Ow(7,7) = sigmaT_Ov^2;
Ow(8,8) = sigmaT_Ov^2;
Ow(9,9) = sigmaTV_Ov^2;
Ow(10,10) = sigmaTV_Ov^2;

% sensory/observation noise
sigmaP_Ov = 0.05*0.005/sqrt(0.01)*sqrt(dt);
sigmaV_Ov = 0.05*0.005/sqrt(0.01)*sqrt(dt);
sigmaT_Ov = 0.05*0.005/sqrt(0.01)*sqrt(dt);
sigmaTV_Ov = 0.01*0.005/sqrt(0.01)*sqrt(dt);
% Ov = diag([sigmaP_Ov^2 sigmaP_Ov^2 sigmaT_Ov^2 sigmaT_Ov^2]);
Ov = diag([sigmaP_Ov^2 sigmaP_Ov^2 sigmaV_Ov^2 sigmaV_Ov^2 sigmaT_Ov^2 sigmaT_Ov^2 sigmaTV_Ov^2 sigmaTV_Ov^2]);

%% initial state and state uncertainty
x0 = zeros(size(Ae,1),1);
% x0(1:2) = target(1:2,1);
% x0(7:8) = target(1:2,1);

S0 = initializeS0(Ae,Aim,H,Ow,Ov,2000);

%% run LQG
[~,~,L,xe] = lqg_target(Ae,Aim,B,H,Q,R,Ow,Ov,x0,S0,N,dt,td,tp,target,0);

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