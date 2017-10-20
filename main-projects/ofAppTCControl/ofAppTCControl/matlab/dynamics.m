function [A,B,H] = dynamics(dt,M,tu,td,D)
%% [A,B,H] = dynamics(dt,m,tu,td,D)
% Return discrete-time state space matrices of the environment and internal
% model (which only accounts for a fraction of gamma of the disturbance). A
% time delay is also used.
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
Atmp = zeros(14,14);
Atmp(1:10,1:10) = eye(size(Ac)) + Ac*dt;
Atmp(11,1) = 1;
Atmp(12,2) = 1;
Atmp(11,3) = dt+td;
Atmp(12,4) = dt+td;
Atmp(13,7) = 1;
Atmp(14,8) = 1;
Atmp(13,9) = dt+td;
Atmp(14,10) = dt+td;

Btmp = dt*Bc;
Htmp = Cc;

%% sensory time delay
% add time delay in A matrix by extending the system matrix using hidden state for the delay
% see Izawa and Shadmehr, 2008

% add time delay


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