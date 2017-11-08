function [A,B,H] = dynamics(dt,M,tu,td,D)
%% [A,B,H] = dynamics(dt,m,tu,td,D)
% Return discrete-time state space matrices of the environment and internal
% model (which only accounts for a fraction of gamma of the disturbance).
% state vector
% x = [px py vx vy fx fy gx gy tx ty pfx pfy]^T

if isscalar(M)
    M = diag([M M]);
end

% x = [px py vx vy fx fy ptx pty vtx vty]
  Ac = [0 0 1   0   0     0     0 0 0 0;...
        0 0 0   1   0     0     0 0 0 0;...
        0 0 D(1,1)/M(1,1) D(1,2)/M(1,1) 1/M(1,1)   0     0 0 0 0;...
        0 0 D(2,1)/M(2,2) D(2,2)/M(2,2) 0     1/M(2,2)   0 0 0 0;...
        0 0 0   0   -1/tu 0     0 0 0 0;...
        0 0 0   0   0     -1/tu 0 0 0 0;...
        0 0 0   0   0     0     0 0 1 0;...
        0 0 0   0   0     0     0 0 0 1;...
        0 0 0   0   0     0     0 0 0 0;...
        0 0 0   0   0     0     0 0 0 0];

Bc = [0 0;0 0;0 0;0 0;1/tu 0;0 1/tu;0 0;0 0; 0 0; 0 0; 0 0; 0 0; 0 0; 0 0];

Cc = [1 0 0 0 0 0 0 0 0 0 0 0 0 0;...
      0 1 0 0 0 0 0 0 0 0 0 0 0 0;...
      0 0 1 0 0 0 0 0 0 0 0 0 0 0;...
      0 0 0 1 0 0 0 0 0 0 0 0 0 0;...
      0 0 0 0 0 0 1 0 0 0 0 0 0 0;...
      0 0 0 0 0 0 0 1 0 0 0 0 0 0;...
      0 0 0 0 0 0 0 0 1 0 0 0 0 0;...
      0 0 0 0 0 0 0 0 0 1 0 0 0 0];
  
%% discretization
% environment
Ntd = round(td/dt);

% x = [px py vx vy fx fy ptx pty vtx vty pfx pfy ptfx ptfy]
A = zeros(14,14);
A(1:10,1:10) = eye(size(Ac)) + Ac*dt;

% target movement prediction
A(11,1) = 1;
A(12,2) = 1;
A(11,3) = dt+td;
A(12,4) = dt+td;
A(13,7) = 1;
A(14,8) = 1;
A(13,9) = dt+td;
A(14,10) = dt+td;

B = dt*Bc;
H = Cc;
end