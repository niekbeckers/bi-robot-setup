function [K,S,L,xe,ye,xhat,xsim,ysim] = lqg_target(Ae,Aim,B,H,Q,R,Ow,Ov,x0,S0,N,dt,td,tpred,target,Nsim)
%% function [K,S,L,xe,ye,xhat,xsim,ysim] = lqg_target(Ae,Aim,B,H,Q,R,Ow,Ov,x0,S0,N,dt,td,tpred,target,Nsim)
%
% Linear Quadratic Gaussian control problem: state estimation using a
% Kalman filter in combination with a Linear Quadratic Regulator for
% control. 
%
% INPUT
% Ae        System matrix of environment (real system)
% Aim       System matrix of internal model
% B         Input matrix (assumed to be the same for environment and internal model)
% H         Observation matrix
% Q         State cost matrix
% R         Input cost matrix
% Ow        Process noise covariance matrix
% Ov        Observation noise covariance matrix
% x0        Initial state
% S0        Initial state uncertainty matrix
% N         Number of time steps
% dt        Time step
% td        Sensory delay (observation delay)
% tpred     If tpred > 0, the human predicts (through extrapolation) the position of the target
% target    Target position (x x N)
% Nsim      If Nsim > 0, simulate the system (with noise)
%
% OUTPUT
% K         Kalman gains
% L         Control policy gains
% xe        Real system state vector (environment)
% ye        Real system observation vector (environment)
% xhat      Posteriori state estimate
% xsim      Simulated real system state vector (for Nsim repetitions)
% ysim      Simulated real observation vector (for Nsim repetitions)
% S         System state uncertainty
%
% Models and code based on Izawa and Shadmehr (2008), Li and Todorov
% (2006), Diedrichsen (2007), 
% Niek Beckers

nX = size(Ae,1);
nU = size(B,2);
nY = size(H,1);

% preallocate matrices and vectors
xe = zeros(nX,N);
xhat = zeros(nX,N);
xhatp = zeros(nX,N);
ye = zeros(nY,N);
u = zeros(nU,N-1);
S = zeros(nX,nX,N);
K = zeros(nX,nY,N);

% initialize state estimate uncertainty
S(:,:,1) = S0;

% initial states
xe(:,1) = x0;
xhat(:,1) = x0; % initial state estimate
xhatp(:,1) = x0;
ye(:,1) = H*xe(:,1);

% retrieve control law
L = controllaw(Aim,B,Q,R,N);

% generate process and observation noise
w = sqrt(Ow)*randn(nX,N-1);
v = sqrt(Ov)*randn(nY,N);

% forward pass
for k = 1:N-1
    % measurement update
    % calculate kalman gain
    K(:,:,k) = (S(:,:,k)*H')/(H*S(:,:,k)*H'+Ov);

    % posterior state estimate
    xhat(:,k) = xhatp(:,k) + K(:,:,k)*(ye(:,k)-H*xhatp(:,k));

    % update the state uncertainty
    S(:,:,k) = (eye(nX)-K(:,:,k)*H)*S(:,:,k);

    % control input
    u(:,k) = -L(:,:,k)*xhat(:,k);
    
    % time update: forward model predication
    % simulate internal model: prior state estimate
    xhatp(:,k+1) = Aim*xhat(:,k) + B*u(:,k);

    % estimate new state uncertainty (for next time step)
    S(:,:,k+1) = Aim*S(:,:,k)*Aim.' + Ow;

    % set target
    xe(7:10,k) = target(:,k);
    
    % simulate real system (including system and observation noise)
    xe(:,k+1) = Ae*xe(:,k) + B*u(:,k) + w(:,k);
  
    % observation
    ye(:,k+1) = H*xe(:,k+1) + v(:,k+1);
end

% calculate noisy trajectories (if desired)
if Nsim > 0
    [xsim,ysim] = simulate(Ae,Aim,B,H,K,L,Ow,Ov,S0,x0,N,dt,td,tpred,target,Nsim);
    
end


function [xe,ye] = simulate(Ae,Aim,B,H,K,L,Ow,Ov,S0,x0,N,dt,td,tpred,target,Nsim)
%% function [xe,ye] = simulate(Ae,Aim,B,H,K,L,Ow,Ov,S0,x0,N,dt,td,tpred,target,Nsim)
% Simulate trajectories (Nsim times), with noise

nX = size(Ae,1);
nY = size(H,1);
nOv = size(Ov,2);
nOw = size(Ow,2);

% time delay expressed in multiples of timestep
Ntd = round(td/dt);

% square root of P0 to get initial state uncertainty
[u,s,v] = svd(S0);
sqrtS0 = u*diag(sqrt(diag(s)))*v';

xe = zeros(nX,Nsim,N);
xe(:,:,1) = repmat(x0,[1 Nsim]) + sqrtS0*randn(nX,Nsim);
ye = zeros(nY,Nsim,N);
xhat = zeros(nX,Nsim,N);
xhat(:,:,1) = repmat(x0,[1 Nsim]);

try
for k = 1:N-1
    % control input
    u = -L(:,:,k)*xhat(:,:,k);

    % simulate internal model / posterior state estimate
    xprior = Aim*xhat(:,:,k) + B*u;
    
    % update
    xhat(:,:,k+1) = xprior + K(:,:,k)*(ye(:,:,k)-H*xhat(:,:,k));

    % simulate real system
    xe(:,:,k+1) = Ae*xe(:,:,k) + B*u + sqrt(Ow)*randn(nOw,Nsim); 
    % compute noisy observation
    ye(:,:,k+1) = H*xe(:,:,k+1) + sqrt(Ov)*randn(nOv,Nsim);
    
    % update target location
    if k > Ntd
        if tpred > 0
            % predict: ptarget_pred = ptarget(k-td) + vtarget(k-td)*tpred
            vtarget_pred = (target(:,k+1-Ntd) - target(:,k-Ntd))/dt;
            ptarget_pred = target(:,k+1-Ntd)+vtarget_pred*tpred;
            ye(5:6,:,k+1) = repmat(ptarget_pred,[1 Nsim]);
        else
            % no target prediction
            ye(5:6,:,k+1) = repmat(target(:,k+1-Ntd),[1 Nsim]);
        end
    end
    
end
catch me
    getReport(me)
    keyboard
end
%     
% end