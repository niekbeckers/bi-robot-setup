function [pfit_opt, errorFlag, fitInfo] = doModelFit(data,trial,condition)

% Fits virtual agent to experimental data. And returns the optimal fit 
% parameters for the position, velocity and force costs of the optimal
% control. The output errorFlag warns for system instability or an 
% inaccurate fit.
%
% Jolein van der Sluis, 2017

persistent p0_saved

% select data
xmeas = data(1:4,:);        % pos_x,pos_y,vel_x,vel_y
target = data(5:8,:);       % pos_x,pos_y,vel_x,vel_y

if (condition == 1)
    FF = 1;
else
    FF = 0;
end

%% resample and skip the first part
dt = 0.01;
N = size(xmeas,2);

%% fit parameters
fun = @(x)fitfun_invoc_mex(x,xmeas,target,FF,N);

% fmincon settings
opts = optimoptions('fmincon','display', 'iter','MaxIterations',75);
ub = [10000;10;0.01];
lb = [0;0;0];

itt = 3;
pfit= zeros(itt,3); % (ii,pfit)
p0 = zeros(itt,3);
error = zeros(itt,1);

for ii = 1:itt
    if length(p0_saved) == 3
        % cost_p, cost_v, cost_F
        p0(ii,:) = [normrnd(p0_saved(1),ub(1)/12) normrnd(p0_saved(2),ub(2)/12) normrnd(p0_saved(3),ub(3)/12)];
    else 
        p0(ii,:) = [rand*ub(1) rand*ub(2) rand*ub(3)];  
    end
    [pfit(ii,:),error(ii),exitflag(ii),output(ii)] = fmincon(fun,p0(ii,:),[],[],[],[],lb,ub,[],opts);
end 

[min_e,index] = min(error);
pfit_opt = pfit(index,:);
p0_saved = pfit_opt;

%% simulate to evaluate goodness of fit
% model parameters
N = size(xmeas,2);
m = diag([4 1.5]); 
tu = 0.04;
td = 0.100; 
D = FF*[0 15;-15 0];
gamma = 0.8;
noise = 1;

% dynamics matrices
[Ae,B,H] = dynamics(dt,m,tu,td,D);
Aim = dynamics(dt,m,tu,td,gamma*D);

%cost matrices
wp = pfit_opt(1);
wv = pfit_opt(2);
wf = pfit_opt(3);
r = 1e-8;

Q = zeros([size(Ae)]);
R = zeros(size(B,2),size(B,2));

%terminal cost & running cost
Q(1:14,1:14) = diag([0 0 wv wv wf wf 0 0 0 0 wp wp wp wp]);
Q(13,11) = -wp;
Q(14,12) = -wp;
Q(11,13) = -wp;
Q(12,14) = -wp;

R = diag([r r]);

%initial state and state uncertainty
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
Ow(1:14,1:14) = diag([sigmaP_Ow^2 sigmaP_Ow^2 sigmaV_Ow^2 sigmaV_Ow^2 ...
    sigmaF_Ow^2 sigmaF_Ow^2 sigmaPt_Ow^2 sigmaPt_Ow^2 sigmaVt_Ow^2 sigmaVt_Ow^2 ...
    sigmaPf_Ow^2 sigmaPf_Ow^2 sigmaPf_Ow^2 sigmaPf_Ow^2]);

%sensory noise
sigmaP_Ov = 0.00001*sqrt(0.01)/sqrt(dt);
sigmaV_Ov = 0.00001*sqrt(0.01)/sqrt(dt);

Ov = diag([sigmaP_Ov^2 sigmaP_Ov^2 sigmaV_Ov^2 sigmaV_Ov^2 sigmaP_Ov^2 sigmaP_Ov^2 sigmaV_Ov^2 sigmaV_Ov^2]);

%% run LQG to check performance
[xe,Gain] = lqg_target(Ae,Aim,B,H,Q,R,Ow,Ov,x0,N,target,noise);

VAF_px = (1-(var(xe(1,:).'-xmeas(1,:).')./var(xmeas(1,:).')))*100;
VAF_py = (1-(var(xe(2,:).'-xmeas(2,:).')./var(xmeas(2,:).')))*100;

stability = abs(eig(Ae - B*Gain));

% works only if trials alternate SDSDSD in a continuous fashion
% fitInfo(Trial/2) = struct('Trialnr',Trial,'fiterror', min(error), 'p0', p0(index,:),'pfit',...
%     pfit_opt,'exitflag', exitflag(index),'nrIterations',output(index).iterations);

fitInfo(trial/2).Trialnr = trial;
fitInfo(trial/2).fiterror = min(error);
fitInfo(trial/2).p0 = p0(index,:);
fitInfo(trial/2).pfit = pfit_opt;
fitInfo(trial/2).eixtFlag = exitflag(index);
fitInfo(trial/2).nrIterations = output(index).iterations;

if VAF_px>=80 && VAF_py>=80 && min_e<=0.01 && max(abs(stability))<=1 
    errorFlag = 0;
elseif max(abs(stability))>=1
    errorFlag = 1;               % unstable system
else
    errorFlag = 2;               % bad performance (VAF or min_e wrong)
end