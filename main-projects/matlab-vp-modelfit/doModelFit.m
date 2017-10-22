function [pfit, fval, errorFlag, fitInfo] = doModelFit(data,trial,p0,condition)

% Fits virtual agent to experimental data. And returns the optimal fit 
% parameters for the position, velocity and force costs of the optimal
% control. The output errorFlag warns for system instability or an 
% inaccurate fit.
%
% Jolein van der Sluis, 2017

% persistent p0_saved

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

% itt = 3;
% pfit= zeros(itt,3); % (ii,pfit)
% p0 = zeros(itt,3);
% error = zeros(itt,1);
% 
% for ii = 1:itt
%     if length(p0_saved) == 3
%         % cost_p, cost_v, cost_F
%         p0(ii,:) = [normrnd(p0_saved(1),ub(1)/12) normrnd(p0_saved(2),ub(2)/12) normrnd(p0_saved(3),ub(3)/12)];
%     else 
%         p0(ii,:) = [rand*ub(1) rand*ub(2) rand*ub(3)];  
%     end
%     [pfit(ii,:),error(ii),exitflag(ii),output(ii)] = fmincon(fun,p0(ii,:),[],[],[],[],lb,ub,[],opts);
% end 
% 
% [min_e,index] = min(error);
% pfit_opt = pfit(index,:);
% p0_saved = pfit_opt;

[pfit,fval,exitflag,output] = fmincon(fun,p0,[],[],[],[],lb,ub,[],opts);
fitInfo = struct;
fitInfo.trial = trial;
fitInfo.fval = fval;
fitInfo.exitflag = exitflag;
fitInfo.output = output;

%% simulate to evaluate goodness of fit
[xe,L] = sim_lqg(target,FF);

%% VAF, stability
VAF_px = (1-(var(xe(1,:).'-xmeas(1,:).')./var(xmeas(1,:).')))*100;
VAF_py = (1-(var(xe(2,:).'-xmeas(2,:).')./var(xmeas(2,:).')))*100;

stability = abs(eig(Ae - B*L));

% works only if trials alternate SDSDSD in a continuous fashion
% fitInfo(Trial/2) = struct('Trialnr',Trial,'fiterror', min(error), 'p0', p0(index,:),'pfit',...
%     pfit_opt,'exitflag', exitflag(index),'nrIterations',output(index).iterations);

% fitInfo(trial/2).Trialnr = trial;
% fitInfo(trial/2).fiterror = min(error);
% fitInfo(trial/2).p0 = p0(index,:);
% fitInfo(trial/2).pfit = pfit;
% fitInfo(trial/2).eixtFlag = exitflag(index);
% fitInfo(trial/2).nrIterations = output(index).iterations;

if VAF_px>=80 && VAF_py>=80 && min_e<=0.01 && max(abs(stability))<=1 
    errorFlag = 0;
elseif max(abs(stability))>=1
    errorFlag = 1;               % unstable system
else
    errorFlag = 2;               % bad performance (VAF or min_e wrong)
end