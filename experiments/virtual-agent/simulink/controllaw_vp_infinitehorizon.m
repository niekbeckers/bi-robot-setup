function L = controllaw_vp_infinitehorizon(A,B,Q,R,N)

% initialize parameters and cost-to-go
Ltmp = zeros(size(B,2),size(A,1),N-1);

% initialize cost-to-go
V = Q;

lastk = 1;
for k = N-1:-1:1
    % calculate gain
    Ltmp(:,:,k) = pinv(R + B'*V*B)*B'*V*A;
    
    % update cost (running)
    V = Q +A'*V*(A-B*Ltmp(:,:,k));

    % stop loop when gains do not change anymore
    if (k < N-1)
        if (norm(Ltmp(:,:,k)-Ltmp(:,:,k+1)) < 1e-6)
            lastk = k;
            break;
        end
    end
end

% infinite horizon gain
L = Ltmp(:,:,lastk);
end