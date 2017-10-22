function [xe, L] = lqg_target(Ae,Aim,B,H,Q,R,Ow,Ov,x0,N,target,noise)
%% function [K,L,xe,u,y,xhat,P,xnoise] = lqg(Ae,Aim,B,H,Q,R,Ow,Ov,x0,P0,N,Nsim)

% Ae        System matrix of environment (real system)
% Aim       System matrix of internal model
% B         Input matrix (assumed to be the same for environment and internal model

nX = size(Ae,1);
nU = size(B,2);
nY = size(H,1);

% preallocate matrices and vectors
xe = zeros(nX,N);
xhat = zeros(nX,N);
xhatp = zeros(nX,N);
y = zeros(nY,N);
u = zeros(nU,N-1);
P = zeros(nX,nX,N);
K = zeros(nX,nY,N);

% initialize
xe(:,1) = x0;
xhat(:,1) = x0; 
xhatp(:,1) = x0;
y(:,1) = H*xe(:,1);

% retrieve control law
L_fast = controllaw_infinitehorizon(Aim(1:14,1:14),B(1:14,:),Q(1:14,1:14),R,N);
L = zeros(nU,nX,N-1);
L(:,1:14,:) = L_fast;

% L = controllaw_infinitehorizon(Aim,B,Q,R,N);

Gain = L(:,:,1);

% forward pass
for k = 1:N-1
    % calculate kalman gain
    K(:,:,k) = (P(:,:,k)*H')/(H*P(:,:,k)*H'+Ov);
    
    % update the state uncertainty
    P(:,:,k) = (eye(nX)-K(:,:,k)*H)*P(:,:,k);
    % estimate new state uncertainty (for next time step)
    P(:,:,k+1) = Aim*P(:,:,k)*Aim' + Ow;
    
    % posterior state estimate
    xhat(:,k) = xhatp(:,k) + K(:,:,k)*(y(:,k)-H*xhatp(:,k));
    
    % control input
    u(:,k) = -L(:,:,k)*xhat(:,k);
    
    % simulate internal model: priori state estimate
    xhatp(:,k+1) = Aim*xhat(:,k) + B*u(:,k);
    
    % simulate real system
    xe(:,k+1) = Ae*xe(:,k) + B*u(:,k) + noise*sqrt(Ow)*randn(nX,1);
    xe(7:10,k+1) = target(:,k+1);
    y(:,k+1) = H*xe(:,k+1) + noise*sqrt(Ov)*randn(nY,1);
end