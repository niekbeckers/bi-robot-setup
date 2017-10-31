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

% persistent p0_saved

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
opts = optimoptions('fmincon','display','iter','MaxIterations',maxIter,'useparallel',false);
% bounds
ub = [10000;10;0.01];
lb = [0;0;0];

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

% VAF (percentage)
VAF_px = (1-(var(xe(1,:)-xmeas(1,:))./var(xmeas(1,:))))*100;
VAF_py = (1-(var(xe(2,:)-xmeas(2,:))./var(xmeas(2,:))))*100;


if (VAF_px >= 80) && (VAF_py >= 80) && stable 
    errorFlag = 0;
elseif ~stable
    errorFlag = 1;               % unstable system
else
    errorFlag = 2;               % bad performance (VAF or min_e wrong)
end

    eh = sqrt(sum((xmeas(1:2,:)-target(1:2,:)).^2,1));
    evp = sqrt(sum((xe(1:2,:)-target(1:2,:)).^2,1));
    mean_eh = mean(eh)
    mean_evp = mean(evp)
    
    MSEh = mean(eh.^2);
    MSEvp = mean(evp.^2); 

    pos_error_human = sqrt((target(1,:)-xmeas(1,:)).^2+(target(2,:)-xmeas(2,:)).^2);
    pos_error_agent = sqrt((target(1,:)-xe(1,:)).^2+(target(2,:)-xe(2,:)).^2);

    % to get the mean error of the agent and human equal
    pos_error_diff = abs(mean(pos_error_human) - mean(pos_error_agent));
    fit_error = mean(abs(pos_error_human-pos_error_agent));

plot(xe(1,:))