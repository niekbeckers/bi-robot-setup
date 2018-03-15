function [pfit, fvalfit, fitInfo, errorFlag, gof, eh, evp, xvp] = doModelFit(data,dt,p0,condition)

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
if isunix
    maxIter = 250;
else
    maxIter = 75;
end
opts = optimoptions('fmincon',...
    'display','final-detailed',...
    'MaxIterations',maxIter,...
    'useparallel',false); % 'Algorithm','interior-point'

% bounds
lb = [0;0;0];
ub = [1e3;1e1;1e-2];

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

% goodness of fit struct
gof = struct;

%% simulate to evaluate goodness of fit and stability
[xvp,~,stable] = sim_lqg(pfit,target,dt,doFF,1);

gof.stable = stable;
%% evaluate performance
% VAF (percentage)
VAF_px = (1-(var(xvp(1,:)-xmeas(1,:))./var(xmeas(1,:))))*100;
VAF_py = (1-(var(xvp(2,:)-xmeas(2,:))./var(xmeas(2,:))))*100;
gof.VAF_px = VAF_px;
gof.VAF_py = VAF_py;

eh = sqrt((target(1,:)-xmeas(1,:)).^2+(target(2,:)-xmeas(2,:)).^2);
evp = sqrt((target(1,:)-xvp(1,:)).^2+(target(2,:)-xvp(2,:)).^2);

Eh = mean(eh);
Evp = mean(evp);

% to get the tracking error of agent and human equal
E_diff = abs(Eh - Evp);

% to get trajectories to be similar
% e_fit = mean(abs(error_human-error_agent));

gof.E_diff = E_diff;
gof.Eh = Eh;
gof.Evp = Evp;


if (VAF_px >= 80) && (VAF_py >= 80) && stable && (E_diff <= 0.03)
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