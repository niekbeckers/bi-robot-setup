function [xe, L, stable] = sim_lqg(target,doFF,checkStability)


% model parameters
dt = 0.01;
m = diag([4 1.5]); 
tu = 0.04;
td = 0.100;   
D = doFF*[0 15;-15 0];
gamma = 0.8;
noise = 0;

% dynamics matrices
[Ae,B,H] = dynamics(dt,m,tu,td,D);
Aim = dynamics(dt,m,tu,td,gamma*D);

% cost matrices
wp = params(1);
wv = params(2);
wf = params(3);
r = 1e-8;

Q = zeros(size(Ae));
R = zeros(size(B,2),size(B,2));

% terminal cost & running cost
Q(1:14,1:14) = diag([0 0 wv wv wf wf 0 0 0 0 wp wp wp wp]);
Q(13,11) = -wp;
Q(14,12) = -wp;
Q(11,13) = -wp;
Q(12,14) = -wp;

R(1,1,:) = r;
R(2,2,:) = r;

% initial state and state uncertainty
x0 = zeros(size(Ae,1),1);
x0(1:2) = target(1:2,1);
x0(7:8) = target(1:2,1);

% process noise
sigmaP_Ow = 0;
sigmaV_Ow = 0;
sigmaF_Ow = 0.00001/sqrt(0.01)*sqrt(dt);
sigmaPt_Ow = 0.00001/sqrt(0.01)*sqrt(dt);
sigmaVt_Ow = 0.00001/sqrt(0.01)*sqrt(dt);
sigmaPf_Ow = 0.00001/sqrt(0.01)*sqrt(dt);

Ow = zeros(size(Ae));
Ow(1:14,1:14) = 1000*diag([sigmaP_Ow^2 sigmaP_Ow^2 sigmaV_Ow^2 sigmaV_Ow^2 ...
    sigmaF_Ow^2 sigmaF_Ow^2 sigmaPt_Ow^2 sigmaPt_Ow^2 sigmaVt_Ow^2 sigmaVt_Ow^2 ...
    sigmaPf_Ow^2 sigmaPf_Ow^2 sigmaPf_Ow^2 sigmaPf_Ow^2]);

% sensory noise
sigmaP_Ov = 0.00001*sqrt(0.01)/sqrt(dt);
sigmaV_Ov = 0.00001*sqrt(0.01)/sqrt(dt);

Ov = diag([sigmaP_Ov^2 sigmaP_Ov^2 sigmaV_Ov^2 sigmaV_Ov^2 sigmaP_Ov^2 sigmaP_Ov^2 sigmaV_Ov^2 sigmaV_Ov^2]);

%% run LQG
[xe, L] = lqg_target(Ae,Aim,B,H,Q,R,Ow,Ov,x0,N,target,noise);


% check stability
stable = 1;
if checkStability
    for k = 1:size(L,3)
        if any(abs(eig(Ae - B*L(:,:,k))) >= 1)
            stable = 0;
        end
    end 
end
end