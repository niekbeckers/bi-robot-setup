function [improvement, relperformance] = calculate_improvement_vp(rmse,rmse_vp,idx_connected,idx_single,varargin)
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
    
    rmse_connected_vp = rmse_vp(idx_connected,:);
    rmse_single_vp = rmse_vp(idx_single,:);
    
    % calculate improvement in preceding connected trial with respect to
    % single trial
    if any(idx_connected(:)) % connected trial
        improvement = [1-rmse_connected(:,1)./rmse_single(:,1) 1-rmse_connected(:,2)./rmse_single(:,2)];
        improvement_vp = [1-rmse_connected_vp(:,1)./rmse_single_vp(:,1) 1-rmse_connected_vp(:,2)./rmse_single_vp(:,2)];
%         improvement = [rmse_single(:,1)-rmse_connected(:,1) rmse_single(:,2)-rmse_connected(:,2)];
%         improvement_vp = [rmse_single_vp(:,1)-rmse_connected_vp(:,1) rmse_single_vp(:,2)-rmse_connected_vp(:,2)];
    else % no connected trials found, set improvement to NaN's
        improvement = NaN(numel(idx_single),2);
        improvement_vp = NaN(numel(idx_single),2);
    end
    
    improvement = [improvement improvement_vp];
    
%     relperformance = [1-rmse_single(:,2)./rmse_single(:,1) 1-rmse_single(:,1)./rmse_single(:,2)]; 

    relperformance = [1-rmse_single_vp(:,1)./rmse_single(:,1) 1-rmse_single_vp(:,2)./rmse_single(:,2)]; 
    relperformance_vp = [1-rmse_single(:,1)./rmse_single_vp(:,1) 1-rmse_single(:,2)./rmse_single_vp(:,2)]; 
%     relperformance = [rmse_single(:,1)-rmse_single_vp(:,1) rmse_single(:,2)-rmse_single_vp(:,2)]; 
%     relperformance_vp = [rmse_single(:,1)-rmse_single_vp(:,1) rmse_single(:,2)-rmse_single_vp(:,2)]; 

    relperformance = [relperformance relperformance_vp];
catch e
%     rethrow(e);
    disp(getReport(e))
    keyboard
end


end