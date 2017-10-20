function L = controllaw_infinitehorizon(Ae,B,Q,R,N)

% initialize parameters and cost-to-go
L = zeros(size(B,2),size(Ae,1),N-1);
Lold = zeros(size(B,2),size(Ae,1));
V = Q;

for k = N-1:-1:1
    % calculate gain
    L(:,:,k) = pinv(R + B'*V*B)*B'*V*Ae;
    
    % update cost (running)
    V = Q +Ae'*V*(Ae-B*L(:,:,k));

    % stop loop when gains do not change anymore
    if (norm(L(:,:,k)-Lold) < 1e-6) && k > 10
        break;
    end
    
    Lold = L(:,:,k);
end

% extend gain if necessary
L(:,:,1:k)= repmat(L(:,:,k),[1,1,k]);
end