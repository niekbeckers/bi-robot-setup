function L = controllaw(Ae,B,Q,R,N)

% initialize parameters and cost-to-go
L = zeros(size(B,2),size(Ae,1),N-1);
% initialize cost-to-go
V = Q;

lastk = 1;
for k = N-1:-1:1
    % calculate gain
    L(:,:,k) = pinv(R + B'*V*B)*B'*V*Ae;
    
    % update cost (running)
    V = Q +Ae'*V*(Ae-B*L(:,:,k));

    % stop loop when gains do not change anymore
    if (k < N-1)
        if (norm(L(:,:,k)-L(:,:,k+1)) < 1e-6)
            lastk = k;
            break;
        end
    end
end

% extend gain if necessary
L(:,:,1:lastk)= repmat(L(:,:,lastk),[1,1,lastk]);

end