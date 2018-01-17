function [A,B,H] = dynamics_vp(dt,M,tu,td,tp,D)
%% [A,B,H] = dynamics(dt,m,tu,td,D)
% Return discrete-time state space matrices of the environment and internal
% model (which only accounts for a fraction of gamma of the disturbance).
% state vector
% x = [px py vx vy fx fy tx ty vtx vty]^T

if isscalar(M)
    M = diag([M M]);
end

% x = [px py vx vy fx fy ptx pty vtx vty]
% short hand notations
m11 = M(1,1);
m22 = M(2,2);

d11 = D(1,1);
d12 = D(1,2);
d21 = D(2,1);
d22 = D(2,2);

%% system matrices, continuous time

% environment dynamics
% x = [px py vx vy fx fy tx ty vtx vty]^T
Ac = [0 0 1         0        0     0     0 0 0 0;...
      0 0 0         1        0     0     0 0 0 0;...
      0 0 d11/m11   d12/m11  1/m11 0     0 0 0 0;...
      0 0 d21/m22   d22/m22  0     1/m22 0 0 0 0;...
      0 0 0         0        -1/tu 0     0 0 0 0;...
      0 0 0         0        0     -1/tu 0 0 0 0;...
      0 0 0         0        0     0     0 0 0 0;...
      0 0 0         0        0     0     0 0 0 0;...
      0 0 0         0        0     0     0 0 0 0;...
      0 0 0         0        0     0     0 0 0 0];

Bc = [0 0;0 0;0 0;0 0;1/tu 0;0 1/tu;0 0;0 0;0 0;0 0];

Cc = [1 0 0 0 0 0 0 0 0 0;...
      0 1 0 0 0 0 0 0 0 0;...
      0 0 1 0 0 0 0 0 0 0;...
      0 0 0 1 0 0 0 0 0 0;...
      0 0 0 0 0 0 1 0 0 0;...
      0 0 0 0 0 0 0 1 0 0;...
      0 0 0 0 0 0 0 0 1 0;...
      0 0 0 0 0 0 0 0 0 1];

%% discretization
% environment
Atmp = eye(size(Ac)) + Ac*dt;
Btmp = dt*Bc;
Htmp = Cc;

%% add target extrapolation
if tp > 0
    Atmp(11,7) = 1;
    Atmp(11,9) = tp;

    Atmp(12,8) = 1;
    Atmp(12,10) = tp;

    Atmp = [Atmp zeros(size(Atmp,1),2)];

    Btmp = [Btmp; zeros(2,2)];
    Htmp = [Htmp zeros(size(Htmp,1),2)];
end

%% sensory time delay
% add time delay in A matrix by extending the system matrix using hidden state for the delay
% see Izawa and Shadmehr, 2008

% add time delay
Ntd = round(td/dt);

if Ntd > 0 
    nx = size(Atmp,1);

    Atd = zeros(nx+nx*Ntd,nx+nx*Ntd);
    Atd(1:nx,1:nx) = Atmp;
    for ii=1:Ntd
        Atd(nx*ii+1:nx*ii+nx,nx*(ii-1)+1:nx*ii) = eye(nx);
    end
    B = [Btmp; zeros(nx*Ntd,2)];
    H = [zeros(size(Htmp,1),nx*Ntd) Htmp];  % you can only observe the delayed state
    A = Atd;
else
    A = Atmp;
    B = Btmp;
    H = Htmp;
end

end