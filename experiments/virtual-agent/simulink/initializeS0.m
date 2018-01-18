function S0 = initializeS0(Ae,Aim,H,Ow,Ov,N)
%% function S0 = initializeS0(Ae,Aim,H,Ow,Ov,N)
% Make an initial guess of the covariance matrix
%
% Niek Beckers



% preallocate matrices and vectors
nX = size(Ae,1);
nY = size(H,1);
xhat = zeros(nX,N);
xhatp = zeros(nX,N);
ye = zeros(nY,N);
S = zeros(nX,nX,N);
K = zeros(nX,nY,N);

% initialize state estimate uncertainty
S(:,:,1) = zeros(size(Ae)); % initial state uncertaintyS(:,:,1) = eye(size(Ae))*1
for k = 1:N-1
    % measurement update
    % calculate kalman gain
    K(:,:,k) = (S(:,:,k)*H')/(H*S(:,:,k)*H'+Ov);

    % posterior state estimate
    xhat(:,k) = xhatp(:,k) + K(:,:,k)*(ye(:,k)-H*xhatp(:,k));

    % update the state uncertainty
    S(:,:,k) = (eye(nX)-K(:,:,k)*H)*S(:,:,k);

    % time update: forward model predication
    % simulate internal model: prior state estimate
    xhatp(:,k+1) = Aim*xhat(:,k);

    % estimate new state uncertainty (for next time step)
    S(:,:,k+1) = Aim*S(:,:,k)*Aim.' + Ow; 
end

S0 = S(:,:,end);