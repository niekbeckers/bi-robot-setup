function [pfit, fvalfit, fitInfo, errorFlag] = doModelFit(data,dt,p0,condition)

% Fits virtual agent to experimental data. And returns the optimal fit 
% parameters for the position, velocity and force costs of the optimal
% control. The output errorFlag warns for system instability or an 
% inaccurate fit.
%
% Jolein van der Sluis, 2017
%
% Change log
% 23-10-17 / NB
% Took out iterations, now in outer loop for more efficient parallel
% pool process scheduling. Made sim_lqg function which does the simulation
% of the LQG (used here to assess performance and in the fit function). 

% select data
xmeas = data(:,1:4).';        % pos_x,pos_y,vel_x,vel_y
target = data(:,5:8).';       % pos_x,pos_y,vel_x,vel_y

% based on condition, change model (force field yes/no)
if condition
    doFF = 1;
else
    doFF = 0;
end

%% setup fit function and parameters
% fit function
fun = @(x)fitfun_invoc_mex(x,dt,xmeas,target,doFF);

% fmincon settings
maxIter = 75;
opts = optimoptions('fmincon',...
    'display','iter',...
    'MaxIterations',maxIter,...
    'useparallel',false); % 'Algorithm','interior-point'
% bounds
lb = [0;0;0];
ub = [1e4;1e2;1e-2];

% perform fit
[pfit,fvalfit,exitflag,output] = fmincon(fun,p0,[],[],[],[],lb,ub,[],opts);

% store results and settings in struct
fitInfo = struct;
fitInfo.pfit = pfit;
fitInfo.fval = fvalfit;
fitInfo.exitflag = exitflag;
fitInfo.output = output;
fitInfo.p0 = p0;
fitInfo.ub = ub;
fitInfo.lb = lb;
fitInfo.fitfun = 'fitfun_invoc';


%% simulate to evaluate goodness of fit and stability
[xe,~,stable] = sim_lqg(pfit,target,dt,doFF, 1);

%% evaluate performance
% VAF (percentage)
VAF_px = (1-(var(xe(1,:)-xmeas(1,:))./var(xmeas(1,:))))*100;
VAF_py = (1-(var(xe(2,:)-xmeas(2,:))./var(xmeas(2,:))))*100;

% difference in tracking error between human and agent
eh = mean(sqrt(sum((xmeas(1:2,:)-target(1:2,:)).^2,1)));
evp = mean(sqrt(sum((xe(1:2,:)-target(1:2,:)).^2,1)));
error_diff = abs(eh-evp);

if (VAF_px >= 80) && (VAF_py >= 80) && stable && (error_diff <= 0.03)
    errorFlag = 0;
elseif ~stable
    errorFlag = 1;                  % unstable system
else
    if (VAF_px >= 80) && (VAF_py >= 80)
        errorFlag = 2;              % bad performance; tracking error difference too high
    else 
        errorFlag = 3;              % bad performance; VAF too low
    end
end