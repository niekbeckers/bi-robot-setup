function [A,B,H] = dynamics(dt,M,tu,td,tp,D,Kimp)

%% function [A,B,H] = dynamics(dt,M,tu,td,tp,D,Kimp)
% Return discrete-time state space matrices of a point mass in a 2D plane.
% A time delay is introduced using the method presented by Izawa and
% Shadmehr (2008)
%
% INPUT
% dt    Time step
% M     Mass matrix
% tu    Muscle dynamics time constant
% td    Sensory time delay 
% Kimp  Arm stiffness/impedance
% D     Disturbance (curl field)
%
% OUTPUT
% A,B,H System, input and observation matrix
%
% State vector
% x = [px py vx vy fx fy tx ty]^T
% Output vector
% y = [px py vx vy tx ty]^T
%
% Niek Beckers

% check inputs
if isscalar(M)
    M = diag([M M]);
end

if (nargin < 7) || isempty(Kimp)
   Kimp = diag([0 0]); 
end
if isscalar(Kimp)
    Kimp = diag([Kimp Kimp]);
end

% short hand notations
m11 = M(1,1);
m22 = M(2,2);

d11 = D(1,1);
d12 = D(1,2);
d21 = D(2,1);
d22 = D(2,2);

%% system matrices, continuous time

% environment dynamics
Ac = [0 0 1         0        0     0     0 0 0 0;...
      0 0 0         1        0     0     0 0 0 0;...
      0 0 d11/m11   d12/m11  1/m11 0     0 0 0 0;...
      0 0 d21/m22   d22/m22  0     1/m22 0 0 0 0;...
      0 0 0         0        -1/tu 0     0 0 0 0;...
      0 0 0         0        0     -1/tu 0 0 0 0;...
      0 0 0         0        0     0     0 0 1 0;...
      0 0 0         0        0     0     0 0 0 1;...
      0 0 0         0        0     0     0 0 0 0;...
      0 0 0         0        0     0     0 0 0 0];

Bc = [0 0;0 0;0 0;0 0;1/tu 0;0 1/tu;0 0;0 0;0 0;0 0];

Cc = [1 0 0 0 0 0 0 0 0 0;... % xe
      0 1 0 0 0 0 0 0 0 0;... % ye
      0 0 1 0 0 0 0 0 0 0;... % vxe
      0 0 0 1 0 0 0 0 0 0;... % vye
      0 0 0 0 0 0 1 0 0 0;... % tx
      0 0 0 0 0 0 0 1 0 0;... % ty
      0 0 0 0 0 0 0 0 1 0;... % txdot
      0 0 0 0 0 0 0 0 0 1];   % tydot

  
%% discretization
% environment
Atmp = eye(size(Ac)) + Ac*dt;
Btmp = dt*Bc;
Htmp = Cc;

%% add target extrapolation
% if tp > 0
%     Atmp(11,7) = 1;
%     Atmp(11,9) = tp;
% 
%     Atmp(12,8) = 1;
%     Atmp(12,10) = tp;
% 
%     Atmp = [Atmp zeros(size(Atmp,1),2)];
% 
%     Btmp = [Btmp; zeros(2,2)];
%     Htmp = [Htmp zeros(size(Htmp,1),2)];
% end

%% arm stiffness
% added arm stiffnes term (in discrete time, I added the stiffness as
% follows:
% Fimp = Kimp * (xk-xk1) = Kimp * vk * dt 
% basically extra damping force
% Atmp(3,3) = Atmp(3,3) -k11*dt/m11;
% Atmp(4,4) = Atmp(4,4) -k22*dt/m22;

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