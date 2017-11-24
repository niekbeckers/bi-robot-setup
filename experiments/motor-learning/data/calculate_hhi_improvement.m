function [improvement, relperformance] = calculate_hhi_improvement(rmse,idx_connected,idx_single,varargin)
%% [improvement, relperformance] = hhi_improvement(rmse,idx_connected,idx_single,hfig)
% calculate improvement and relative performance

% parse inputs
p = inputParser;
p.addOptional('hfig',[]);
p.parse(varargin{:});
hfig = p.Results.hfig;

try 
    % select connected and single RMSe
    rmse_connected = rmse(idx_connected,:);
    rmse_single = rmse(idx_single,:);

    % calculate improvement in preceding connected trial with respect to
    % single trial
    if any(idx_connected(:)) % connected trial
        improvement = [1-rmse_connected(:,1)./rmse_single(:,1) 1-rmse_connected(:,2)./rmse_single(:,2)];
    else % no connected trials found, set improvement to NaN's
        improvement = NaN(numel(idx_single),2);
    end
    relperformance = [1-rmse_single(:,2)./rmse_single(:,1) 1-rmse_single(:,1)./rmse_single(:,2)]; 
catch e
%     rethrow(e);
    keyboard
end

if ~isempty(hfig)
    plotHHI(relperformance,improvement,hfig);
end

end