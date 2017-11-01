function [xe, L] = lqg_target(Ae,Aim,B,H,Q,R,Ow,Ov,x0,N,target,noise,delay)
%% function [K,L,xe,u,y,xhat,P,xnoise] = lqg(Ae,Aim,B,H,Q,R,Ow,Ov,x0,P0,N,Nsim)

% Ae        System matrix of environment (real system)
% Aim       System matrix of internal model
% B         Input matrix (assumed to be the same for environment and internal model

nX = size(Ae,1);
nU = size(B,2);
nY = size(H,1);

% preallocate matrices and vectors
xe = repmat(x0,1,N);
xhat = repmat(x0,1,N);
xhatp = repmat(x0,1,N);
y = zeros(nY,N);
u = zeros(nU,N-1);
P = zeros(nX,nX,N);
K = zeros(nX,nY,N);

% initialize
y(:,1) = H*xe(:,1);

% retrieve control law
L = controllaw(Aim,B,Q,R,N);

% forward pass
for k = 1:N-1
    if k > delay
        % calculate kalman gain
        K(:,:,k) = (P(:,:,k)*H')/(H*P(:,:,k)*H'+Ov);

        % update the state uncertainty
        P(:,:,k) = (eye(nX)-K(:,:,k)*H)*P(:,:,k);
        % estimate new state uncertainty (for next time step)
        P(:,:,k+1) = Aim*P(:,:,k)*Aim' + Ow;

        % posterior state estimate
        xhat(:,k-delay) = xhatp(:,k-delay) + K(:,:,k)*(y(:,k)-H*xhatp(:,k-delay));

        x_comp = xhat(:,k-delay);
        for s = 1:delay
            x_comp = Ae*x_comp+B*u(:,k-delay+s-1);
        end

        % control input
        u(:,k) = -L(:,:,k)*x_comp;

        % simulate internal model: priori state estimate
        xhatp(:,k+1-delay) = Aim*xhat(:,k-delay) + B*u(:,k-delay);

        % simulate real system
        xe(:,k+1) = Ae*xe(:,k) + B*u(:,k) + noise*sqrt(Ow)*randn(nX,1);
        xe(7:10,k+1) = target(:,k+1);
        y(:,k+1) = H*xe(:,k+1-delay) + noise*sqrt(Ov)*randn(nY,1);
    end
end